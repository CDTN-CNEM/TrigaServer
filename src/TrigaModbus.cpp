#include "TrigaModbus.hpp"
#include <iostream>

TrigaModbus::TrigaModbus(const std::string& ipAddress, int port)
    : ipAddress(ipAddress), port(port), ctx(nullptr) {}

TrigaModbus::~TrigaModbus() {
    if (ctx != nullptr) {
        modbus_free(ctx);
    }
}

bool TrigaModbus::connect() {
    ctx = modbus_new_tcp(ipAddress.c_str(), port);
    if (ctx == nullptr) {
        std::cerr << "Não foi possível criar a estrutura Modbus: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    if (modbus_connect(ctx) == -1) {
        std::cerr << "Falha ao conectar ao dispositivo Modbus: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx);
        ctx = nullptr;
        return false;
    }

    return true;
}

bool TrigaModbus::readRegisters(int regAddr, int numRegs, uint16_t* buffer) {
    if (ctx == nullptr) {
        std::cerr << "Conexão Modbus não estabelecida." << std::endl;
        return false;
    }

    int readResult = modbus_read_registers(ctx, regAddr, numRegs, buffer);

    if (readResult == -1) {
        std::cerr << "Falha ao ler registradores: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    return true;
}
