#!/bin/sh

set -e

CURRENT_DIR=$(pwd)

# Set up container environment
apt update
apt install sudo tzdata python3 -y
ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
dpkg-reconfigure --frontend noninteractive tzdata
apt install ca-certificates-java -y

# Install dependencies
sudo apt-get install default-jdk default-jdk-headless default-jre default-jre-headless \
    zlib1g zlib1g-dev libssl-dev libusb-1.0-0 \
    libusb-1.0-0-dev libdbus-1-3 libdbus-1-dev libbluetooth3 libbluetooth-dev zlib1g zlib1g-dev \
    git cmake g++ make libboost-all-dev autoconf \
    automake libtool bison libpcre3-dev python3-dev python3-numpy python3-setuptools python3-wheel \
    python3-pytest mono-devel curl libgtest-dev \
    python-dev python-numpy python-setuptools python-pytest -qq


# Install gtest
cd $CURRENT_DIR
git clone https://github.com/google/googletest
cd googletest
git checkout release-1.8.1
cmake -DCMAKE_CXX_FLAGS="-fPIC" .
cmake --build .
sudo cmake --build . --target install

# Install swig
cd $CURRENT_DIR
rm -rf swig_build_dir
mkdir -p swig_build_dir
cd swig_build_dir
curl -s -L https://github.com/swig/swig/archive/refs/tags/v4.0.2.tar.gz --output swig-src.tar.gz
tar xf swig-src.tar.gz
cd swig-4.0.2
./autogen.sh
./configure
make
sudo make install

# cmake configure
cd $CURRENT_DIR
rm -rf build2
mkdir -p build2
cd build2
cmake -G "Unix Makefiles" -DBUILD_GEN=ON  -DBUILD_TESTING=ON -DBUILD_PYTHON3=ON -DBUILD_PYTHON3_WHEEL=ON \
    -DBoost_USE_STATIC_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DPYTHON3_EXECUTABLE=/usr/bin/python3 \
    -DBUILD_JAVA=ON -DBUILD_NET=ON -DJAVA_HOME=/usr/lib/jvm/default-java \
    -DBUILD_PYTHON_WHEEL=OFF  -DBUILD_PYTHON=ON \
    ../robotraconteur

# build
cd $CURRENT_DIR/build2
cmake --build . --config Release -- -j 4

# test
ctest . -C Release -E "robotraconteur_test_discovery_loopback|RobotRaconteurService.DiscoveryLoopback" --output-on-failure
