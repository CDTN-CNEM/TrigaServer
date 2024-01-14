/*
libModbusSystematomSPU is a library to communicate with the SystemAtom SPU
using MODBUS-RTU (RS-485) on a GNU operating system.
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

#include <iostream>
#include <thread>
#include <memory>
#include <iostream>
#include "libModbusSystematomSPU.hpp"
#include "libModbusMaestecPLC.hpp"

#define MuTeX//Mutex para proteção de leitura e escrita
#ifdef MuTeX
    #include <mutex>
#endif

struct SPU_DATA
{
    bool    READ            = 0;
    float   N_DATA_FP       = 0;
    float   T_DATA_FP       = 0;
    float   F1_DATA_FP      = 0;
    float   F2_DATA_FP      = 0;
    float   F3_DATA_FP      = 0;
    float   EMR_N_THRESHOLD = 0;
    float   WRN_N_THRESHOLD = 0;
    float   EMR_T_THRESHOLD = 0;
    float   WRN_T_THRESHOLD = 0;
    bool    EMR_N           = 0;
    bool    WRN_N           = 0;
    bool    EMR_T           = 0;
    bool    WRN_T           = 0;
    bool    R1              = 0;
    bool    R2              = 0;
    bool    R3              = 0;
    bool    RDY             = 0;
    bool    TEST            = 0;
    bool    XXXX            = 0;
};

struct PLC_DATA
{
    bool    READ            = 0;
    float   CH_PAR          = 0;
    float   CH_LOG          = 0;
    float   CH_LIN          = 0;
    float   CH_PER          = 0;
    
};

class TrigaServer
{
    public:
        TrigaServer() : spuChA("/dev/ttyF0"), spuChB("/dev/ttyF1"), plc("127.0.0.1",12345) {}

        void startServer() // Método para iniciar o servidor
        {
            //Iniciar todas as treads
            std::thread spuChAThread(&TrigaServer::readModbusRTU, this, std::ref(spuChA));
            std::thread spuChBThread(&TrigaServer::readModbusRTU, this, std::ref(spuChB));
            std::thread plcThread   (&TrigaServer::readModbusTCP, this, std::ref(plc));
            std::thread serverThread(&TrigaServer::handleTCPClients, this);

            //Esperar todas as threads terminarem (idelmente não deveria terminar)
            serverThread.join();
            spuChAThread.join();
            spuChBThread.join();
            plcThread.join();
        }

        void showLicense()
        {
            std::cout << "TrigaServer      Copyright (C) 2023-2024      Thalles Campagnani" << std::endl;
            std::cout << "This    program    comes    with    ABSOLUTELY    NO   WARRANTY;" << std::endl;
            std::cout << "This is free software,    and you are welcome to redistribute it" << std::endl;
            std::cout << "under certain conditions; For more details read the file LICENSE" << std::endl;
            std::cout << "that came together with the source code." << std::endl << std::endl;
        }

    private:
        // Objetos para comunicação Modbus nas duas portas seriais
        libModbusSystematomSPU spuChA;
        libModbusSystematomSPU spuChB;
        libModbusMaestecPLC    plc;

        #ifdef MuTeX
            std::mutex spuChAMutex;
            std::mutex spuChBMutex;
            std::mutex plcMutex;
        #endif

        //Ponteiros inteligentes globais
        std::shared_ptr <SPU_DATA> data_global_spuChA = std::shared_ptr <SPU_DATA> (new SPU_DATA);
        std::shared_ptr <SPU_DATA> data_global_spuChB = std::shared_ptr <SPU_DATA> (new SPU_DATA);
        std::shared_ptr <PLC_DATA> data_global_plc    = std::shared_ptr <PLC_DATA> (new PLC_DATA);
        
        // Servidor TCP/IP
        void handleTCPClients()
        {
            while (true)
            {
                auto data_local_spuChA = std::shared_ptr <SPU_DATA> (new SPU_DATA);
                auto data_local_spuChB = std::shared_ptr <SPU_DATA> (new SPU_DATA);
                auto data_local_plc    = std::shared_ptr <PLC_DATA> (new PLC_DATA);

                #ifdef MuTeX
                {
                    std::lock_guard<std::mutex> lock(spuChAMutex);
                    data_local_spuChA = data_global_spuChA;
                }
                {
                    std::lock_guard<std::mutex> lock(spuChBMutex);
                    data_local_spuChB = data_global_spuChB;
                }
                {
                    std::lock_guard<std::mutex> lock(plcMutex);
                    data_local_plc = data_global_plc;
                }
                #else
                    data_local_spuChA      = data_global_spuChA;
                    data_local_spuChB      = data_global_spuChB;
                    data_local_plc         = data_global_plc;
                #endif
            }
        }

        // Ler dados da SPU usando libModbusSystematomSPU
        void readModbusRTU(libModbusSystematomSPU& spu)
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
                if(spu.get_portname()=="/dev/ttyF0") 
                {
                    #ifdef MuTeX
                    {
                        std::lock_guard<std::mutex> lock(spuChAMutex);
                        data_global_spuChA = data_local;
                    }
                    #else
                        data_global_spuChA = data_local;
                    #endif
                }
                else
                {
                    #ifdef MuTeX
                    {
                        std::lock_guard<std::mutex> lock(spuChBMutex);
                        data_global_spuChB = data_local;
                    }
                    #else
                        data_global_spuChB = data_local;
                    #endif
                }
            }
        }
        // Ler dados Modbus do PLC usando libModbusSiemensPLC
        void readModbusTCP(libModbusMaestecPLC& plc) 
        {
            while (true)
            {
                auto data_local    = std::shared_ptr <PLC_DATA> (new PLC_DATA);

                data_local->READ   = 1;//plc.readAllRegisters(100);
                data_local->CH_LIN = plc.get_Var1();
                data_local->CH_LOG = plc.get_Var2();
                data_local->CH_PAR = plc.get_Var1();
                data_local->CH_PER = plc.get_Var2();

                #ifdef MuTeX
                {
                    std::lock_guard<std::mutex> lock(plcMutex);
                    data_global_plc = data_local;
                }
                #else
                    data_global_plc = data_local;
                #endif
            }
        }
};

int main()
{
    system("clear");
    TrigaServer server;
    server.showLicense();
    server.startServer();

    return 0;
}
