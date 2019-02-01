#!/bin/bash

# C++17
sudo apt-get install -y g++-7
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 90
sudo ln -s /usr/bin/g++-7 /usr/local/bin/g++

# Boost 1.68
wget -q -O boost_1_68_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.68.0/boost_1_68_0.tar.gz/download
tar xzf boost_1_68_0.tar.gz
cd boost_1_68_0
sudo ./bootstrap.sh --with-libraries=all --prefix=/usr/local/
sudo ./b2 -d0 -j4
sudo ./b2 -d0 -j4 install
