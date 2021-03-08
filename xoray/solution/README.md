```
brenrigh@lambda ~/projects/github/ctf-2021/challenges/xoray/solution $ ./poly_byte_break.pl ~/download/english.txt ../distfiles/xoray.bin 16
counting n-graph stats for n= 2
counting n-graph stats for n= 3
counting n-graph stats for n= 4
counting n-graph stats for n= 5
counting n-graph stats for n= 6
counting n-graph stats for n= 7
counting n-graph stats for n= 8
Diffing n-graph indexes for n= 2
Diffing n-graph indexes for n= 3
Diffing n-graph indexes for n= 4
Diffing n-graph indexes for n= 5
Diffing n-graph indexes for n= 6
Diffing n-graph indexes for n= 7
Diffing n-graph indexes for n= 8
-------------------------------------------------------------------------------
8-graph key length votes: 16:84, 8:84, 4:84, 2:84, 1:84, 20:10, 14:10, 10:10, 7:10, 5:10, 18:5, 12:5, 9:5, 6:5, 3:5, 11:3
7-graph key length votes: 16:102, 8:102, 4:102, 2:102, 1:102, 20:15, 10:15, 5:15, 14:9, 7:9, 12:6, 6:6, 3:6, 18:5, 9:5, 11:4, 17:1, 13:1
6-graph key length votes: 16:127, 8:127, 4:127, 2:127, 1:127, 20:21, 10:21, 5:21, 14:9, 12:9, 7:9, 6:9, 3:9, 18:6, 9:6, 11:5, 17:3, 13:2
5-graph key length votes: 16:184, 8:184, 4:184, 2:184, 1:184, 20:30, 10:30, 5:30, 12:22, 6:22, 3:22, 14:11, 7:11, 18:10, 9:10, 11:8, 17:5, 19:2, 13:2, 15:1
4-graph key length votes: 16:283, 8:283, 4:283, 2:283, 1:283, 20:43, 10:43, 5:43, 12:42, 6:42, 3:42, 11:21, 18:16, 9:16, 14:14, 7:14, 17:9, 13:7, 19:3, 15:2
3-graph key length votes: 16:484, 8:484, 4:484, 2:484, 1:484, 12:94, 6:94, 3:94, 20:74, 10:74, 5:74, 11:47, 14:36, 7:36, 18:26, 9:26, 13:20, 17:15, 15:8, 19:5
2-graph key length votes: 1:1357, 2:1349, 4:1328, 8:1311, 16:1309, 3:405, 6:401, 12:394, 10:246, 5:246, 20:244, 7:144, 14:143, 9:117, 18:115, 11:110, 15:73, 13:67, 17:63, 19:44
-------------------------------------------------------------------------------
Score for ctext: 0.061641312962838
Top 4 keys for slot 0: 193, 199, 192, 197
Top 4 keys for slot 1: 99, 101, 98, 100
Top 4 keys for slot 2: 178, 180, 181, 179
Top 4 keys for slot 3: 148, 146, 147, 149
Top 4 keys for slot 4: 246, 240, 242, 247
Top 4 keys for slot 5: 116, 114, 117, 115
Top 4 keys for slot 6: 249, 255, 248, 253
Top 4 keys for slot 7: 26, 28, 27, 29
Top 4 keys for slot 8: 158, 180, 152, 219
Top 4 keys for slot 9: 70, 64, 65, 108
Top 4 keys for slot 10: 190, 148, 184, 251
Top 4 keys for slot 11: 97, 103, 96, 125
Top 4 keys for slot 12: 16, 22, 23, 58
Top 4 keys for slot 13: 48, 54, 49, 55
Top 4 keys for slot 14: 133, 131, 132, 130
Top 4 keys for slot 15: 166, 160, 167, 161

===============================================================================
FIRST 5 GUESSES IN SEMI-OPTIMAL ORDER
===============================================================================
guess key # 0 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 48, 133, 166] (.c...t...F.a.0..) score: 0.0448771408266584 word score: 21416 :
<!DOCTYPE.HTML.PUBLIC."-//W3C//DTD.HTML.4.0.Transitional//EN">.<html>.<head>..<meta.http-equiv="content-type".content="text/html;.charset=utf-8"/>..<title></title>..<meta.name="generator".content="LibreOffice.6.4.7.2.(Linux)"/>..<meta.name="created".conten

guess key # 1 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 48, 133, 160] (.c...t...F.a.0..) score: 0.0418245175781879 word score: 13760 :
<!DOCTYPE.HTML.VUBLIC."-//W3C//BTD.HTML.4.0.Trahsitional//EN">.:html>.<head>..<keta.http-equiv=$content-type".cintent="text/htmj;.charset=utf-8$/>..<title></tirle>..<meta.name;"generator".conrent="LibreOfficc.6.4.7.2.(Linux/"/>..<meta.name;"created".conteh

guess key # 2 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 48, 133, 167] (.c...t...F.a.0..) score: 0.0418665659998517 word score: 14064 :
<!DOCTYPE.HTML.QUBLIC."-//W3C//ETD.HTML.4.0.Traositional//EN">.=html>.<head>..<leta.http-equiv=#content-type".cnntent="text/htmm;.charset=utf-8#/>..<title></tiule>..<meta.name<"generator".conuent="LibreOfficd.6.4.7.2.(Linux("/>..<meta.name<"created".conteo

guess key # 3 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 48, 133, 161] (.c...t...F.a.0..) score: 0.0418017810496655 word score: 13808 :
<!DOCTYPE.HTML.WUBLIC."-//W3C//CTD.HTML.4.0.Traisitional//EN">.;html>.<head>..<jeta.http-equiv=%content-type".chntent="text/htmk;.charset=utf-8%/>..<title></tisle>..<meta.name:"generator".consent="LibreOfficb.6.4.7.2.(Linux."/>..<meta.name:"created".contei

guess key # 4 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 48, 133, 186] (.c...t...F.a.0..) score: 0.0422089667866101 word score: 13768 :
<!DOCTYPE.HTML.LUBLIC."-//W3C//XTD.HTML.4.0.Trarsitional//EN">..html>.<head>..<qeta.http-equiv=>content-type".csntent="text/htmp;.charset=utf-8>/>..<title></tihle>..<meta.name!"generator".conhent="LibreOfficy.6.4.7.2.(Linux5"/>..<meta.name!"created".conter


===============================================================================
FIRST 5 GUESSES IN STATISTICAL SCORE ORDER
===============================================================================
guess key # 997 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 103, 23, 52, 175, 167] (.c...t...F.g.4..) score: 0.03279250368006 word score: 5072 :
<!DOCTYPE.HRJH.QUBLIC."-//W5D+.ETD.HTML.4.0&SvKositional//EH%:.=html>.<head8...leta.http-eqsnr.#content-typc%$Inntent="text)opGm;.charset=ura).#/>..<title>:(pCule>..<meta.hfiO<"generator"&dkDuent="LibreO`amId.6.4.7.2.(LoiqR("/>..<meta.hfiO<"created".ciipOo

guess key # 922 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 103, 23, 54, 175, 167] (.c...t...F.g.6..) score: 0.0329237964614102 word score: 5056 :
<!DOCTYPE.HRJJ.QUBLIC."-//W5D).ETD.HTML.4.0&StKositional//EH%8.=html>.<head8...leta.http-eqsnp.#content-typc%&Inntent="text)orGm;.charset=ura+.#/>..<title>:(rCule>..<meta.hfkO<"generator"&diDuent="LibreO`aoId.6.4.7.2.(LoisR("/>..<meta.hfkO<"created".ciirOo

guess key # 946 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 103, 23, 49, 175, 160] (.c...t...F.g.1..) score: 0.0330749404287513 word score: 4680 :
<!DOCTYPE.HRJM.VUBLIC."-//W5D..BTD.HTML.4.0&SsKhsitional//EH%?.:html>.<head8...keta.http-eqsnw.$content-typc%!Iintent="text)ouGj;.charset=ura,.$/>..<title>:(uCrle>..<meta.hflO;"generator"&dnDrent="LibreO`ahIc.6.4.7.2.(LoitR/"/>..<meta.hflO;"created".ciiuOh

guess key # 947 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 103, 23, 49, 175, 167] (.c...t...F.g.1..) score: 0.0330831334121827 word score: 5056 :
<!DOCTYPE.HRJM.QUBLIC."-//W5D..ETD.HTML.4.0&SsKositional//EH%?.=html>.<head8...leta.http-eqsnw.#content-typc%!Inntent="text)ouGm;.charset=ura,.#/>..<title>:(uCule>..<meta.hflO<"generator"&dnDuent="LibreO`ahId.6.4.7.2.(LoitR("/>..<meta.hflO<"created".ciiuOo

