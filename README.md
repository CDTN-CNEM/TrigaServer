# TrigaServer

TrigaServer is a software for GNU operating system to get the real-time values of the Nuclear Reator Triga IPR-R1 and share in network.

## Clone and compiling

First you need install the requeriments. If you are using Debian distribution:

``` Bash
sudo apt install git build-essential cmake libboost-all-dev libcxxopts-dev libmodbus-dev libserialport-dev libjsoncpp-dev
```

After you can clone, build and compiling with the following commands:

``` Bash
git clone https://github.com/SEURT-CDTN/TrigaServer.git
cd TrigaServer
mkdir -p build
cd build
cmake ..
make
```

## License, Warranty and Copyright

This library comes with ABSOLUTELY NO WARRANTY; Is under GNU GENERAL PUBLIC LICENSE version 3 (GPL3), that means you ONLY can use in free software projects.
For more details read the file LICENSE.
Copyright (C) 2023-2024  Thalles Campagnani
