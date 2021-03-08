The JPEG was saved with the highest quality quantization table (DQT)
and the coefficients have been overwritten with FF to fully saturate
all the DCT math that uses the coefficients from the table. This
causes the smallest artifacts to overwhelm and saturate everything
making the image unreadable.

Header before:
```
00000000  ff d8 ff e0 00 10 4a 46  49 46 00 01 01 01 00 48  |......JFIF.....H|
00000010  00 48 00 00 ff db 00 43  00 01 01 01 01 01 01 01  |.H.....C........|
00000020  01 01 01 01 01 01 01 01  01 01 01 01 01 01 01 01  |................|
00000030  01 01 01 01 01 01 01 01  01 01 01 01 01 01 01 01  |................|
00000040  01 01 01 01 01 01 01 01  01 01 01 01 01 01 01 01  |................|
00000050  01 01 01 01 01 01 01 01  01 ff db 00 43 01 01 01  |............C...|
00000060  01 01 01 01 01 01 01 01  01 01 01 01 01 01 01 01  |................|
00000070  01 01 01 01 01 01 01 01  01 01 01 01 01 01 01 01  |................|
00000080  01 01 01 01 01 01 01 01  01 01 01 01 01 01 01 01  |................|
00000090  01 01 01 01 01 01 01 01  01 01 01 01 01 01 ff c0  |................|
000000a0  00 11 08 01 e4 02 80 03  01 11 00 02 11 01 03 11  |................|
000000b0  01 ff c4 00 1f 00 00 01  05 01 01 01 01 01 01 00  |................|
000000c0  00 00 00 00 00 00 00 01  02 03 04 05 06 07 08 09  |................|
000000d0  0a 0b ff c4 00 b5 10 00  02 01 03 03 02 04 03 05  |................|
000000e0  05 04 04 00 00 01 7d 01  02 03 00 04 11 05 12 21  |......}........!|
000000f0  31 41 06 13 51 61 07 22  71 14 32 81 91 a1 08 23  |1A..Qa."q.2....#|
00000100  42 b1 c1 15 52 d1 f0 24  33 62 72 82 09 0a 16 17  |B...R..$3br.....|
00000110  18 19 1a 25 26 27 28 29  2a 34 35 36 37 38 39 3a  |...%&'()*456789:|
00000120  43 44 45 46 47 48 49 4a  53 54 55 56 57 58 59 5a  |CDEFGHIJSTUVWXYZ|
00000130  63 64 65 66 67 68 69 6a  73 74 75 76 77 78 79 7a  |cdefghijstuvwxyz|
```


Header after:
```
00000000  ff d8 ff e0 00 10 4a 46  49 46 00 01 01 01 00 48  |......JFIF.....H|
00000010  00 48 00 00 ff db 00 43  00 ff ff ff ff ff ff ff  |.H.....C........|
00000020  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000030  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000040  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000050  ff ff ff ff ff ff ff ff  ff ff db 00 43 01 ff ff  |............C...|
00000060  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000070  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000080  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000090  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff c0  |................|
000000a0  00 11 08 01 e4 02 80 03  01 11 00 02 11 01 03 11  |................|
000000b0  01 ff c4 00 1f 00 00 01  05 01 01 01 01 01 01 00  |................|
000000c0  00 00 00 00 00 00 00 01  02 03 04 05 06 07 08 09  |................|
000000d0  0a 0b ff c4 00 b5 10 00  02 01 03 03 02 04 03 05  |................|
000000e0  05 04 04 00 00 01 7d 01  02 03 00 04 11 05 12 21  |......}........!|
000000f0  31 41 06 13 51 61 07 22  71 14 32 81 91 a1 08 23  |1A..Qa."q.2....#|
00000100  42 b1 c1 15 52 d1 f0 24  33 62 72 82 09 0a 16 17  |B...R..$3br.....|
00000110  18 19 1a 25 26 27 28 29  2a 34 35 36 37 38 39 3a  |...%&'()*456789:|
00000120  43 44 45 46 47 48 49 4a  53 54 55 56 57 58 59 5a  |CDEFGHIJSTUVWXYZ|
00000130  63 64 65 66 67 68 69 6a  73 74 75 76 77 78 79 7a  |cdefghijstuvwxyz|
```


To make the image readable a more sensible table must be restored. The
DQT table from almost any JPEG will do.

The solution is not at all sensitive to the details. For example
setting the first 5 coefficients in each table to 05 makes the flag
pretty much readable:

```
00000000  ff d8 ff e0 00 10 4a 46  49 46 00 01 01 01 00 48  |......JFIF.....H|
00000010  00 48 00 00 ff db 00 43  00 05 05 05 ff ff ff ff  |.H.....C........|
00000020  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000030  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000040  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000050  ff ff ff ff ff ff ff ff  ff ff db 00 43 01 05 05  |............C...|
00000060  05 ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000070  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000080  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
00000090  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff c0  |................|
000000a0  00 11 08 01 e4 02 80 03  01 11 00 02 11 01 03 11  |................|
000000b0  01 ff c4 00 1f 00 00 01  05 01 01 01 01 01 01 00  |................|
000000c0  00 00 00 00 00 00 00 01  02 03 04 05 06 07 08 09  |................|
000000d0  0a 0b ff c4 00 b5 10 00  02 01 03 03 02 04 03 05  |................|
000000e0  05 04 04 00 00 01 7d 01  02 03 00 04 11 05 12 21  |......}........!|
000000f0  31 41 06 13 51 61 07 22  71 14 32 81 91 a1 08 23  |1A..Qa."q.2....#|
00000100  42 b1 c1 15 52 d1 f0 24  33 62 72 82 09 0a 16 17  |B...R..$3br.....|
00000110  18 19 1a 25 26 27 28 29  2a 34 35 36 37 38 39 3a  |...%&'()*456789:|
00000120  43 44 45 46 47 48 49 4a  53 54 55 56 57 58 59 5a  |CDEFGHIJSTUVWXYZ|
00000130  63 64 65 66 67 68 69 6a  73 74 75 76 77 78 79 7a  |cdefghijstuvwxyz|
```

A better restoration than this will make the flag even more readable.
