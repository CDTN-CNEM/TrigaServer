# TrigaServer

TrigaServer is a software for GNU operating system to get the real-time values of the Nuclear Reator Triga IPR-R1 and share in network.

## Compiling

``` Bash
git clone https://github.com/CDTNuclear/TrigaServer.git
cd vendor
chmod +x clone.sh
./clone.sh
cd ..
mkdir -p build
cd build
cmake ..
make
cp src/TrigaServer ./
cp ../resources/TrigaServer.conf ./
```

## License, Warranty and Copyright

This library comes with ABSOLUTELY NO WARRANTY; Is under GNU GENERAL PUBLIC LICENSE version 3 (GPL3), that means you ONLY can use in free software projects.
For more details read the file LICENSE.
Copyright (C) 2023-2024  Thalles Campagnani