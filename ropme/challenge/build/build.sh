#!/bin/bash

set -euo pipefail

pushd /src
make clean && make
popd
