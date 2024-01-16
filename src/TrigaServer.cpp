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

#include "TrigaServer.hpp"

TrigaServer::TrigaServer(const char* spu_sp1,//SPU_CH_A serial port
                         const char* spu_sp2,//SPU_CH_B serial port
                         const char* clp_ip, //CLP IP
                         u_int16_t clp_port, //CPL Port
                         boost::asio::io_service& io_service, //Service of server
                         short port) //Port of server
                        :spuChA(spu_sp1),
                         spuChB(spu_sp2),
                         plc(clp_ip,clp_port),
                         acceptor_(io_service,
                         tcp::endpoint(tcp::v4(), port)),
                         socket_(io_service)
{
    startThreads();
    StartAccept();
}


void TrigaServer::startThreads() // Método para Threads
{
    //Iniciar todas as treads necessárias
    std::thread spuChAThread(&TrigaServer::readModbusRTU, this, std::ref(spuChA));
    std::thread spuChBThread(&TrigaServer::readModbusRTU, this, std::ref(spuChB));
    std::thread plcThread   (&TrigaServer::readModbusTCP, this, std::ref(plc));
    #ifdef TestMax
        std::thread serverThread0   (&TrigaServer::handleTCPClients, this);
        std::thread serverThread1   (&TrigaServer::handleTCPClients, this);
        std::thread serverThread2   (&TrigaServer::handleTCPClients, this);
        std::thread serverThread3   (&TrigaServer::handleTCPClients, this);
        std::thread serverThread4   (&TrigaServer::handleTCPClients, this);
        std::thread serverThread5   (&TrigaServer::handleTCPClients, this);
        std::thread serverThread6   (&TrigaServer::handleTCPClients, this);
        std::thread serverThread7   (&TrigaServer::handleTCPClients, this);
        std::thread serverThread8   (&TrigaServer::handleTCPClients, this);
        std::thread serverThread9   (&TrigaServer::handleTCPClients, this);
    #endif

    //Esperar todas as threads terminarem (idelmente não deveria terminar)
    spuChAThread.join();
    spuChBThread.join();
    plcThread.join();
}

void TrigaServer::StartAccept() {
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec) {
            if (!ec) {
                std::thread([this]() { handleTCPClients(); }).detach();
            }

            StartAccept();
        });
}

void TrigaServer::SendData(const ALL_DATA& data) {
    // Converte a estrutura de dados para JSON
    Json::Value jsonData;
    jsonData["SPU_CHA_READ"           ].append(data.SPU_CHA.READ);
    jsonData["SPU_CHA_N_DATA_FP"      ].append(data.SPU_CHA.N_DATA_FP);
    jsonData["SPU_CHA_T_DATA_FP"      ].append(data.SPU_CHA.T_DATA_FP);
    jsonData["SPU_CHA_F1_DATA_FP"     ].append(data.SPU_CHA.F1_DATA_FP);
    jsonData["SPU_CHA_F2_DATA_FP"     ].append(data.SPU_CHA.F2_DATA_FP);
    jsonData["SPU_CHA_F3_DATA_FP"     ].append(data.SPU_CHA.F3_DATA_FP);
    jsonData["SPU_CHA_EMR_N_THRESHOLD"].append(data.SPU_CHA.EMR_N_THRESHOLD);
    jsonData["SPU_CHA_WRN_N_THRESHOLD"].append(data.SPU_CHA.WRN_N_THRESHOLD);
    jsonData["SPU_CHA_EMR_T_THRESHOLD"].append(data.SPU_CHA.EMR_T_THRESHOLD);
    jsonData["SPU_CHA_WRN_T_THRESHOLD"].append(data.SPU_CHA.WRN_T_THRESHOLD);
    jsonData["SPU_CHA_EMR_N"          ].append(data.SPU_CHA.EMR_N);
    jsonData["SPU_CHA_WRN_N"          ].append(data.SPU_CHA.WRN_N);
    jsonData["SPU_CHA_EMR_T"          ].append(data.SPU_CHA.EMR_T);
    jsonData["SPU_CHA_WRN_T"          ].append(data.SPU_CHA.WRN_T);
    jsonData["SPU_CHA_R1"             ].append(data.SPU_CHA.R1);
    jsonData["SPU_CHA_R2"             ].append(data.SPU_CHA.R2);
    jsonData["SPU_CHA_R3"             ].append(data.SPU_CHA.R3);
    jsonData["SPU_CHA_RDY"            ].append(data.SPU_CHA.RDY);
    jsonData["SPU_CHA_TEST"           ].append(data.SPU_CHA.TEST);
    jsonData["SPU_CHA_XXXX"           ].append(data.SPU_CHA.XXXX);
    jsonData["SPU_CHA_READ"           ].append(data.SPU_CHA.READ);
    jsonData["SPU_CHB_N_DATA_FP"      ].append(data.SPU_CHB.N_DATA_FP);
    jsonData["SPU_CHB_T_DATA_FP"      ].append(data.SPU_CHB.T_DATA_FP);
    jsonData["SPU_CHB_F1_DATA_FP"     ].append(data.SPU_CHB.F1_DATA_FP);
    jsonData["SPU_CHB_F2_DATA_FP"     ].append(data.SPU_CHB.F2_DATA_FP);
    jsonData["SPU_CHB_F3_DATA_FP"     ].append(data.SPU_CHB.F3_DATA_FP);
    jsonData["SPU_CHB_EMR_N_THRESHOLD"].append(data.SPU_CHB.EMR_N_THRESHOLD);
    jsonData["SPU_CHB_WRN_N_THRESHOLD"].append(data.SPU_CHB.WRN_N_THRESHOLD);
    jsonData["SPU_CHB_EMR_T_THRESHOLD"].append(data.SPU_CHB.EMR_T_THRESHOLD);
    jsonData["SPU_CHB_WRN_T_THRESHOLD"].append(data.SPU_CHB.WRN_T_THRESHOLD);
    jsonData["SPU_CHB_EMR_N"          ].append(data.SPU_CHB.EMR_N);
    jsonData["SPU_CHB_WRN_N"          ].append(data.SPU_CHB.WRN_N);
    jsonData["SPU_CHB_EMR_T"          ].append(data.SPU_CHB.EMR_T);
    jsonData["SPU_CHB_WRN_T"          ].append(data.SPU_CHB.WRN_T);
    jsonData["SPU_CHB_R1"             ].append(data.SPU_CHB.R1);
    jsonData["SPU_CHB_R2"             ].append(data.SPU_CHB.R2);
    jsonData["SPU_CHB_R3"             ].append(data.SPU_CHB.R3);
    jsonData["SPU_CHB_RDY"            ].append(data.SPU_CHB.RDY);
    jsonData["SPU_CHB_TEST"           ].append(data.SPU_CHB.TEST);
    jsonData["SPU_CHB_XXXX"           ].append(data.SPU_CHB.XXXX);
    jsonData["PLC_READ"               ].append(data.PLC.READ);
    jsonData["PLC_CH_PAR"             ].append(data.PLC.CH_PAR);
    jsonData["PLC_CH_LOG"             ].append(data.PLC.CH_LOG);
    jsonData["PLC_CH_LIN"             ].append(data.PLC.CH_LIN);
    jsonData["PLC_CH_PER"             ].append(data.PLC.CH_PER);
    // Converte o JSON para string
    Json::StreamWriterBuilder writer;
    std::string jsonString = Json::writeString(writer, jsonData);

    // Envia a string JSON para o cliente
    boost::asio::write(socket_, boost::asio::buffer(jsonString.c_str(), jsonString.length()));
}
// Servidor TCP/IP
void TrigaServer::handleTCPClients()
{
    while (true)
    {
        auto data_local_spuChA = std::shared_ptr <SPU_DATA> (new SPU_DATA);
        auto data_local_spuChB = std::shared_ptr <SPU_DATA> (new SPU_DATA);
        auto data_local_plc    = std::shared_ptr <PLC_DATA> (new PLC_DATA);

        data_local_spuChA = data_global_spuChA.load();
        data_local_spuChB = data_global_spuChB.load();
        data_local_plc = data_global_plc.load();

        #ifndef TestMax
        try {
            // Leitura da taxa do cliente
            float rate;
            socket_.receive(boost::asio::buffer(&rate, sizeof(rate)));

            // Enviar dados na taxa especificada
            while (true) {
                ALL_DATA data;
                SendData(data);
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000 / rate)));
            }
        } catch (const std::exception& e) {
            std::cerr << "Erro na comunicação com o cliente: " << e.what() << std::endl;
        }
        #endif
    }
}

