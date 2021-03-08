#!/bin/bash

set -euo pipefail

pushd /src/client
make clean && make
popd

pushd /src/server
make clean && make
popd
