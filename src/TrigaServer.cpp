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

const char CSV_HEADER[] = 
"SPU_CHA_STATE;SPU_CHA_TIME_Y;SPU_CHA_TIME_Mo;SPU_CHA_TIME_D;SPU_CHA_TIME_H;SPU_CHA_TIME_Mi;SPU_CHA_TIME_S;SPU_CHA_TIME_MS;SPU_CHA_N_DATA_FP;SPU_CHA_T_DATA_FP;SPU_CHA_F1_DATA_FP;SPU_CHA_F2_DATA_FP;SPU_CHA_F3_DATA_FP;SPU_CHA_EMR_N_THRESHOLD;SPU_CHA_WRN_N_THRESHOLD;SPU_CHA_EMR_T_THRESHOLD;SPU_CHA_WRN_T_THRESHOLD;SPU_CHA_EMR_N;SPU_CHA_WRN_N;SPU_CHA_EMR_T;SPU_CHA_WRN_T;SPU_CHA_R1;SPU_CHA_R2;SPU_CHA_R3;SPU_CHA_RDY;SPU_CHA_TEST;SPU_CHA_XXXX;SPU_CHB_STATE;SPU_CHB_TIME_Y;SPU_CHB_TIME_Mo;SPU_CHB_TIME_D;SPU_CHB_TIME_H;SPU_CHB_TIME_Mi;SPU_CHB_TIME_S;SPU_CHB_TIME_MS;SPU_CHB_N_DATA_FP;SPU_CHB_T_DATA_FP;SPU_CHB_F1_DATA_FP;SPU_CHB_F2_DATA_FP;SPU_CHB_F3_DATA_FP;SPU_CHB_EMR_N_THRESHOLD;SPU_CHB_WRN_N_THRESHOLD;SPU_CHB_EMR_T_THRESHOLD;SPU_CHB_WRN_T_THRESHOLD;SPU_CHB_EMR_N;SPU_CHB_WRN_N;SPU_CHB_EMR_T;SPU_CHB_WRN_T;SPU_CHB_R1;SPU_CHB_R2;SPU_CHB_R3;SPU_CHB_RDY;SPU_CHB_TEST;SPU_CHB_XXXX;PLC_ORIG_STATE;PLC_ORIG_TIME;PLC_ORIG_TIME_Mo;PLC_ORIG_TIME_D;PLC_ORIG_TIME_H;PLC_ORIG_TIME_Mi;PLC_ORIG_TIME_S;PLC_ORIG_TIME_MS;PLC_ORIG_BarraReg;PLC_ORIG_BarraCon;PLC_ORIG_BarraSeg;PLC_ORIG_CLogALog;PLC_ORIG_CLogALin;PLC_ORIG_CLogAPer;PLC_ORIG_CParALin;PLC_ORIG_CParALog;PLC_ORIG_CParAPer;PLC_ORIG_CLogARea;PLC_ORIG_CLin;PLC_ORIG_CPer;PLC_ORIG_SRadAre;PLC_ORIG_SRadEntPri;PLC_ORIG_SRadPoc;PLC_ORIG_SRadRes;PLC_ORIG_SRadSaiSec;PLC_ORIG_SRadAer;PLC_ORIG_SVasPri;PLC_ORIG_SPt100Poco;PLC_ORIG_SPt100EntPri;PLC_ORIG_SPt100SaiPri;PLC_ORIG_SPt100EntSec;PLC_ORIG_SPt100SaiSec;PLC_ORIG_STpPoc1;PLC_ORIG_STpPoc2;PLC_ORIG_STpLen;PLC_ORIG_SConPoc;PLC_ORIG_SConSaiPri;PLC_CONV_STATE;PLC_CONV_TIME;PLC_CONV_TIME_Mo;PLC_CONV_TIME_D;PLC_CONV_TIME_H;PLC_CONV_TIME_Mi;PLC_CONV_TIME_S;PLC_CONV_TIME_MS;PLC_CONV_BarraReg;PLC_CONV_BarraCon;PLC_CONV_BarraSeg;PLC_CONV_CLogALog;PLC_CONV_CLogALin;PLC_CONV_CLogAPer;PLC_CONV_CParALin;PLC_CONV_CParALog;PLC_CONV_CParAPer;PLC_CONV_CLogARea;PLC_CONV_CLin;PLC_CONV_CPer;PLC_CONV_SRadAre;PLC_CONV_SRadEntPri;PLC_CONV_SRadPoc;PLC_CONV_SRadRes;PLC_CONV_SRadSaiSec;PLC_CONV_SRadAer;PLC_CONV_SVasPri;PLC_CONV_SPt100Poco;PLC_CONV_SPt100EntPri;PLC_CONV_SPt100SaiPri;PLC_CONV_SPt100EntSec;PLC_CONV_SPt100SaiSec;PLC_CONV_STpPoc1;PLC_CONV_STpPoc2;PLC_CONV_STpLen;PLC_CONV_SConPoc;PLC_CONV_SConSaiPri;\n";

