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
//TrigaServer.cpp
#include "TrigaServer.h"

const char * JSON_TEMPLATE = 
"{\n\
    SPU_CHA: {\n\
        READ: %d,\n\
        N_DATA_FP: %f,\n\
        T_DATA_FP: %f,\n\
        F1_DATA_FP: %f,\n\
        F2_DATA_FP: %f,\n\
        F3_DATA_FP: %f,\n\
        EMR_N_THRESHOLD: %f,\n\
        WRN_N_THRESHOLD: %f,\n\
        EMR_T_THRESHOLD: %f,\n\
        WRN_T_THRESHOLD: %f,\n\
        EMR_N: %d,\n\
        WRN_N: %d,\n\
        EMR_T: %d,\n\
        WRN_T: %d,\n\
        R1: %d,\n\
        R2: %d,\n\
        R3: %d,\n\
        RDY: %d,\n\
        TEST: %d,\n\
        XXXX: %d\n\
    },\n\
    SPU_CHB: {\n\
        READ: %d,\n\
        N_DATA_FP: %f,\n\
        T_DATA_FP: %f,\n\
        F1_DATA_FP: %f,\n\
        F2_DATA_FP: %f,\n\
        F3_DATA_FP: %f,\n\
        EMR_N_THRESHOLD: %f,\n\
        WRN_N_THRESHOLD: %f,\n\
        EMR_T_THRESHOLD: %f,\n\
        WRN_T_THRESHOLD: %f,\n\
        EMR_N: %d,\n\
        WRN_N: %d,\n\
        EMR_T: %d,\n\
        WRN_T: %d,\n\
        R1: %d,\n\
        R2: %d,\n\
        R3: %d,\n\
        RDY: %d,\n\
        TEST: %d,\n\
        XXXX: %d\n\
    },\n\
    PLC: {\n\
        READ: %d,\n\
        IG01: %f,\n\
        IG02: %f,\n\
        IG03: %f,\n\
        IG04: %f,\n\
        JT01: %f,\n\
        JT02: %f,\n\
        JT03: %f,\n\
        KT01: %f,\n\
        TT01: %f,\n\
        TT02: %f,\n\
        TT03: %f,\n\
        TT04: %f,\n\
        TT05: %f,\n\
        CT01: %f,\n\
        CT02: %f,\n\
        FI01: %f,\n\
        FI02: %f,\n\
        LT01: %f,\n\
        PHT01: %f,\n\
        TTXX: %f,\n\
        TT06: %f,\n\
        TT07: %f,\n\
        TT08: %f,\n\
        RT01: %f,\n\
        RT02: %f,\n\
        RT03: %f,\n\
        RT04: %f,\n\
        RT05: %f,\n\
        RT06: %f,\n\
        Reserva_AI11: %f,\n\
        Reserva_AI12: %f,\n\
        Reserva_AI13: %f,\n\
        Reserva_AI14: %f,\n\
        Reserva_AI15: %f,\n\
        Reserva_AI16: %f,\n\
        Reserva_AI17: %f,\n\
        Reserva_AI18: %f,\n\
        Reserva_AI19: %f,\n\
        Reserva_AI20: %f,\n\
        Reserva_AI21: %f\n\
    },\n\
}";

TrigaServer::TrigaServer(std::string spu_sp1,//SPU_CH_A serial port
                         std::string spu_sp2,//SPU_CH_B serial port
                         std::string clp_ip, //CLP IP
                         u_int16_t clp_port) //CPL Port
                        :spuChA(spu_sp1),
                         spuChB(spu_sp2),
                         plc(clp_ip,clp_port)
{
}


TrigaServer::~TrigaServer() {
    for(auto& thread : clientThreads) {
        if(thread.joinable())
            thread.join();
    }
}

void TrigaServer::startReadThreads() // Método para Threads
{
    //Iniciar todas as treads necessárias
    std::thread spuChAThread(&TrigaServer::readModbusRTU, this, std::ref(spuChA));
    std::thread spuChBThread(&TrigaServer::readModbusRTU, this, std::ref(spuChB));
    std::thread plcThread   (&TrigaServer::readOpcTCP,    this, std::ref(plc));
    #ifdef TestMax
        for (int i = 0; i < 10; i++) {
            std::thread serverThread(&TrigaServer::handleTCPClients, this);
            serverThread.detach();
        }
    #endif

    //Esperar todas as threads terminarem (idelmente não deveria terminar)
    spuChAThread.join();
    spuChBThread.join();
    plcThread.join();
}

