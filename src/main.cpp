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
//main.cpp
#include "TrigaServer.h"
#include <cxxopts.hpp>
#include <fstream>
#include <string>
#include <chrono>
#include <csignal>

void showVersion()
{
    std::cout << "Development Version" << std::endl;
}

void showLicense()
{
    std::cout << "TrigaServer      Copyright (C) 2023-2024      Thalles Campagnani" << std::endl;
    std::cout << "This    program    comes    with    ABSOLUTELY    NO   WARRANTY;" << std::endl;
    std::cout << "This is free software,    and you are welcome to redistribute it" << std::endl;
    std::cout << "under certain conditions; For more details read the file LICENSE" << std::endl;
    std::cout << "that came together with the source code." << std::endl << std::endl;
}

struct CONFIG
{
    std::string spu_sp1             = "/dev/modbus/spuChA";       //SPU_CH_A serial port
    std::string spu_sp2             = "/dev/modbus/spuChB";       //SPU_CH_B serial port
    std::string plc_adress          = "192.168.1.1:4840";         //CLP IP:PORT
    std::string plc_conv_file       = "/etc/libOpcTrigaPLC.conf"; //CLP conv file
    short       port_raw            = 123;                        //Port of server RAW
    short       port_csv            = 1234;                       //Port of server CSV
    short       port_json           = 12345;                      //Port of server JSON
    int         error_interval_spu  = 2;                          //
    int         error_interval_plc  = 2;                          //
    int         monitor_interval    = 3;                          //
    bool        monitor_stdout      = 0;                          //
    bool        close               = 0;
};

CONFIG configOptions(int argc, char* argv[])
{
    CONFIG config;
    cxxopts::Options options("TrigaServer","TrigaServer is a software for GNU operating system to get the real-time values of the Nuclear Reator Triga IPR-R1 and share in network.");

    options.add_options()
        ("v,version",          "Show the program version")
        ("h,help",             "Show this help message")
        ("l,license",          "Show info of the license")
        ("m,monitor",          "Set system monitor to STD_OUT")
        ("SPU_CHA",            "Set spuChA device file",                                        cxxopts::value<std::string>())
        ("SPU_CHB",            "Set spuChB device file",                                        cxxopts::value<std::string>())
        ("PLC_ADRESS",         "Set PLC adress in format IP:PORT",                              cxxopts::value<std::string>())
        ("PLC_CONV_FILE",      "Set convertion file for PLC values",                            cxxopts::value<std::string>())
        ("PORT_RAW",           "Set port for the server in RAW format (set 0 to desactivate)",  cxxopts::value<int>())
        ("PORT_CSV",           "Set port for the server in CSV format",                         cxxopts::value<int>())
        ("PORT_JSON",          "Set port for the server in JSON format",                        cxxopts::value<int>())
        ("INTERVAL_ERROR_SPU", "Set reconnect interval of SPU in case of error",                cxxopts::value<int>())
        ("INTERVAL_ERROR_PLC", "Set reconnect interval of PLC in case of error",                cxxopts::value<int>())
        ("INTERVAL_MONITOR",   "Set interval of system monitor",                                cxxopts::value<int>());

    auto result = options.parse(argc, argv);

    if (result.count("version") || result.count("v"))
    {
        showVersion();
        config.close = true;
        return config;
    } 
    if (result.count("help") || result.count("h"))
    {
        std::cout << options.help() << std::endl;
        config.close = true;
        return config;
    } 
    if (result.count("license") || result.count("l"))
    {
        showLicense();
        config.close = true;
        return config;
    }

    if (result.count("monitor") || result.count("m"))  config.monitor_stdout = true;//Impor monitor na STD_OUT
    
    if (result.count("SPU_CHA"))             config.spu_sp1            = result["SPU_CHA"].as<std::string>();
    if (result.count("SPU_CHB"))             config.spu_sp2            = result["SPU_CHB"].as<std::string>();
    if (result.count("PLC_ADRESS"))          config.plc_adress         = result["PLC_ADRESS"].as<std::string>();
    if (result.count("PLC_CONV_FILE"))       config.plc_conv_file      = result["PLC_CONV_FILE"].as<std::string>();
    if (result.count("PORT_RAW"))            config.port_raw           = result["PORT_RAW"].as<int>();
    if (result.count("PORT_CSV"))            config.port_csv           = result["PORT_CSV"].as<int>();
    if (result.count("PORT_JSON"))           config.port_json          = result["PORT_JSON"].as<int>();
    if (result.count("INTERVAL_ERROR_SPU"))  config.error_interval_spu = result["INTERVAL_ERROR_SPU"].as<int>();
    if (result.count("INTERVAL_ERROR_PLC"))  config.error_interval_plc = result["INTERVAL_ERROR_PLC"].as<int>();
    if (result.count("INTERVAL_MONITOR"))    config.monitor_interval   = result["INTERVAL_MONITOR"].as<int>();

    return config;
}

