import flask
import uuid
import os
import io
import base64
import pickle
import json
import hmac
import hashlib

import crypter

app = flask.Flask(__name__)
app.config.from_object('config')


MAC_SEP = b'!'


def upload_data(cfg, fp):
    """Upload and encrypt data.

    Returns tuple of:
        - file_path
        - key as hex
        - iv as hex
    """
    os.makedirs(cfg['BASE_DIR'], mode=0o700, exist_ok=True)
    fname = str(uuid.uuid4())
    file_path = os.path.join(cfg['BASE_DIR'], fname)
    c = crypter.Crypter()
    with open(file_path, 'wb') as dst:
        c.encrypt(fp, dst)
    return fname, c.key, c.iv


def retrieve_data(cfg, fname, key, iv, dst=None):
    """Locate and decrypt data.

    Returns destination data.
    """
    if '..' in fname:
        raise ValueError('Directory traversal detected!')
    dst = dst or io.BytesIO()
    fpath = os.path.join(cfg['BASE_DIR'], fname)
    c = crypter.Crypter(key=key, iv=iv)
    with open(fpath, 'rb') as fp:
        c.decrypt(fp, dst)
    return dst


def pack_key(cfg, key, iv):
    """Pack a key and iv."""
    d = pickle.dumps({'key': key, 'iv': iv})
    mac = hmac.new(
            cfg['AUTH_KEY'].encode('utf-8'),
            msg=d,
            digestmod=hashlib.sha256).digest()[:16]
    return (
            base64.urlsafe_b64encode(mac) +
            MAC_SEP +
            base64.urlsafe_b64encode(d)).decode('utf-8')


def unpack_key(cfg, data):
    """Retrieve key and iv."""
    mac, d = data.encode('utf-8').split(MAC_SEP)
    mac = base64.urlsafe_b64decode(mac)
    d = base64.urlsafe_b64decode(d)
    expected = hmac.new(
            cfg['AUTH_KEY'].encode('utf-8'),
            msg=d,
            digestmod=hashlib.sha256).digest()[:16]
    if not hmac.compare_digest(mac, expected):
        app.logger.warn('Invalid MAC: ' + mac.hex() + ' ' + expected.hex())
        raise ValueError('Error deserializing pickled data: Invalid MAC')
    keyd = pickle.loads(d)
    return keyd['key'], keyd['iv']


def json_response(d):
    resp = flask.make_response(json.dumps(d))
    resp.headers.add('Content-type', 'application/json')
    return resp


@app.route('/', defaults={'path': ''})
@app.route('/<path:path>')
def index(path):
    return flask.render_template(
            'index.html',
            fname=os.path.abspath(__file__))


@app.route('/upload', methods=['POST'])
def upload():
    try:
        f = flask.request.form['paste']
    except KeyError:
        flask.abort(400)
    path, key, nonce = upload_data(app.config, io.BytesIO(f.encode('utf-8')))
    return json_response({
        'name': path,
        'key': pack_key(app.config, key, nonce),
    })


@app.route('/load')
def load_file():
    try:
        key = flask.request.args['key']
        path = flask.request.args['file']
    except (KeyError, ValueError):
        flask.abort(400)
    try:
        key, nonce = unpack_key(app.config, key)
    except Exception as ex:
        app.logger.warning('Error retrieving key: ' + str(ex))
        flask.abort(403, str(ex))
    try:
        fp = retrieve_data(app.config, path, key, nonce)
        fp.seek(0)
        resp = flask.make_response(fp.read())
        resp.headers.add('Content-type', 'application/octet-stream')
        return resp
    except FileNotFoundError:
        flask.abort(404)
    except PermissionError:
        flask.abort(403)
