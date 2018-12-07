#!/bin/bash
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-6 90
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 90
sudo apt-get install -y equivs openjdk-8-jdk
wget -O buck.deb https://github.com/facebook/buck/releases/download/v2018.08.27.01/buck.2018.08.27.01_all.deb
sudo dpkg -i buck.deb
buck --version
c++ --version
g++ --version
gcc --version

wget https://github.com/LoopPerfect/buckaroo/releases/download/buckaroo-redux-alpha-5/buckaroo-linux -O buckaroo-portable
chmod +x ./buckaroo-portable

./buckaroo-portable
