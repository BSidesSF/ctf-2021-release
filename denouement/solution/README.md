The flag has been inserted at the end of the zlib deflate stream after
the PNG row / pixel data.


```
$ dd if=denouement.png bs=1 skip=41 |  ~/projects/misc_perl/zlibinflate.pl | strings -a | tail -n 1
126867+0 records in
126867+0 records out
126867 bytes (127 kB, 124 KiB) copied, 0.0976865 s, 1.3 MB/s
CTF{then_we_just_float_away_with_the_rest_of_the_pixels}
```

libpng provides a big hint:
`libpng warning: IDAT: Too much image data`

Also the image is a screenshot of a stackoverflow question about
what's in a png zlib stream.

