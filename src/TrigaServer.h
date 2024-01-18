/*
TrigaServer is a software for GNU operating system to get the real-time
values of the Nuclear Reator Triga IPR-R1 and share in network.
Copyright (C) 2023-2024 Thalles Campagnani

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef TRIGA_SERVER
#define TRIGA_SERVER

#include <libModbusSystematomSPU.h>
#include <libModbusMaestecPLC.h>

#include <iostream>
#include <thread>
#include <memory>
#include <boost/asio.hpp>
#include <json/json.h>

#define TestMax

using boost::asio::ip::tcp;

struct ALL_DATA
{
    PLC_DATA PLC;
    SPU_DATA SPU_CHA;
    SPU_DATA SPU_CHB;
};

class TrigaServer
{
    public:
        TrigaServer(std::string spu_sp1, 
                    std::string spu_sp2, 
                    std::string clp_ip, 
                    u_int16_t clp_port, 
                    boost::asio::io_service& io_service, 
                    short port);
        
    private:
        //
        tcp::acceptor acceptor_;
        tcp::socket socket_;
        std::vector<std::thread> threads_;

        // Objetos para comunicação Modbus nas duas portas seriais
        libModbusSystematomSPU spuChA;
        libModbusSystematomSPU spuChB;
        libModbusMaestecPLC    plc;

        //Ponteiros inteligentes globais
        std::atomic<std::shared_ptr<SPU_DATA>> data_global_spuChA = std::make_shared<SPU_DATA>();
        std::atomic<std::shared_ptr<SPU_DATA>> data_global_spuChB = std::make_shared<SPU_DATA>();
        std::atomic<std::shared_ptr<PLC_DATA>> data_global_plc    = std::make_shared<PLC_DATA>();


        void startThreads();
        void StartAccept();
        void SendData(const ALL_DATA& data);
        void handleTCPClients();
        void readModbusRTU(libModbusSystematomSPU& spu);
        void readModbusTCP(libModbusMaestecPLC& plc);
};

#endif
