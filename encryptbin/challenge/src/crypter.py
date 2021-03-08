import io

from Crypto import Random
from Crypto.Cipher import AES


class Crypter(object):

    RNG = Random.new()
    KEY_LEN = 128//8

    def __init__(self, key=None, iv=None):
        self.key = key or self.RNG.read(self.KEY_LEN)
        self.iv = iv or self.RNG.read(8)
        assert(len(self.iv) <= 15)
        assert(len(self.key) in AES.key_size)
        self._cipher = AES.new(self.key, AES.MODE_CTR, nonce=self.iv)

    @property
    def hexkey(self):
        return self.key.hex()

    @property
    def hexiv(self):
        return self.iv.hex()

    @staticmethod
    def convert_src(s):
        if hasattr(s, 'read'):
            return s
        if isinstance(s, bytes):
            return io.BytesIO(s)
        raise TypeError("Unsupported type: " + type(s))

    def encrypt(self, src, dst):
        src = self.convert_src(src)
        while True:
            blk = src.read(4096)
            if not blk:
                try:
                    dst.flush()
                except:
                    pass
                return
            dst.write(self._cipher.encrypt(blk))

    def decrypt(self, src, dst):
        src = self.convert_src(src)
        while True:
            blk = src.read(4096)
            if not blk:
                try:
                    dst.flush()
                except:
                    pass
                return
            dst.write(self._cipher.decrypt(blk))
