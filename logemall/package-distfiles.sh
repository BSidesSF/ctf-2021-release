#!/bin/bash

set -euo pipefail

# Make temp folders
mkdir -p distfiles/sample/maps distfiles/sample/encounters distfiles/sample/data

# Strip down the CSV file
cat challenge/data/asciimon.csv | egrep -i '(^;|,caterpillar,|,pigeon,|,bee,|,plant frog,|,fire lizard,|,water turtle,)' > distfiles/sample/data/asciimon.csv

# Grab the portraits
cp challenge/data/1.txt  distfiles/sample/data/
cp challenge/data/4.txt  distfiles/sample/data/
cp challenge/data/7.txt  distfiles/sample/data/
cp challenge/data/10.txt distfiles/sample/data/
cp challenge/data/15.txt distfiles/sample/data/
cp challenge/data/16.txt distfiles/sample/data/

# Grab the other datafiles
cp challenge/maps/lab.map challenge/maps/colour.map challenge/maps/chartreuse.map distfiles/sample/maps/
cp challenge/encounters/chartreuse.enc distfiles/sample/encounters/

# Put in some instructions
echo -e "These files need to go in /home/ctf/ - if you just go ahead and\nstart the binary, it'll let you know if they're in the\nwrong place.\n\nKeep in mind this is very limited - if you try to leave the first three maps, you'll\nget an error (unless you create your own maps!). Good luck!" > distfiles/sample/README.txt

# Tar them up
pushd distfiles
tar -cvvjf sample.tar.bz2 sample
popd

# Delete the temp files
rm -rf distfiles/sample

# Copy the main binary
cp challenge/src/logemall distfiles/
