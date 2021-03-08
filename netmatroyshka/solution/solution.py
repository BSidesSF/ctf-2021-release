import zipfile
from scapy import all as scapy
from scapy.layers import http
from scapy.layers import tftp
import IPython
import io
import struct
import zlib


class ZippedPCAPLoader:

    def __init__(self, b, pwd=None):
        if isinstance(b, bytes):
            b = io.BytesIO(b)
        zf = zipfile.ZipFile(b)
        fname = None
        for info in zf.infolist():
            if info.filename.endswith('.pcap'):
                fname = info.filename
                break
        fp = zf.open(fname, mode='r', pwd=pwd)
        self.pcap = scapy.rdpcap(fp)


class Stage8Loader(ZippedPCAPLoader):

    def __init__(self, fname):
        super().__init__(fname)

    def get_next(self):
        """Return a byte stream representing the payload."""
        pkts = [p for p in self.pcap if p[scapy.IP].src in ('10.128.0.3', '10.128.0.2') and p[scapy.IP].dst in ('10.128.0.3', '10.128.0.2')]
        pkts = scapy.PacketList(pkts)
        sess = pkts.sessions()['TCP 10.128.0.3:80 > 10.128.0.2:39864']
        sess_contents = b''.join(p[scapy.Raw].load for p in sess if p.haslayer(scapy.Raw))
        resp = http.HTTPResponse()
        return resp.do_dissect(sess_contents)


class Stage7Loader(ZippedPCAPLoader):

    def __init__(self, fname):
        super().__init__(fname, b'goodluck,havefun')

    def get_next(self):
        # FTP, it's just the whole connection
        sess = self.pcap.sessions()['TCP 10.128.0.3:20 > 10.128.0.2:58025']
        return b''.join(p.load for p in sess.getlayer(scapy.Raw))


class Stage6Loader(ZippedPCAPLoader):

    def get_next(self):
        sess = self.pcap.sessions()['TCP 10.128.0.3:873 > 10.128.0.2:57536']
        raw = b''.join(p.load for p in sess.getlayer(scapy.Raw))
        pk_start = raw.index(b'PK')
        chunk_len = struct.unpack('<I', raw[pk_start-4:pk_start])[0]
        zip_bytes = raw[raw.index(b'PK'):]
        first = zip_bytes[:chunk_len]
        left = zip_bytes[chunk_len:]
        chunk_len = struct.unpack('<I', zip_bytes[:4])[0]
        first += left[4:4+chunk_len]
        return first


class Stage5Loader(ZippedPCAPLoader):

    def get_next(self):
        sess = self.pcap.sessions()['UDP 10.128.0.3:33792 > 10.128.0.2:49406']
        pkts = [tftp.TFTP(p.load) for p in sess]
        data_pkts = [p.load for p in pkts if p.haslayer(tftp.TFTP_DATA)]
        # decode netascii
        return b''.join(data_pkts).replace(b'\x0d\x0a', b'\x0a').replace(b'\x0d\x00', b'\x0d')


class Stage4Loader(ZippedPCAPLoader):

    def get_next(self):
        sess = self.pcap.sessions()['TCP 10.128.0.3:445 > 10.128.0.2:59840']
        include = False
        stream = []
        for p in sess:
            if p.haslayer(scapy.SMB2_Header) and p[scapy.SMB2_Header].Command == 2048:
                include = True
            if include and p.haslayer(scapy.NBTSession):
                stream.append(bytes(p[scapy.NBTSession]))
        buf = b''.join(stream)
        l = 26406
        return buf[buf.index(b'PK'):][:l]


