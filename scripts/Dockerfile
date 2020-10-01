FROM ubuntu:18.04

RUN apt-get -y update && apt-get -y install \
    libgl1-mesa-glx libglib2.0-0 libfontconfig1 libdbus-1-3 libxrender1 \
    bash-completion \
    build-essential git cmake wget tar sudo gdb \
    doxygen \
    lcov

ARG BOOST_VERSION
ARG BOOST_URL

ENV BOOST_ROOT=/usr/local/include/boost-${BOOST_VERSION}

# boost
WORKDIR /tmp
RUN mkdir -p ${BOOST_ROOT} \
 && wget -O - ${BOOST_URL} | tar --strip-components=1 -xz -C ${BOOST_ROOT}

# RapidJSON
WORKDIR /tmp
RUN git clone --recurse-submodules https://github.com/Tencent/rapidjson.git \
 && cd rapidjson \
 && mkdir build/ && cd build/ \
 && cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DRAPIDJSON_BUILD_TESTS=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_DOC=OFF .. \
 && cmake --build . \
 && cmake --build . --target install

# Catch2
WORKDIR /tmp
RUN git clone --recurse-submodules https://github.com/catchorg/Catch2.git \
 && cd Catch2 \
 && mkdir build/ && cd build/ \
 && cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCATCH_BUILD_TESTING=OFF -DCATCH_INSTALL_DOCS=OFF .. \
 && cmake --build . \
 && cmake --build . --target install

# debug_assert
WORKDIR /tmp
RUN git clone --recurse-submodules https://github.com/foonathan/debug_assert.git \
 && cd debug_assert \
 && mkdir build/ && cd build/ \
 && cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE .. \
 && cmake --build . \
 && cmake --build . --target install

# type_safe
WORKDIR /tmp
RUN git clone --recurse-submodules https://github.com/foonathan/type_safe.git \
 && cd type_safe \
 && mkdir build/ && cd build/ \
 && cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DTYPE_SAFE_BUILD_TEST_EXAMPLE=OFF .. \
 && cmake --build . \
 && cmake --build . --target install

RUN echo "if [ -f /home/$USER/.bashrc ]; then . /home/$USER/.bashrc ; fi" >> /root/.bashrc

CMD /bin/bash -l
