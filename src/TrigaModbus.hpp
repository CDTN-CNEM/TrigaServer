#ifndef TRIGA_MODBUS_H
#define TRIGA_MODBUS_H

#include <string>
#include <modbus/modbus.h>

class TrigaModbus {
public:
    TrigaModbus(const std::string& ipAddress, int port);
    ~TrigaModbus();

    bool connect();
    bool readRegisters(int regAddr, int numRegs, uint16_t* buffer);

private:
    std::string ipAddress;
    int port;
    modbus_t* ctx;
};

#endif  // TRIGA_MODBUS_H