const char * CSV_TEMPLATE = 
"%d;%d;%d;%d;%d;%d;%d;%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n";

TrigaServer::TrigaServer(std::string spu_sp1,//SPU_CH_A serial port
                         std::string spu_sp2,//SPU_CH_B serial port
                         std::string clp_adress, //CLP IP
                         std::string clp_conv_file,
                         int error_interval_plc,
                         int error_interval_spu) //CPL Port
                        :spuChA(spu_sp1),
                         spuChB(spu_sp2),
                         plc(clp_adress,clp_conv_file)
{
    adressSpuA = spu_sp1;
    adressSpuB = spu_sp2;
    errorIntervalSPU = error_interval_plc;
    errorIntervalPLC = error_interval_spu;
    startReadThreads();
}

TrigaServer::~TrigaServer() {}

std::vector<int> TrigaServer::state()
{
    auto localSpuA = data_global_spuCh[0].load();
    auto localSpuB = data_global_spuCh[1].load();
    auto localPlc = data_global_plc.load();

    return {localSpuA.get()->STATE,
            localSpuB.get()->STATE,
            localPlc.get()->STATE};
}

std::vector<float> TrigaServer::readPP()
{
    auto localSpuA = data_global_spuCh[0].load();
    auto localSpuB = data_global_spuCh[1].load();
    auto localPlc = data_global_plc.load();

    return {localSpuA.get()->N_DATA_FP,
            localSpuA.get()->T_DATA_FP,
            localSpuB.get()->N_DATA_FP,
            localSpuB.get()->T_DATA_FP,
            localPlc.get()->CLogALin,
            localPlc.get()->CLogALog,
            localPlc.get()->CLogAPer};
}

void TrigaServer::startReadThreads() // Método para Threads
{
    //Iniciar todas as treads necessárias
    std::thread spuChAThread(&TrigaServer::readModbusRTU, this, std::ref(spuChA));
    std::thread spuChBThread(&TrigaServer::readModbusRTU, this, std::ref(spuChB));
    std::thread plcThread   (&TrigaServer::readOpcTCP,    this, std::ref(plc));
    #ifdef TestMax
        for (int i = 0; i < 10; i++) {
            std::thread serverThread(&TrigaServer::handleTCPClients, this, false);
            serverThread.detach();
        }
    #endif

    //Desatar das readThreads
    spuChAThread.detach();
    spuChBThread.detach();
    plcThread.detach();
}

void TrigaServer::createServer(int port, bool sendCsv) {
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
    //std::cout << "[startServer] Server started on port " << port << std::endl;

    while(true) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            std::cerr << "[startServer] Error on accept" << std::endl;
            continue;
        }

        //std::cout << "[startServer] Client connected" << std::endl;
        
        std::thread clientThread(&TrigaServer::handleTCPClients, this, clientSocket, sendCsv);
        clientThread.detach();
    }
}

