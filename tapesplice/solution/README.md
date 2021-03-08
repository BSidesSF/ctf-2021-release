bzip file has been appended to the png file in the tar archive in the
'slack space' part of the tar archive in the remaiving 512 byte block.

```
$ hexdump -C tapesplice.tar | tail
0001f850  9a d6 6d 7e 0c 53 fc 5f  7f fb 1b 63 09 b6 4c 01  |..m~.S._...c..L.|
0001f860  cb 19 2c 00 00 00 00 49  45 4e 44 ae 42 60 82 42  |..,....IEND.B`.B|
0001f870  5a 68 39 31 41 59 26 53  59 14 c8 70 b2 00 00 00  |Zh91AY&SY..p....|
0001f880  07 80 09 00 04 00 af e1  dd 2a 20 00 22 a7 a9 b4  |.........* ."...|
0001f890  1a 99 b5 32 1a 85 1a 32  06 8d 32 34 89 fa a9 86  |...2...2..24....|
0001f8a0  77 d0 d8 0e 6c 5c 8a 02  95 d2 41 38 bd 35 b9 44  |w...l\....A8.5.D|
0001f8b0  da 0f f1 77 24 53 85 09  01 4c 87 0b 20 00 00 00  |...w$S...L.. ...|
0001f8c0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
```

```
$ dd if=tapesplice.tar bs=1 skip=129135 | bzip2 -d > flag.txt
3985+0 records in
3985+0 records out
3985 bytes (4.0 kB, 3.9 KiB) copied, 0.00351079 s, 1.1 MB/s

bzip2: (stdin): trailing garbage after EOF ignored
```

CTF{everything_has_space_for_hidden_data}