void TrigaServer::startServer(int port, bool sendJson) {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "[startServer] Error opening socket" << std::endl;
        return;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "[startServer] Error on binding" << std::endl;
        return;
    }

    listen(serverSocket, 5);
    std::cout << "[startServer] Server started on port " << port << std::endl;

    while(true) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            std::cerr << "[startServer] Error on accept" << std::endl;
            continue;
        }

        std::cout << "[startServer] Client connected" << std::endl;
        
        std::thread clientThread(&TrigaServer::handleTCPClients, this, clientSocket, sendJson);
        clientThread.detach();
    }
}

void TrigaServer::handleTCPClients(int clientSocket, bool sendJson)
{
    int timeout = 2; // 2 seconds timeout
    char buffer[1024];
    int n = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (n <= 0)
    {
        std::cerr << "[handleTCPClients] Error receiving data" << std::endl;
        close(clientSocket);
        return;
    }

    // Parse received data (assuming it's a number)
    int interval = std::stoi(std::string(buffer, n));

    std::cout << "[handleTCPClients] Received interval: " << interval << "ms" << std::endl;

    if(sendJson)
    {
        // Create new thread
        std::thread([this, interval, clientSocket]()
        {
            while(true)
            {
                auto data_local_spuChA = std::shared_ptr <SPU_DATA> (new SPU_DATA);
                auto data_local_spuChB = std::shared_ptr <SPU_DATA> (new SPU_DATA);
                auto data_local_plc    = std::shared_ptr <PLC_DATA> (new PLC_DATA);

                data_local_spuChA = data_global_spuChA.load();
                data_local_spuChB = data_global_spuChB.load();
                data_local_plc = data_global_plc.load();

                ALL_DATA data;
                data.SPU_CHA = *data_local_spuChA;
                data.SPU_CHB = *data_local_spuChB;
                data.PLC    = *data_local_plc;
                
                // Send JSON data
                std::string json = genJson(data); // Assuming all_data is accessible here

                send(clientSocket, json.c_str(), json.size(), 0);

                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            }
        }).detach();
    }

    // Create new thread
    std::thread([this, interval, clientSocket]()
    {
        while(true)
        {
            auto data_local_spuChA = std::shared_ptr <SPU_DATA> (new SPU_DATA);
            auto data_local_spuChB = std::shared_ptr <SPU_DATA> (new SPU_DATA);
            auto data_local_plc    = std::shared_ptr <PLC_DATA> (new PLC_DATA);

            data_local_spuChA = data_global_spuChA.load();
            data_local_spuChB = data_global_spuChB.load();
            data_local_plc = data_global_plc.load();

            #ifndef TestMax
                ALL_DATA data;
                data.SPU_CHA = *data_local_spuChA;
                data.SPU_CHB = *data_local_spuChB;
                data.PLC     = *data_local_plc;
                
                // Convert ALL_DATA to a char pointer
                char* buffer = reinterpret_cast<char*>(&data);
                size_t size = sizeof(data);

                // Send data
                send(clientSocket, buffer, size, 0);

                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            #endif
        }
    }).detach();
    
    
}

// Ler dados da SPU usando libModbusSystematomSPU
void TrigaServer::readModbusRTU(libModbusSystematomSPU& spu)
{
    while (true)
    {
        //Criação do ponteiro inteligente local
        auto data_local = std::shared_ptr <SPU_DATA> (new SPU_DATA);
        //Realizando leitura e salvando informação se foi lido
        *data_local  = spu.get_all();
        //Passar para o ponteiro inteligente global
        if (spu.get_portname() == "/dev/ttyF0") data_global_spuChA.store(data_local);
        else                                    data_global_spuChB.store(data_local);
    }
}

