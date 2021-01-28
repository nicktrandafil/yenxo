#/bin/bash

set -e

BUILD_TYPE=Release

docker build -t serialize --build-arg BUILD_TYPE=${BUILD_TYPE} -f Dockerfile ..
