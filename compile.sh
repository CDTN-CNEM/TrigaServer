#!/bin/bash

reset() {
    rm -rf build lib*
    
    git clone https://github.com/CDTNuclear/libModbusSystematomSPU.git
    git clone https://github.com/CDTNuclear/libModbusMaestecPLC.git

    ln -s libModbusMaestecPLC/libModbusMaestecPLC.cpp       libModbusMaestecPLC.cpp
    ln -s libModbusMaestecPLC/libModbusMaestecPLC.hpp       libModbusMaestecPLC.hpp
    ln -s libModbusSystematomSPU/libModbusSystematomSPU.hpp libModbusSystematomSPU.hpp
    ln -s libModbusSystematomSPU/libModbusSystematomSPU.cpp libModbusSystematomSPU.cpp
}

run() {
    cd ../../
    ./TrigaServer
}

installbin(){
    sudo cp -f TrigaServer /usr/bin/
}

installservice(){
    sudo su
    echo "[Unit]"                                   >  /etc/systemd/system/TrigaServer.service
    echo "Description=TrigaServer"                  >> /etc/systemd/system/TrigaServer.service
    echo "After=network.target"                     >> /etc/systemd/system/TrigaServer.service
    echo "[Service]"                                >> /etc/systemd/system/TrigaServer.service
    echo "ExecStart=/bin/TrigaServer"               >> /etc/systemd/system/TrigaServer.service
    echo "Restart=always"                           >> /etc/systemd/system/TrigaServer.service
    echo "User=TrigaServer"                         >> /etc/systemd/system/TrigaServer.service
    echo "[Install]"                                >> /etc/systemd/system/TrigaServer.service
    echo "WantedBy=multi-user.target"               >> /etc/systemd/system/TrigaServer.service
    sudo systemctl daemon-reload
    sudo systemctl enable TrigaServer --now
}

set -x
set -e

cd src

if [ "$1" == "reset" ]; then
    reset
fi

mkdir -p build
cd build
cmake ..
make

cp -f TrigaServer ../../

case "$1" in
    "run")
        run
        ;;
    "install")
        installbin
        ;;
    "installservice")
        installbin
        installservice
        ;;
    *)
esac

case "$2" in
    "run")
        run
        ;;
    "install")
        installbin
        ;;
    "installservice")
        installbin
        installservice
        ;;
    *)
esac

exit 0

