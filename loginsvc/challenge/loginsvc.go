package main

import (
	"crypto/ecdsa"
	"fmt"
	"html/template"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"net/url"
	"sync"
	"time"

	jwt "github.com/dgrijalva/jwt-go"
)

type TmplArgs struct {
	Error    string
	Success  string
	Continue string
}

const (
	REGISTER_TMPL   = "register.html"
	LOGIN_TMPL      = "login.html"
	CHECK_TMPL      = "check.html"
	BASE_TMPL       = "base.html"
	COOKIE_NAME     = "loginsvc"
	AUTHTOK_NAME    = "authtok"
	CHECK_AUTH_PATH = "/check"
)

var (
	userLookup map[string]string = make(map[string]string)
	userMutex  sync.RWMutex
	templates  map[string]*template.Template
	jwtKey     *ecdsa.PrivateKey
)

func checkUsernamePassword(username, password string) bool {
	userMutex.RLock()
	defer userMutex.RUnlock()
	if pw, ok := userLookup[username]; !ok {
		return false
	} else {
		return pw == password
	}
}

func registerUsernamePassword(username, password string) error {
	userMutex.Lock()
	defer userMutex.Unlock()
	if _, ok := userLookup[username]; ok {
		return fmt.Errorf("Username %s already exists!", username)
	}
	userLookup[username] = password
	return nil
}

func registerHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method == "POST" {
		registerPostHandler(w, r)
		return
	}
	render(w, REGISTER_TMPL, &TmplArgs{
		Continue: getContinue(r),
	})
}

func registerPostHandler(w http.ResponseWriter, r *http.Request) {
	username := r.FormValue("username")
	password := r.FormValue("password")
	password2 := r.FormValue("password2")
	if username == "" || password == "" || password2 == "" {
		render(w, REGISTER_TMPL, &TmplArgs{
			Error:    "Username and password are required.",
			Continue: getContinue(r),
		})
		return
	}
	if password != password2 {
		render(w, REGISTER_TMPL, &TmplArgs{
			Error:    "Password mismatch",
			Continue: getContinue(r),
		})
		return
	}
	if err := registerUsernamePassword(username, password); err != nil {
		render(w, REGISTER_TMPL, &TmplArgs{
			Error:    err.Error(),
			Continue: getContinue(r),
		})
		return
	}
	// Success?
	setSession(w, username)
	// Redirect to continue handler
	redirectCheckAuth(w, r)
}

func getContinue(r *http.Request) string {
	if cform := r.FormValue("continue"); cform != "" {
		if _, err := url.Parse(cform); err == nil {
			return cform
		}
	}
	if c, ok := r.URL.Query()["continue"]; ok {
		if len(c) == 1 {
			cont := c[0]
			if _, err := url.Parse(cont); err == nil {
				return cont
			}
		}
	}
	return ""
}

func redirectWithContinue(w http.ResponseWriter, r *http.Request, where string) {
	cont := getContinue(r)
	u, _ := url.Parse(where)
	if cont != "" {
		qry := u.Query()
		qry.Set("continue", cont)
		u.RawQuery = qry.Encode()
	}
	http.Redirect(w, r, u.String(), http.StatusFound)
}

func redirectCheckAuth(w http.ResponseWriter, r *http.Request) {
	redirectWithContinue(w, r, CHECK_AUTH_PATH)
}

func checkAuthHandler(w http.ResponseWriter, r *http.Request) {
	if username, err := usernameFromSession(r); err == nil {
		if cont := getContinue(r); cont != "" {
			if contUrl, err := url.Parse(cont); err == nil {
				authtok := makeAuthTokenValue(username)
				qry := contUrl.Query()
				qry.Set(AUTHTOK_NAME, authtok)
				contUrl.RawQuery = qry.Encode()
				http.Redirect(w, r, contUrl.String(), http.StatusFound)
				return
			}
		} // No continue set
		render(w, CHECK_TMPL, nil)
		return
	}
	redirectWithContinue(w, r, "/login")
}

func loginHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method == "POST" {
		loginPostHandler(w, r)
		return
	}
	render(w, LOGIN_TMPL, &TmplArgs{
		Continue: getContinue(r),
	})
}

func loginPostHandler(w http.ResponseWriter, r *http.Request) {
	username := r.FormValue("username")
	password := r.FormValue("password")
	if username == "" || password == "" {
		render(w, LOGIN_TMPL, &TmplArgs{
			Error:    "Username and password are required.",
			Continue: getContinue(r),
		})
		return
	}
	if checkUsernamePassword(username, password) {
		setSession(w, username)
		redirectCheckAuth(w, r)
		return
	}
	render(w, LOGIN_TMPL, &TmplArgs{
		Error:    "Invalid username/password.",
		Continue: getContinue(r),
	})
}

