package main

import (
	"crypto/tls"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/pem"
	"errors"
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"os"
)

const (
	listenAddrDefault = "0.0.0.0:1300"
	certFileDefault   = "ca/chain.crt"
	keyFileDefault    = "ca/server.key"
	flagEnv           = "FLAG"
	flagDefault       = "broken_flag_contact_admin"
)

var (
	caCert      *x509.Certificate
	errNotTLS   = errors.New("Not TLS")
	errNoClient = errors.New("No Client Certificate")
	flagVal     string
	certFile    = certFileDefault
)

func CreateTLSListener(addr, certFile, keyFile string) (net.Listener, error) {
	cert, err := tls.LoadX509KeyPair(certFile, keyFile)
	if err != nil {
		return nil, err
	}
	cfg := &tls.Config{
		Certificates: []tls.Certificate{cert},
		ClientAuth:   tls.RequestClientCert,
		MinVersion:   tls.VersionTLS13,
	}
	log.Printf("Preparing to listen on %s", addr)
	return tls.Listen("tcp", addr, cfg)
}

func logRequest(handler http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("%s %s %s\n", r.RemoteAddr, r.Method, r.URL)
		handler.ServeHTTP(w, r)
	})
}

type ClientInfo struct {
	Subject string
	Issuer  string
}

func (ci *ClientInfo) String() string {
	return fmt.Sprintf("Subject: <%s> Issuer: <%s>", ci.Subject, ci.Issuer)
}

func extractTLSInfo(r *http.Request) (*ClientInfo, error) {
	if r.TLS == nil {
		return nil, errNotTLS
	}
	cstate := r.TLS
	if cstate.PeerCertificates == nil || len(cstate.PeerCertificates) == 0 {
		return nil, errNoClient
	}
	for i, cert := range cstate.PeerCertificates {
		log.Printf("Cert %d Issuer: %s", i, cert.Issuer)
		log.Printf("Cert %d Subject: %s", i, cert.Subject)
		log.Printf("Cert %d IsCA: %t", i, cert.IsCA)
	}
	// Verify signatures
	for i := 0; i < len(cstate.PeerCertificates)-1; i++ {
		if err := cstate.PeerCertificates[i].CheckSignatureFrom(cstate.PeerCertificates[i+1]); err != nil {
			new_err := fmt.Errorf("Certificate %d missing valid signature: %s", i, err)
			log.Println(new_err)
			return nil, new_err
		}
	}
	// Verify final
	finCert := cstate.PeerCertificates[len(cstate.PeerCertificates)-1]
	if !finCert.IsCA || !PkixNameEquals(finCert.Issuer, caCert.Subject) {
		err := fmt.Errorf("Expected client cert signed by CA '%s'", caCert.Subject.String())
		log.Println(err)
		return nil, err
	}
	cliCert := cstate.PeerCertificates[0]
	res := &ClientInfo{
		Issuer:  finCert.Issuer.String(),
		Subject: cliCert.Subject.String(),
	}
	log.Printf("TLS result: %s", res)
	return res, nil
}

func PkixNameEquals(a, b pkix.Name) bool {
	return a.String() == b.String()
}

func indexHandler(w http.ResponseWriter, r *http.Request) {
	info, err := extractTLSInfo(r)
	if err != nil {
		http.Error(w, err.Error(), http.StatusForbidden)
		return
	}
	fmt.Fprintf(w, "Welcome, %s.  Your flag: %s", info.Subject, flagVal)
}

func main() {
	listenAddr := listenAddrDefault
	if val, ok := os.LookupEnv("LISTEN_ADDR"); ok {
		listenAddr = val
		log.Printf("Listen addr from environment: %s", listenAddr)
	}
	keyFile := keyFileDefault
	if val, ok := os.LookupEnv("KEY_FILE"); ok {
		keyFile = val
		log.Printf("Key file from environment: %s", keyFile)
	}
	l, err := CreateTLSListener(listenAddr, certFile, keyFile)
	if err != nil {
		log.Fatalf("Error listening: %s", err)
	}
	log.Fatal(http.Serve(l, logRequest(http.DefaultServeMux)))
}

func init() {
	http.HandleFunc("/", indexHandler)
	if val, ok := os.LookupEnv("CERT_FILE"); ok {
		certFile = val
		log.Printf("Cert file from environment: %s", certFile)
	}
	caBytes, err := ioutil.ReadFile(certFile)
	if err != nil {
		panic(err)
	}
	var block *pem.Block
	for {
		block, caBytes = pem.Decode(caBytes)
		if block == nil {
			break
		}
		tmpCert, err := x509.ParseCertificate(block.Bytes)
		if err != nil {
			panic(err)
		}
		caCert = tmpCert
	}
	if flagTmp, ok := os.LookupEnv(flagEnv); ok {
		flagVal = flagTmp
		log.Printf("Setting flag to %s", flagVal)
	} else {
		flagVal = flagDefault
	}
}