// Ler dados da SPU usando libModbusSystematomSPU
void TrigaServer::readModbusRTU(libModbusSystematomSPU& spu)
{
    while (true)
    {
        //Criação do ponteiro inteligente local
        auto data_local = std::shared_ptr <SPU_DATA> (new SPU_DATA);
        //Realizando leitura e salvando informação se foi lido
        data_local->READ = spu.readAllRegisters(100);
        

        // Se for lido, passar informações para o ponteiro inteligente local
        if(data_local->READ)
        {
            // Obter os valores lidos
            data_local->N_DATA_FP       = spu.get_N_DATA_FP();
            data_local->T_DATA_FP       = spu.get_T_DATA_FP();
            data_local->F1_DATA_FP      = spu.get_F1_DATA_FP();
            data_local->F2_DATA_FP      = spu.get_F2_DATA_FP();
            data_local->F3_DATA_FP      = spu.get_F3_DATA_FP();
            data_local->EMR_N_THRESHOLD = spu.get_EMR_N_THRESHOLD();
            data_local->WRN_N_THRESHOLD = spu.get_WRN_N_THRESHOLD();
            data_local->EMR_T_THRESHOLD = spu.get_EMR_T_THRESHOLD();
            data_local->WRN_T_THRESHOLD = spu.get_WRN_T_THRESHOLD();
            data_local->EMR_N           = spu.get_EMR_N();
            data_local->WRN_N           = spu.get_WRN_N();
            data_local->EMR_T           = spu.get_EMR_T();
            data_local->WRN_T           = spu.get_WRN_T();
            data_local->R1              = spu.get_R1();
            data_local->R2              = spu.get_R2();
            data_local->R3              = spu.get_R3();
            data_local->RDY             = spu.get_RDY();
            data_local->TEST            = spu.get_TEST();
            data_local->XXXX            = spu.get_XXXX();
        }

        //Passar para o ponteiro inteligente global
        if (spu.get_portname() == "/dev/ttyF0") data_global_spuChA.store(data_local);
        else                                    data_global_spuChB.store(data_local);
    }
}
// Ler dados Modbus do PLC usando libModbusSiemensPLC
void TrigaServer::readModbusTCP(libModbusMaestecPLC& plc) 
{
    while (true)
    {
        auto data_local = std::shared_ptr <PLC_DATA> (new PLC_DATA);

        data_local->READ   = 1;//plc.readAllRegisters(100);
        data_local->CH_LIN = plc.get_Var1();
        data_local->CH_LOG = plc.get_Var2();
        data_local->CH_PAR = plc.get_Var1();
        data_local->CH_PER = plc.get_Var2();
        data_global_plc.store(data_local);
    }
}
