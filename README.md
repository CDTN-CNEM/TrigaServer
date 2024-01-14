# TrigaServer

TrigaServer is a software for GNU operating system to get the real-time values of the Nuclear Reator Triga IPR-R1 and share in network.

## License, Warranty and Copyright

This library comes with ABSOLUTELY NO WARRANTY; Is under GNU GENERAL PUBLIC LICENSE version 3 (GPL3), that means you ONLY can use in free software projects.
For more details read the file LICENSE.
Copyright (C) 2023-2024  Thalles Campagnani

## Compiling the first time

First clone the repository:

``` Bash
git clone https://github.com/CDTNuclear/TrigaServer.git
```

Give permission to execute the script ```compile.sh```.

``` Bash
chmod +x compile.sh
```

Execute the script ```compile.sh``` using the parameter clean:

``` Bash
./compile.sh clean
```

## Compiling modifications

After modify the code you can just run the script ```compile.sh``` doing:

``` Bash
./compile.sh
```

If you use the parameter clean, will clean the modification of the librarys.