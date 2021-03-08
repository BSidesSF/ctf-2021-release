```
brenrigh@lambda /tmp $ xzcat chimera.bin.img.xz > img.bin
brenrigh@lambda /tmp $ sudo mount -t ext2 -o loop,ro img.bin /mnt/other/
Password:
brenrigh@lambda /tmp $ ls /mnt/other/
002.jpg  005.jpg  008.jpg  011.jpg  016.jpg  019.jpg  022.jpg  025.jpg  029.jpg  032.jpg  035.jpg  038.jpg  041.jpg  045.jpg  048.jpg  051.jpg  054.jpg  057.jpg   grot.jpg    lost+found
003.jpg  006.jpg  009.jpg  012.jpg  017.jpg  020.jpg  023.jpg  026.jpg  030.jpg  033.jpg  036.jpg  039.jpg  042.jpg  046.jpg  049.jpg  052.jpg  055.jpg  058.jpg   images.jpg  shim.jpg
004.jpg  007.jpg  010.jpg  013.jpg  018.jpg  021.jpg  024.jpg  028.jpg  031.jpg  034.jpg  037.jpg  040.jpg  043.jpg  047.jpg  050.jpg  053.jpg  056.jpg  1024.jpg  key.docx    tree.jpg
brenrigh@lambda /tmp $ python /mnt/other/key.docx
zip key: eternalfiresofchimera
brenrigh@lambda /tmp $ sudo umount /mnt/other/
brenrigh@lambda /tmp $ sudo mount -t msdos -o loop,ro img.bin /mnt/other/
brenrigh@lambda /tmp $ pdfdetach -list /mnt/other/flag.pdf
1 embedded files
1: flag.zip
brenrigh@lambda /tmp $ pdfdetach -save 1 /mnt/other/flag.pdf
brenrigh@lambda /tmp $ unzip flag.zip
Archive:  flag.zip
[flag.zip] flag.png password:
  inflating: flag.png
```

CTF{digital_metamorphism}