func render(w io.Writer, name string, args *TmplArgs) error {
	if args == nil {
		args = &TmplArgs{}
	}
	tmpl, ok := templates[name]
	if !ok {
		log.Printf("Could not find template %s!", name)
		return fmt.Errorf("Could not find template %s!", name)
	}
	return tmpl.Execute(w, args)
}

func setSession(w http.ResponseWriter, username string) {
	cookie := http.Cookie{
		Name:     COOKIE_NAME,
		Value:    makeLoginCookieValue(username),
		HttpOnly: true,
		Path:     "/",
	}
	http.SetCookie(w, &cookie)
}

func logoutHandler(w http.ResponseWriter, r *http.Request) {
	cookie := http.Cookie{
		Name:     COOKIE_NAME,
		Value:    "",
		HttpOnly: true,
		Expires:  time.Unix(0, 0),
		Path:     "/",
	}
	http.SetCookie(w, &cookie)
	redirectWithContinue(w, r, "/login")
}

func homeHandler(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("OK"))
}

func usernameFromSession(r *http.Request) (string, error) {
	if c, err := r.Cookie(COOKIE_NAME); err != nil {
		return "", err
	} else {
		uname, err := usernameFromJWT(c.Value)
		if err != nil {
			log.Printf("Error getting username from session: %s", err)
		}
		return uname, err
	}
}

func usernameFromJWT(tokenStr string) (string, error) {
	keyFunc := func(*jwt.Token) (interface{}, error) {
		return jwtKey.Public(), nil
	}
	parser := &jwt.Parser{
		ValidMethods: []string{"ES256"},
	}
	if tok, err := parser.Parse(tokenStr, keyFunc); err != nil {
		return "", err
	} else {
		if mapClaims, ok := tok.Claims.(jwt.MapClaims); !ok {
			return "", fmt.Errorf("Not map claims!")
		} else {
			if !mapClaims.VerifyAudience("loginsvc", true) {
				return "", fmt.Errorf("Wrong audience!")
			}
			if uname, ok := mapClaims["sub"]; !ok {
				return "", fmt.Errorf("No subject!")
			} else {
				if unames, ok := uname.(string); ok {
					return unames, nil
				} else {
					return "", fmt.Errorf("Not a string")
				}
			}
		}
	}
}

func makeLoginCookieValue(username string) string {
	return makeJWTToken(username, "loginsvc")
}

func makeAuthTokenValue(username string) string {
	return makeJWTToken(username, "authtok")
}

func makeJWTToken(username string, purpose string) string {
	now := time.Now().Unix()
	exp := time.Now().Add(31 * 24 * time.Hour).Unix()
	token := jwt.NewWithClaims(jwt.SigningMethodES256, jwt.MapClaims{
		"sub": username,
		"aud": purpose,
		"exp": exp,
		"iat": now,
		"nbf": now,
		"iss": "loginsvc",
	})
	ss, _ := token.SignedString(jwtKey)
	return ss
}

func loadECDSAPrivateKey(file string) *ecdsa.PrivateKey {
	if buf, err := ioutil.ReadFile(file); err != nil {
		panic(err)
	} else {
		if k, err := jwt.ParseECPrivateKeyFromPEM(buf); err != nil {
			panic(err)
		} else {
			return k
		}
	}
}

func main() {
	log.Fatal(http.ListenAndServe(":5123", logRequest(http.DefaultServeMux)))
}

func logRequest(handler http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("%s %s %s\n", r.RemoteAddr, r.Method, r.URL)
		handler.ServeHTTP(w, r)
	})
}

func init() {
	if err := registerUsernamePassword("admin", "Sepaijah8ohp8oos"); err != nil {
		panic(err)
	}
	http.HandleFunc("/", homeHandler)
	http.HandleFunc("/login", loginHandler)
	http.HandleFunc("/register", registerHandler)
	http.HandleFunc("/logout", logoutHandler)
	http.HandleFunc(CHECK_AUTH_PATH, checkAuthHandler)
	fs := http.FileServer(http.Dir("./static"))
	http.Handle("/static/", http.StripPrefix("/static/", fs))
	templates = make(map[string]*template.Template)
	for _, f := range []string{REGISTER_TMPL, LOGIN_TMPL, CHECK_TMPL} {
		templates[f] = template.Must(template.ParseFiles(f, BASE_TMPL))
	}
	jwtKey = loadECDSAPrivateKey("jwtkey.pem")
}
