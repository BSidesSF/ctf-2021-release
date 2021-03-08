package main

import (
	"crypto/ecdsa"
	"fmt"
	"html/template"
	"io"
	"io/ioutil"
	"log"
	insecurerand "math/rand"
	"net/http"
	"net/url"
	"os"

	jwt "github.com/dgrijalva/jwt-go"
)

const (
	COOKIE_NAME            = "loginsid"
	CONT_PATH              = "/setsid"
	SUBMIT_PATH            = "/submit"
	MODERATE_PATH          = "/moderate"
	INDEX_TMPL             = "index.html"
	MODERATE_TMPL          = "moderate.html"
	SUBMIT_TMPL            = "submit.html"
	BASE_TMPL              = "base.html"
	LOGIN_TMPL             = "login.html"
	LOGIN_PATH             = "/login"
	ADMIN_USER             = "admin"
	ENV_LOGIN_SVC_URL      = "LOGIN_SERVICE"
	ENV_WEBBOT_SVC_URL     = "WEBBOT_SERVICE"
	ENV_BASE_URL           = "BASE_URL"
	ENV_FLAG               = "FLAG"
	ENV_IMG_DIR            = "IMG_DIR"
	DEFAULT_FLAG           = "NOT_REAL_FLAG_CONTACT_ADMIN"
	DEFAULT_LOGIN_SVC_URL  = "http://localhost:5123/"
	DEFAULT_WEBBOT_SVC_URL = "http://localhost:3000/"
	DEFAULT_BASE_URL       = "http://localhost:5124/"
	DEFAULT_IMG_DIR        = "./img"
)

var (
	jwtKey          *ecdsa.PublicKey
	templates       map[string]*template.Template
	loginServiceURL *url.URL
	webbotURL       *url.URL
	baseURL         *url.URL
	imgDir          string
	secretFlag      string
)