void TrigaServer::handleTCPClients(int clientSocket, bool sendCsv)
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
    //Caso algum digito não numero for recebido, encerre a execução
    for (int i = 0; i < n-1; ++i) if (!isdigit(buffer[i])) 
    {
        std::cerr << "[handleTCPClients] Error: client sent a not number" << std::endl;
        close(clientSocket);
        return;
    }
    // Parse received data (assuming it's a number)
    int interval = std::stoi(std::string(buffer, n));

    //std::cout << "[handleTCPClients] Received interval: " << interval << "ms" << std::endl;

    if(sendCsv)
    {
        // Create new thread
        std::thread([this, interval, clientSocket]()
        {
            auto data_local_spuChA = std::shared_ptr <SPU_DATA> (new SPU_DATA);
            auto data_local_spuChB = std::shared_ptr <SPU_DATA> (new SPU_DATA);
            auto data_local_plc    = std::shared_ptr <PLC_DATA> (new PLC_DATA);
            ALL_DATA data;

            if(send(clientSocket, CSV_HEADER, sizeof(CSV_HEADER), 0) > 0)
                while(true)
                {
                    data_local_spuChA = data_global_spuCh[0].load();
                    data_local_spuChB = data_global_spuCh[1].load();
                    data_local_plc = data_global_plc.load();

                    data.SPU_CHA = *data_local_spuChA;
                    data.SPU_CHB = *data_local_spuChB;
                    data.PLC    = *data_local_plc;
                    
                    std::string csv = genCsv(data);

                    if(send(clientSocket, csv.c_str(), csv.size(), 0) <= 0) break;

                    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                }
        }).detach();
    }
    else
    {
        // Create new thread
        std::thread([this, interval, clientSocket]()
        {
            auto data_local_spuChA = std::shared_ptr <SPU_DATA> (new SPU_DATA);
            auto data_local_spuChB = std::shared_ptr <SPU_DATA> (new SPU_DATA);
            auto data_local_plc    = std::shared_ptr <PLC_DATA> (new PLC_DATA);
            ALL_DATA data;
            while(true)
            {
                data_local_spuChA = data_global_spuCh[0].load();
                data_local_spuChB = data_global_spuCh[1].load();
                data_local_plc    = data_global_plc.load();

                #ifndef TestMax
                    data.SPU_CHA = *data_local_spuChA;
                    data.SPU_CHB = *data_local_spuChB;
                    data.PLC     = *data_local_plc;
                    
                    // Convert ALL_DATA to a char pointer
                    char* buffer = reinterpret_cast<char*>(&data);
                    size_t size = sizeof(data);

                    // Send data
                    if(send(clientSocket, buffer, size, 0) ==-1) break;

                    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                #endif
            }
        }).detach();
    }
}

// Ler dados da SPU usando libModbusSystematomSPU
void TrigaServer::readModbusRTU(libModbusSystematomSPU& spu)
{
    //Criação do ponteiro inteligente local
    auto data_local = std::shared_ptr <SPU_DATA> (new SPU_DATA);

    //Definindo qual SPU essa thread representa
    int adressSpu;
    if      (spu.get_portname() == adressSpuB) adressSpu = 0;
    else if (spu.get_portname() == adressSpuA) adressSpu = 1;
    else
    {
        std::cerr << "ERRO TrigaServer::readModbusRTU: erro em get_portname()";
        return;
    }

    while (true)
    {
        //Realizando leitura e salvando informação se foi lido
        *data_local  = spu.get_all();
        //Passar para o ponteiro inteligente global spuChA
        data_global_spuCh[adressSpu].store(data_local);
        //Se o valor de STATE for igual a 2 (desconectado) pause a thread por 1 segundo e tente reconectar
        if(data_local->STATE==2) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(errorIntervalSPU));
            spu.tryConnect();
        }
        //OBS: A pausa TEM que ser depois de armazenar data_local no ponteiro global
        //Caso contrario o valor poderá ficar congelado no ponteiro global com STATE = 0.
    }
}

// Ler dados Modbus do PLC usando libModbusSiemensPLC
void TrigaServer::readOpcTCP(libOpcTrigaPLC& plc) 
{
    //Mesma lógica de readModbusRTU
    auto data_local = std::shared_ptr <PLC_DATA> (new PLC_DATA);
    while (true)
    {
        *data_local = plc.get_all_conv();
        data_global_plc.store(data_local);
        if(data_local->STATE) //Caso STATE for diferente de 0
        {
            std::this_thread::sleep_for(std::chrono::seconds(errorIntervalPLC));
            if(data_local->STATE==2) //Caso o erro seja "desconexão"
                plc.tryConnect();
        }
    }
}

int_TIME TrigaServer::decodeTime(std::chrono::system_clock::time_point t)
{
    auto now = std::chrono::system_clock::to_time_t(t);
    std::tm tm_local = *std::localtime(&now);

    int_TIME time;
    time.year = tm_local.tm_year + 1900; // Ano: tm_year é anos desde 1900
    time.month = tm_local.tm_mon + 1; // Mês: tm_mon é 0-based
    time.day = tm_local.tm_mday; // Dia do mês
    time.hour = tm_local.tm_hour; // Hora
    time.minute = tm_local.tm_min; // Minuto
    time.second = tm_local.tm_sec; // Segundo

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count();
    time.millisecond = ms % 1000;
    return time;
}