class Stage3Loader(ZippedPCAPLoader):

    @staticmethod
    def parse_pack_info(buf):
        n = buf[0]
        t = (n & 0x70) >> 4
        lb = [n & 0b1111]
        buf = buf[1:]
        while n & 0x80:
            n = buf[0]
            buf = buf[1:]
            lb = [n & 0x7f] + lb
        l = 0
        for b in lb[:len(lb)-1]:
            l = l << 7
            l |= b
        l = l << 4
        l |= lb[len(lb)-1]
        return t, l, buf

    @staticmethod
    def decompress_next(buf):
        l = 1
        while l <= len(buf):
            try:
                dc = zlib.decompress(buf[:l])
                return l, dc
            except zlib.error:
                l += 1

    def get_next(self):
        sess = self.pcap.sessions()['TCP 10.128.0.3:9418 > 10.128.0.2:59704']
        buf = []
        for p in sess:
            if p.haslayer(scapy.Raw):
                buf.append(bytes(p[scapy.Raw]))
        buf = b''.join(buf)
        netbuf = buf
        pack_start = buf.index(b'PACK')-5
        buf = buf[pack_start:]
        pack_data = []
        while buf:
            l = int(buf[:4], 16)
            sl, buf = buf[:l], buf[l:]
            if l <= 6:
                break
            pack_data.append(sl[5:])
        pack_data = b''.join(pack_data)
        # Skip the header
        buf = pack_data[12:]
        while True:
            t, l, buf = self.parse_pack_info(buf)
            raw_len, dc = self.decompress_next(buf)
            assert(len(dc) == l)
            buf = buf[raw_len:]
            if t == 3 and dc[:2] == b'PK':
                return dc


class Stage2Loader(ZippedPCAPLoader):

    @staticmethod
    def c2(pkt):
        if isinstance(pkt.rdata, list):
            return bytes.fromhex((b''.join(pkt.rdata)).decode('ascii'))
        return bytes.fromhex(pkt.rdata.replace(b'.c2.challenges.bsidessf.net.', b'').replace(b'.', b'').decode('ascii'))

    @staticmethod
    def decode_bytes(b):
        return bytes.fromhex(b.replace(b'.c2.challenges.bsidessf.net.', b'').replace(b'.', b'').decode('ascii'))

    @classmethod
    def c2_pkt(cls, pkt):
        if pkt.haslayer(scapy.DNSRR):
            if isinstance(pkt[scapy.DNSRR].rdata, list):
                return cls.decode_bytes(b''.join(pkt[scapy.DNSRR].rdata))
            return cls.decode_bytes(pkt[scapy.DNSRR].rdata)
        if pkt.haslayer(scapy.DNSRRMX):
            return cls.decode_bytes(pkt[scapy.DNSRRMX].exchange)


    def get_next(self):
        pkts = [p for p in self.pcap
                if p.haslayer(scapy.UDP) and
                    p.haslayer(scapy.DNS) and
                    p[scapy.DNSQR].qname != b'dnscat2.c2.challenges.bsidessf.net.']
        c2_data = [self.c2_pkt(p) for p in pkts]
        c2_data = [p[9:] for p in c2_data if p is not None]
        data_stream = b''.join(c2_data)
        return data_stream[data_stream.index(b'PK\x03\x04'):]


class Stage1Loader(ZippedPCAPLoader):

    def get_flag(self):
        pkts = self.pcap.filter(lambda p: p[scapy.IP].src=='10.128.0.2' and p.haslayer(scapy.Raw))
        data = b''.join(p[scapy.Raw].load for p in pkts)
        flag_encoded = data.split(b'echo -e "')[1].split(b'"')[0]
        return flag_encoded.decode("unicode_escape")


if __name__ == '__main__':
    stage8 = Stage8Loader('../distfiles/8.zip')
    stage7 = Stage7Loader(stage8.get_next())
    stage6 = Stage6Loader(stage7.get_next())
    stage5 = Stage5Loader(stage6.get_next())
    stage4 = Stage4Loader(stage5.get_next())
    stage3 = Stage3Loader(stage4.get_next())
    stage2 = Stage2Loader(stage3.get_next())
    stage1 = Stage1Loader(stage2.get_next())
    print(stage1.get_flag())
