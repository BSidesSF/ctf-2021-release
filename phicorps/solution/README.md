Your private e * d mod phi(n) = 1

Therefore (e * d) - 1 === phi (is some multiple of phi)

Therefore phi = (e * d - 1) / ceil((e * d - 1) / n)

Agent 7's private decrypting exponent is

d7 = lift(Mod(1/e7, phi))

```
Agent 65537, we have your key:
ϕ-Corps public modulus: 22433508948585914858389533452954109906541104689276276196062849779615780198187923026517199919892010523407883925419683046496024331863249501188346302184452760755100190398550846397551900218691808954296189210602122886946851893136115739247947384522481979162222306553143282264743780419828348869497695174019065309377369475909236181294109888158796115569864341457906714398294058593167097315430671070961971149847349077480167051450803209574246461786635196748437706089824725899994854659781821758802304023560284833849265288722261262092992626779340181681459655577812047839361970491130915639075944366073364039717345278889859281938347
Your public encryption exponent: 11364977279923434110968170511580152595406792162567926105566857842407035337599202204160651281461809682944343773099185286855802624305113915194963870973285607
Your private decryption exponent: 4494697365107947160417908584695343823486326528798821528044210991316094354643601441580502365769344901647231163413298501301585656158135006121660146830872419496672846616157447764068831453374238117989446815906833475416418517262413482901914880598929964485655954207409840186761384021674621084414294279046166834560436953039913886968304161391032585448549550665541319542557977796809412943083904427503620838936824399834185137144976175857200007384783772781151434335858218812425615223037891668500655184566247465761514974031834769053687432596240324516011429331371403922520073763001510273080428478239904200027327347201918475792343

Agent 7's public encryption exponent is 10235912157710408827085616586885364350585957582884546294366939082004991569933345160806042592439799297445080455012527089355984033107421475343743604274899137
The message we intercepted to Agent 7 was 12978126741384971333211523530657437644661548090508682342510500119244847297340334351399565032588496221575744738318981886104414449539352830188526483838103836869721926068433628973714092578888140424240282725951300514861604430187618339413312218792683761318837111844281860921816229721921451821940819869172798953184735244582833190334901068084642043856390152024773141697469148229450350557878350991758898815409789925951454933702666268294646348347692590859476814679243143292159338514805685100332691668637406178737235819911244931805477699909804105291135007311299130830655476086511432860380515134737611072146157531884388673095031
```


```
GP/PARI> n = 22433508948585914858389533452954109906541104689276276196062849779615780198187923026517199919892010523407883925419683046496024331863249501188346302184452760755100190398550846397551900218691808954296189210602122886946851893136115739247947384522481979162222306553143282264743780419828348869497695174019065309377369475909236181294109888158796115569864341457906714398294058593167097315430671070961971149847349077480167051450803209574246461786635196748437706089824725899994854659781821758802304023560284833849265288722261262092992626779340181681459655577812047839361970491130915639075944366073364039717345278889859281938347;

GP/PARI> e = 11364977279923434110968170511580152595406792162567926105566857842407035337599202204160651281461809682944343773099185286855802624305113915194963870973285607;

GP/PARI> d = 4494697365107947160417908584695343823486326528798821528044210991316094354643601441580502365769344901647231163413298501301585656158135006121660146830872419496672846616157447764068831453374238117989446815906833475416418517262413482901914880598929964485655954207409840186761384021674621084414294279046166834560436953039913886968304161391032585448549550665541319542557977796809412943083904427503620838936824399834185137144976175857200007384783772781151434335858218812425615223037891668500655184566247465761514974031834769053687432596240324516011429331371403922520073763001510273080428478239904200027327347201918475792343;

GP/PARI> phi = (e * d - 1) / ceil((e * d - 1) / n)
%4 = 22433508948585914858389533452954109906541104689276276196062849779615780198187923026517199919892010523407883925419683046496024331863249501188346302184452760755100190398550846397551900218691808954296189210602122886946851893136115739247947384522481979162222306553143282264743780419828348869497695174019065309377069560074803059750507184682837967583077675426258854165159818018365858649464110576682083466963059503779704309315145142506771448861484431125643998114392284996040987989851386679460069430367655446124286342931629185192030018528301495819744446771131451435445129598103801252568137984295928600851977895489205257398400

GP/PARI> e7 = 10235912157710408827085616586885364350585957582884546294366939082004991569933345160806042592439799297445080455012527089355984033107421475343743604274899137;

GP/PARI> d7 = lift(Mod(1/e7, phi))
%6 = 21635956382077818649627281555279665890501553040278215560684547910682264906298433722647935302318398749438343834308968894203172826388668616930733021642988235591774692391148890824763170957675401980546239137851462418610717267467168178165854422170444984391378297425827284137121208043569492017595633236204352593824706581428654379529601103267392994327225280838542791656572247695119028166061835713370927850266601289710369777941608654316483067696065451549111062769429917582811492385514136591325816395985600338138486377546909259496812441010782151901844632899687668627607878181203030835676325865313781121955292556405370918017473

GP/PARI> c7 = 12978126741384971333211523530657437644661548090508682342510500119244847297340334351399565032588496221575744738318981886104414449539352830188526483838103836869721926068433628973714092578888140424240282725951300514861604430187618339413312218792683761318837111844281860921816229721921451821940819869172798953184735244582833190334901068084642043856390152024773141697469148229450350557878350991758898815409789925951454933702666268294646348347692590859476814679243143292159338514805685100332691668637406178737235819911244931805477699909804105291135007311299130830655476086511432860380515134737611072146157531884388673095031;

GP/PARI> lift(modexp(c7, d7, n))
%8 = 11171401588375924988400608175479805239853986357828714114692361063010253115748423623976251395921511327342705334513208740854304179903744643779958983726435231
```

```
What was the message sent to Agent 7? 11171401588375924988400608175479805239853986357828714114692361063010253115748423623976251395921511327342705334513208740854304179903744643779958983726435231
CTF{omg_rsa_destroyyyyyeeeeddddd}
```