guess key # 996 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 103, 23, 52, 175, 160] (.c...t...F.g.4..) score: 0.0330944932680397 word score: 4680 :
<!DOCTYPE.HRJH.VUBLIC."-//W5D+.BTD.HTML.4.0&SvKhsitional//EH%:.:html>.<head8...keta.http-eqsnr.$content-typc%$Iintent="text)opGj;.charset=ura).$/>..<title>:(pCrle>..<meta.hfiO;"generator"&dkDrent="LibreO`amIc.6.4.7.2.(LoiqR/"/>..<meta.hfiO;"created".ciipOh


===============================================================================
FIRST 5 GUESSES IN WORD SCORE ORDER
===============================================================================
guess key # 0 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 48, 133, 166] (.c...t...F.a.0..) score: 0.0448771408266584 word score: 21416 :
<!DOCTYPE.HTML.PUBLIC."-//W3C//DTD.HTML.4.0.Transitional//EN">.<html>.<head>..<meta.http-equiv="content-type".content="text/html;.charset=utf-8"/>..<title></title>..<meta.name="generator".content="LibreOffice.6.4.7.2.(Linux)"/>..<meta.name="created".conten

guess key # 75 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 55, 133, 166] (.c...t...F.a.7..) score: 0.0422285417072545 word score: 16152 :
<!DOCTYPE.HTMK.PUBLIC."-//W3C(/DTD.HTML.4.0.Tuansitional//EN"9.<html>.<head>..<meta.http-equiq="content-type"'content="text/hsml;.charset=utf*8"/>..<title></sitle>..<meta.naje="generator".chntent="LibreOffnce.6.4.7.2.(Linrx)"/>..<meta.naje="created".consen

guess key # 20 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 48, 175, 166] (.c...t...F.a.0..) score: 0.0421751555882655 word score: 15984 :
<!DOCTYPE.HTML.PUBLIC."-//W3C/.DTD.HTML.4.0.TrKnsitional//EN">.<html>.<head>...meta.http-equiv."content-type".Iontent="text/htGl;.charset=utf-."/>..<title></tCtle>..<meta.namO="generator".coDtent="LibreOffiIe.6.4.7.2.(LinuR)"/>..<meta.namO="created".contOn

guess key # 100 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 52, 133, 166] (.c...t...F.a.4..) score: 0.0422545741149883 word score: 15880 :
<!DOCTYPE.HTMH.PUBLIC."-//W3C+/DTD.HTML.4.0.Tvansitional//EN":.<html>.<head>..<meta.http-equir="content-type"$content="text/hpml;.charset=utf)8"/>..<title></pitle>..<meta.naie="generator".ckntent="LibreOffmce.6.4.7.2.(Linqx)"/>..<meta.naie="created".conpen

guess key # 95 : [193, 99, 178, 148, 246, 116, 249, 26, 158, 70, 190, 97, 16, 55, 175, 166] (.c...t...F.a.7..) score: 0.0397380337485248 word score: 15808 :
<!DOCTYPE.HTMK.PUBLIC."-//W3C(.DTD.HTML.4.0.TuKnsitional//EN"9.<html>.<head>...meta.http-equiq."content-type"'Iontent="text/hsGl;.charset=utf*."/>..<title></sCtle>..<meta.najO="generator".chDtent="LibreOffnIe.6.4.7.2.(LinrR)"/>..<meta.najO="created".consOn
```
