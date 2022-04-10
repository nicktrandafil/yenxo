#/bin/bash

set -e

tag=$1
docker_file=$tag

case $tag in
    main | dev) ;;
    *)
        echo "Expected either 'main' or 'dev'"
        exit 1;;
esac

BUILD_TYPE=Release
BOOST_VERSION=1.79.0
BOOST_URL=https://boostorg.jfrog.io/artifactory/main/release/${BOOST_VERSION}/source/boost_${BOOST_VERSION//\./_}.tar.gz

docker build -t ntrandafil/yenxo:${tag} \
  --build-arg BUILD_TYPE=${BUILD_TYPE} \
  --build-arg BOOST_URL=${BOOST_URL} \
  -f $docker_file ..