std::string TrigaServer::genCsv(ALL_DATA all_data) 
{
    int_TIME SPU_CHA_TIME = decodeTime(all_data.SPU_CHA.TIME);
    int_TIME SPU_CHB_TIME = decodeTime(all_data.SPU_CHB.TIME);
    int_TIME PLC_TIME     = decodeTime(all_data.PLC.TIME);
    PLC_DATA plc_conv     = plc.convAllData(all_data.PLC, plc.fatorConv);

    char buffer[8192];

    sprintf(buffer, CSV_TEMPLATE, all_data.SPU_CHA.STATE,
            SPU_CHA_TIME.year,
            SPU_CHA_TIME.month,
            SPU_CHA_TIME.day,
            SPU_CHA_TIME.hour,
            SPU_CHA_TIME.minute,
            SPU_CHA_TIME.second,
            SPU_CHA_TIME.millisecond,
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

            all_data.SPU_CHB.STATE,
            SPU_CHB_TIME.year,
            SPU_CHB_TIME.month,
            SPU_CHB_TIME.day,
            SPU_CHB_TIME.hour,
            SPU_CHB_TIME.minute,
            SPU_CHB_TIME.second,
            SPU_CHB_TIME.millisecond,
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
            
            all_data.PLC.STATE,
            PLC_TIME.year,
            PLC_TIME.month,
            PLC_TIME.day,
            PLC_TIME.hour,
            PLC_TIME.minute,
            PLC_TIME.second,
            PLC_TIME.millisecond,
            all_data.PLC.BarraReg,
            all_data.PLC.BarraCon,
            all_data.PLC.BarraSeg,
            all_data.PLC.CLogALog,
            all_data.PLC.CLogALin,
            all_data.PLC.CLogAPer,
            all_data.PLC.CParALin,
            all_data.PLC.CParALog,
            all_data.PLC.CParAPer,
            all_data.PLC.CLogARea,
            all_data.PLC.CLin,
            all_data.PLC.CPer,
            all_data.PLC.SRadAre,
            all_data.PLC.SRadEntPri,
            all_data.PLC.SRadPoc,
            all_data.PLC.SRadRes,
            all_data.PLC.SRadSaiSec,
            all_data.PLC.SRadAer,
            all_data.PLC.SVasPri,
            all_data.PLC.SPt100Poco,
            all_data.PLC.SPt100EntPri,
            all_data.PLC.SPt100SaiPri,
            all_data.PLC.SPt100EntSec,
            all_data.PLC.SPt100SaiSec,
            all_data.PLC.STpPoc1,
            all_data.PLC.STpPoc2,
            all_data.PLC.STpLen,
            all_data.PLC.SConPoc,
            all_data.PLC.SConSaiPri,

            plc_conv.STATE,
            PLC_TIME.year,
            PLC_TIME.month,
            PLC_TIME.day,
            PLC_TIME.hour,
            PLC_TIME.minute,
            PLC_TIME.second,
            PLC_TIME.millisecond,
            plc_conv.BarraReg,
            plc_conv.BarraCon,
            plc_conv.BarraSeg,
            plc_conv.CLogALog,
            plc_conv.CLogALin,
            plc_conv.CLogAPer,
            plc_conv.CParALin,
            plc_conv.CParALog,
            plc_conv.CParAPer,
            plc_conv.CLogARea,
            plc_conv.CLin,
            plc_conv.CPer,
            plc_conv.SRadAre,
            plc_conv.SRadEntPri,
            plc_conv.SRadPoc,
            plc_conv.SRadRes,
            plc_conv.SRadSaiSec,
            plc_conv.SRadAer,
            plc_conv.SVasPri,
            plc_conv.SPt100Poco,
            plc_conv.SPt100EntPri,
            plc_conv.SPt100SaiPri,
            plc_conv.SPt100EntSec,
            plc_conv.SPt100SaiSec,
            plc_conv.STpPoc1,
            plc_conv.STpPoc2,
            plc_conv.STpLen,
            plc_conv.SConPoc,
            plc_conv.SConSaiPri
            );
    return buffer;
}