func loadECDSAPublicKey(fname string) *ecdsa.PublicKey {
	if buf, err := ioutil.ReadFile(fname); err != nil {
		panic(err)
	} else {
		if k, err := jwt.ParseECPublicKeyFromPEM(buf); err != nil {
			panic(err)
		} else {
			return k
		}
	}
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
		return jwtKey, nil
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
			if !mapClaims.VerifyAudience("authtok", true) {
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

func handleIndex(w http.ResponseWriter, r *http.Request) {
	username, _ := usernameFromSession(r)
	pickedImages := getSomeImages(8)
	render(w, INDEX_TMPL, struct {
		Username string
		Images   []string
	}{
		Username: username,
		Images:   pickedImages,
	})
}

func getSomeImages(num int) []string {
	files, err := ioutil.ReadDir(imgDir)
	if err != nil {
		log.Printf("Error reading dir %s: %s", imgDir, err)
		return nil
	}
	// Remove hidden items and copy names
	fnames := make([]string, 0, len(files))
	for _, fi := range files {
		name := fi.Name()
		if name[0] == '.' {
			continue
		}
		fnames = append(fnames, "/img/"+name)
	}
	insecurerand.Shuffle(len(fnames), func(i, j int) {
		fnames[i], fnames[j] = fnames[j], fnames[i]
	})
	return fnames[:num]
}

func handleSetSid(w http.ResponseWriter, r *http.Request) {
	if tok := getQueryParam(r, "authtok"); tok != "" {
		if username, err := usernameFromJWT(tok); err != nil {
			http.Error(w, "Authentication Failure", http.StatusForbidden)
		} else {
			log.Printf("Successful login as %s", username)
			http.SetCookie(w, &http.Cookie{
				Name:     COOKIE_NAME,
				Value:    tok,
				HttpOnly: true,
			})
			http.Redirect(w, r, "/", http.StatusFound)
			return
		}
	}
	http.Error(w, "Bad Request", http.StatusBadRequest)
}

func handleModerate(w http.ResponseWriter, r *http.Request) {
	if username, err := usernameFromSession(r); err != nil {
		http.Error(w, "Not Authorized", http.StatusForbidden)
		return
	} else {
		if username != ADMIN_USER {
			http.Error(w, "Not Authorized", http.StatusForbidden)
			return
		}
		if u := getQueryParam(r, "url"); u == "" {
			http.Error(w, "Bad Request", http.StatusBadRequest)
			return
		} else {
			render(w, MODERATE_TMPL, struct {
				Username string
				ImageURL string
			}{
				Username: username,
				ImageURL: u,
			})
		}
	}
}

type submitTemplate struct {
	Username  string
	SubmitURL string
	Error     string
	Info      string
}

func handleSubmit(w http.ResponseWriter, r *http.Request) {
	if username, err := usernameFromSession(r); err != nil {
		renderLoginPage(w, r)
		return
	} else {
		if r.Method == "POST" {
			handleSubmitPost(w, r, username)
			return
		}
		render(w, SUBMIT_TMPL, &submitTemplate{
			Username: username,
		})
	}
}

func handleSubmitPost(w http.ResponseWriter, r *http.Request, username string) {
	submitURL := r.FormValue("url")
	if _, err := url.Parse(submitURL); submitURL == "" || err != nil {
		render(w, SUBMIT_TMPL, &submitTemplate{
			Username:  username,
			Error:     "Must provide valid URL",
			SubmitURL: submitURL,
		})
		return
	}
	pingWebbotAsync(submitURL)
	log.Printf("%s submitted by %s", submitURL, username)
	render(w, SUBMIT_TMPL, &submitTemplate{
		Username: username,
		Info:     fmt.Sprintf("Successfully submitted %s", submitURL),
	})
}

func pingWebbotAsync(target string) {
	go func() {
		u := &url.URL{}
		*u = *webbotURL
		u.Path = "/submit"
		v := url.Values{}
		v.Set("url", target)
		log.Printf("webbot: submitting %s", target)
		if resp, err := http.PostForm(u.String(), v); err != nil {
			log.Printf("webbot: error in making request: %s", err)
		} else {
			if resp.StatusCode != 200 {
				log.Printf("webbot: error submitting request: %d %s", resp.StatusCode, resp.Status)
			}
		}
	}()
}

func handleFlag(w http.ResponseWriter, r *http.Request) {
	if username, err := usernameFromSession(r); err != nil {
		http.Error(w, "Not Authorized", http.StatusForbidden)
		return
	} else {
		if username != ADMIN_USER {
			http.Error(w, "Not Authorized", http.StatusForbidden)
			return
		}
	}
	fmt.Fprintf(w, "FLAG: %s", secretFlag)
}

func getQueryParam(r *http.Request, name string) string {
	if toks, ok := r.URL.Query()[name]; ok {
		if len(toks) == 1 {
			return toks[0]
		}
	}
	return ""
}

func render(w io.Writer, name string, data interface{}) error {
	if tmpl, ok := templates[name]; ok {
		if err := tmpl.Execute(w, data); err != nil {
			log.Printf("Error rendering template %s: %s", name, err)
			return err
		}
		return nil
	}
	err := fmt.Errorf("No such template: %s", name)
	log.Println(err)
	return err
}

func renderLoginPage(w http.ResponseWriter, r *http.Request) {
	loginSvcURL := getLoginSvcURL()
	render(w, LOGIN_TMPL, struct {
		LoginURL string
		Username string
	}{
		LoginURL: loginSvcURL,
	})
}

func getLoginSvcURL() string {
	localURL := &url.URL{}

	// Redir
	*localURL = *baseURL
	localURL.Path = CONT_PATH
	redirURL := localURL.String()

	// Now do the login URL
	*localURL = *loginServiceURL
	localURL.Path = "/check"
	qry := url.Values{}
	qry.Set("continue", redirURL)
	localURL.RawQuery = qry.Encode()
	return localURL.String()
}

func getEnvOrDefault(key, def string) string {
	if val, ok := os.LookupEnv(key); ok {
		return val
	}
	return def
}

func mustURL(surl string) *url.URL {
	if u, err := url.Parse(surl); err == nil {
		return u
	} else {
		panic(err)
	}
}

func main() {
	log.Fatal(http.ListenAndServe(":5124", logRequest(http.DefaultServeMux)))
}

func logRequest(handler http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("%s %s %s\n", r.RemoteAddr, r.Method, r.URL)
		handler.ServeHTTP(w, r)
	})
}

func init() {
	jwtKey = loadECDSAPublicKey("jwtkey.pem")
	http.HandleFunc("/", handleIndex)
	http.HandleFunc(CONT_PATH, handleSetSid)
	http.HandleFunc(MODERATE_PATH, handleModerate)
	http.HandleFunc(SUBMIT_PATH, handleSubmit)
	http.HandleFunc(LOGIN_PATH, renderLoginPage)
	http.HandleFunc("/flag.txt", handleFlag)
	fs := http.FileServer(http.Dir("./static"))
	http.Handle("/static/", http.StripPrefix("/static/", fs))
	imgDir = getEnvOrDefault(ENV_IMG_DIR, DEFAULT_IMG_DIR)
	fs = http.FileServer(http.Dir(imgDir))
	http.Handle("/img/", http.StripPrefix("/img/", fs))
	templates = make(map[string]*template.Template)
	for _, f := range []string{INDEX_TMPL, MODERATE_TMPL, SUBMIT_TMPL, LOGIN_TMPL} {
		templates[f] = template.Must(template.ParseFiles(f, BASE_TMPL))
	}
	loginServiceURL = mustURL(getEnvOrDefault(ENV_LOGIN_SVC_URL, DEFAULT_LOGIN_SVC_URL))
	webbotURL = mustURL(getEnvOrDefault(ENV_WEBBOT_SVC_URL, DEFAULT_WEBBOT_SVC_URL))
	baseURL = mustURL(getEnvOrDefault(ENV_BASE_URL, DEFAULT_BASE_URL))
	secretFlag = getEnvOrDefault(ENV_FLAG, DEFAULT_FLAG)
}