// Ler dados Modbus do PLC usando libModbusSiemensPLC
void TrigaServer::readOpcTCP(libOpcMaestecPLC& plc) 
{
    while (true)
    {
        auto data_local = std::shared_ptr <PLC_DATA> (new PLC_DATA);
        *data_local = plc.get_all();
        data_global_plc.store(data_local);
    }
}

std::string TrigaServer::genJson(ALL_DATA all_data) 
{
    char buffer[4096];
    sprintf(buffer, JSON_TEMPLATE, all_data.SPU_CHA.READ,
            all_data.SPU_CHA.N_DATA_FP,
            all_data.SPU_CHA.T_DATA_FP,
            all_data.SPU_CHA.F1_DATA_FP,
            all_data.SPU_CHA.F2_DATA_FP,
            all_data.SPU_CHA.F3_DATA_FP,
            all_data.SPU_CHA.EMR_N_THRESHOLD,
            all_data.SPU_CHA.WRN_N_THRESHOLD,
            all_data.SPU_CHA.EMR_T_THRESHOLD,
            all_data.SPU_CHA.WRN_T_THRESHOLD,
            all_data.SPU_CHA.EMR_N,
            all_data.SPU_CHA.WRN_N,
            all_data.SPU_CHA.EMR_T,
            all_data.SPU_CHA.WRN_T,
            all_data.SPU_CHA.R1,
            all_data.SPU_CHA.R2,
            all_data.SPU_CHA.R3,
            all_data.SPU_CHA.RDY,
            all_data.SPU_CHA.TEST,
            all_data.SPU_CHA.XXXX,

            all_data.SPU_CHB.READ,
            all_data.SPU_CHB.N_DATA_FP,
            all_data.SPU_CHB.T_DATA_FP,
            all_data.SPU_CHB.F1_DATA_FP,
            all_data.SPU_CHB.F2_DATA_FP,
            all_data.SPU_CHB.F3_DATA_FP,
            all_data.SPU_CHB.EMR_N_THRESHOLD,
            all_data.SPU_CHB.WRN_N_THRESHOLD,
            all_data.SPU_CHB.EMR_T_THRESHOLD,
            all_data.SPU_CHB.WRN_T_THRESHOLD,
            all_data.SPU_CHB.EMR_N,
            all_data.SPU_CHB.WRN_N,
            all_data.SPU_CHB.EMR_T,
            all_data.SPU_CHB.WRN_T,
            all_data.SPU_CHB.R1,
            all_data.SPU_CHB.R2,
            all_data.SPU_CHB.R3,
            all_data.SPU_CHB.RDY,
            all_data.SPU_CHB.TEST,
            all_data.SPU_CHB.XXXX,
            
            all_data.PLC.READ,
            all_data.PLC.IG01,
            all_data.PLC.IG02,
            all_data.PLC.IG03,
            all_data.PLC.IG04,
            all_data.PLC.JT01,
            all_data.PLC.JT02,
            all_data.PLC.JT03,
            all_data.PLC.KT01,
            all_data.PLC.TT01,
            all_data.PLC.TT02,
            all_data.PLC.TT03,
            all_data.PLC.TT04,
            all_data.PLC.TT05,
            all_data.PLC.CT01,
            all_data.PLC.CT02,
            all_data.PLC.FI01,
            all_data.PLC.FI02,
            all_data.PLC.LT01,
            all_data.PLC.PHT01,
            all_data.PLC.TTXX,
            all_data.PLC.TT06,
            all_data.PLC.TT07,
            all_data.PLC.TT08,
            all_data.PLC.RT01,
            all_data.PLC.RT02,
            all_data.PLC.RT03,
            all_data.PLC.RT04,
            all_data.PLC.RT05,
            all_data.PLC.RT06,
            all_data.PLC.Reserva_AI11,
            all_data.PLC.Reserva_AI12,
            all_data.PLC.Reserva_AI13,
            all_data.PLC.Reserva_AI14,
            all_data.PLC.Reserva_AI15,
            all_data.PLC.Reserva_AI16,
            all_data.PLC.Reserva_AI17,
            all_data.PLC.Reserva_AI18,
            all_data.PLC.Reserva_AI19,
            all_data.PLC.Reserva_AI20,
            all_data.PLC.Reserva_AI21);
    return buffer;
}