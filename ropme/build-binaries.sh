#!/bin/bash

set -euo pipefail

docker build challenge/build/ -t build
docker run --rm  -v $PWD/challenge:/src -ti build
./package-distfiles.sh
