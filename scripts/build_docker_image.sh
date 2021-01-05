#/bin/bash

set -e

BOOST_VERSION=1.65.1
BOOST_URL="https://dl.bintray.com/boostorg/release/${BOOST_VERSION}/source/boost_${BOOST_VERSION//\./_}.tar.gz"
BUILD_TYPE=Release

docker build -t serialize --build-arg BOOST_VERSION=${BOOST_VERSION} --build-arg BUILD_TYPE=${BUILD_TYPE} --build-arg BOOST_URL=${BOOST_URL} -f Dockerfile ..