int main(int argc, char* argv[])
{
    signal(SIGPIPE, SIG_IGN);
    
    CONFIG config = configOptions(argc, argv);
    if (config.close) return 0;

    TrigaServer server(config.spu_sp1,
                       config.spu_sp2,
                       config.plc_adress,
                       config.plc_conv_file,
                       config.error_interval_spu,
                       config.error_interval_plc);

    if(config.port_raw)  
    {
        std::thread serverThread    (&TrigaServer::createServer, &server, config.port_raw,  0);
        serverThread.detach();
    }
    if(config.port_csv)  
    {
        std::thread serverCsvThread (&TrigaServer::createServer, &server, config.port_csv,  1);
        serverCsvThread.detach();
    }
    if(config.port_json)
    {
        std::thread serverJsonThread(&TrigaServer::createServer, &server, config.port_json, 2);
        serverJsonThread.detach();
    }
    //Espere 3 segundo antes de abrir o monitor
    std::this_thread::sleep_for(std::chrono::seconds(3));

    //Monitor de sistema
    std::ofstream outputFile("/tmp/trigaserver.systemmonitor");
    while (true)
    {
        std::vector<int> state = server.state();
        std::vector<float> PP = server.readPP();
        
        std::string screen = "TrigaServer - System Monitor\n";
                    screen += "\nClients Side\n";
                    if(config.port_raw)  screen += "Server  RAW:   PORT=" + std::to_string(config.port_raw)           + "\n";
                    if(config.port_csv)  screen += "Server  CSV:   PORT=" + std::to_string(config.port_csv)           + "\n";
                    if(config.port_json) screen += "Server  JSON:  PORT=" + std::to_string(config.port_json)          + "\n";
                    //screen += "Num. Clients:?\n";
                    screen += "\nMachine Side\n";
                    screen += "SPU_A:    STATE="     + std::to_string(state[0]) + "\t\tPORT=" + config.spu_sp1        + "\n";
                    screen += "SPU_B:    STATE="     + std::to_string(state[1]) + "\t\tPORT=" + config.spu_sp2        + "\n";
                    screen += "PLC:      STATE="     + std::to_string(state[2]) + "\t\tPORT=" + config.plc_adress     + "\n";
                    screen += "\nImportante Values\n";
                    screen += "SPU_A:    N="         + std::to_string(PP[0])    + "\t\tT="    + std::to_string(PP[1]) + "\n";
                    screen += "SPU_B:    N="         + std::to_string(PP[2])    + "\t\tT="    + std::to_string(PP[3]) + "\n";
                    screen += "PLC:      N="         + std::to_string(PP[4])    + "\t\tT="    + std::to_string(PP[6]) + "\n";
                    screen += "      N_log="         + std::to_string(PP[5])                                          + "\n";
                    screen += "\n";
        
        if(config.monitor_stdout)
        {
            system("clear");
            std::cout  << screen;
            outputFile << screen;
        }
        else outputFile << screen;

        //Espere 1 segundo antes de abrir o monitor e a cada loop
        std::this_thread::sleep_for(std::chrono::seconds(config.monitor_interval));
    }
    outputFile.close();
    return 0;
}
