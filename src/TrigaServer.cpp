#include "TrigaModbus.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>

std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << "TrigaLog-" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M-%S") << ".log";

    return ss.str();
}

int main() {
    TrigaModbus plc("127.0.0.1", 502);

    if (!plc.connect()) {
        std::cerr << "Não foi possível conectar ao PLC";
        return -1;
    }

    int regAddr = 0;
    int numRegs = 10;
    uint16_t* regsBuffer = new uint16_t[numRegs];

    if (plc.readRegisters(regAddr, numRegs, regsBuffer)) {
        std::cout << "Valores lidos dos registradores:" << std::endl;
        for (int i = 0; i < numRegs; i++) {
            std::cout << "Registrador " << regAddr + i << ": " << regsBuffer[i] << std::endl;
        }

        std::string filename = getCurrentDateTime();
        std::ofstream logfile(filename, std::ios::app);

        if (logfile.is_open()) {
            logfile << "Valores lidos dos registradores:" << std::endl;
            for (int i = 0; i < numRegs; i++) {
                logfile << "Registrador " << regAddr + i << ": " << regsBuffer[i] << std::endl;
            }

            logfile << "---------------------------" << std::endl;

            logfile.close();
            std::cout << "Valores escritos no arquivo de log: " << filename << std::endl;
        } else {
            std::cerr << "Não foi possível abrir o arquivo de log para escrita." << std::endl;
        }
    }

    delete[] regsBuffer;

    return 0;
}
