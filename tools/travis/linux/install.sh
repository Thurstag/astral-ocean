#!/bin/bash

# C++17
sudo apt-get install -y g++-7
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 90
sudo ln -s /usr/bin/g++-7 /usr/local/bin/g++
