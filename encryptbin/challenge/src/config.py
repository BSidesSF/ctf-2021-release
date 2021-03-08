import os

TEMPLATES_AUTO_RELOAD = True

# App specific configs
BASE_DIR = "/tmp/ebin"
AUTH_KEY = os.getenv("AUTH_KEY", "--auth-key--")
FLAG_PATH = "/home/flag/flag.txt"
