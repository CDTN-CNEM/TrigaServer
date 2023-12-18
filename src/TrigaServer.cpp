#include "libModbusSystematomSPU.h"
//#inclUde "libModbusSiemensCLP.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>

std::string logTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M-%S");
    return ss.str();
}

int main() {
    //libModbusSiemensCLP CLP(); //CLP Siemens
    libModbusSystematomSPU SPUch[2] = {
        "/dev/ttyF0", // SPU channel A
        "/dev/ttyF1"  // SPU channel B
    };

    std::string filename = "TrigaLog-" + logTime() + ".log";;
    std::ofstream logfile(filename, std::ios::app);

    if (logfile.is_open()) {
        int num_leituras = 10; 
        for (int i = 0; i < num_leituras; i++) {
            SPUch[0].readAllRegisters(100);
            logfile << logTime() << "\t Canal A \t N = " << SPUch[0].get_N_DATA_FP() << "\t T = " << SPUch[0].get_T_DATA_FP();// << std::endl;
            SPUch[1].readAllRegisters(100);
            logfile /*<< logTime()*/ << "\t Canal B \t N = " << SPUch[1].get_N_DATA_FP() << "\t T = " << SPUch[1].get_T_DATA_FP() << std::endl;
        }
        logfile << "---------------------------" << std::endl;

        logfile.close();

        std::cout << "Valores escritos no arquivo de log: " << filename << std::endl;
    } else {
        std::cerr << "Não foi possível abrir o arquivo de log para escrita." << std::endl;
    }

    return 0;
}
