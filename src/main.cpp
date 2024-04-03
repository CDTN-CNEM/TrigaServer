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
    std::string spu_sp1   = "/dev/ttyUSB0";//SPU_CH_A serial port
    std::string spu_sp2   = "/dev/ttyUSB1";//SPU_CH_B serial port
    std::string plc_adress = "192.168.1.1:4840"; //PLC IP:PORT
    std::string plc_conv_file = "/etc/libOpcTrigaPLC.conf"; //Path to PLCconversion file
    short       port_raw  = 1234; //Port of server RAW
    short       port_json = 12345; //Port of server JSON
    int         error_interval_spu = 2;//
    int         error_interval_plc = 2;//
    int         monitor_stdout   = 0;//
    int         monitor_interval = 3;//
};

CONFIG readConfigFile(std::string filename)
{
    CONFIG config;
    
    std::ifstream configFile(filename);
    
    if (!configFile.is_open()) 
    {
        std::cerr << "[readConfigFile] Erro ao abrir o arquivo de configuração: " << filename << std::endl;
        filename = "trigaserver.conf";
        std::ifstream configFile(filename);
        std::cerr << "[readConfigFile] Tentando abrir: " << filename << std::endl;
        if (!configFile.is_open()) 
        {
            std::cerr << "[readConfigFile] Erro ao abrir o arquivo de configuração: " << filename << std::endl;
            std::cerr << "[readConfigFile] Configurações setadas como padrão" << std::endl;
            return config;
        }
    }

    std::string line;
    while (std::getline(configFile, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // Remova espaços em branco extras
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if      (key == "spu_sp1")            config.spu_sp1 = value.c_str();
            else if (key == "spu_sp2")            config.spu_sp2 = value.c_str();
            else if (key == "clp_adress")         config.plc_adress = value.c_str();
            else if (key == "plc_conv_file")      config.plc_conv_file = value.c_str();
            else if (key == "port_json")          config.port_json = std::stoi(value);
            else if (key == "port_raw")           config.port_raw = std::stoi(value);
            else if (key == "error_interval_spu") config.error_interval_spu = std::stoi(value);
            else if (key == "error_interval_plc") config.error_interval_plc = std::stoi(value);
            else if (key == "interval_monitor")   config.monitor_interval = std::stoi(value);            
        }
    }

    configFile.close();
    return config;

}

int main(int argc, char* argv[])
{
    std::string filename = "/etc/trigaserver.conf";
    cxxopts::Options options("TrigaServer","TrigaServer is a software for GNU operating system to get the real-time values of the Nuclear Reator Triga IPR-R1 and share in network.");
    bool monitor_stdout_arg = 0;

    options.add_options()
        ("v,version", "Show the program version")
        ("h,help", "Show this help message")
        ("l,license", "Show info of the license")
        ("c,config", "Change the configuration file")
        ("m,monitor", "Make the system monitor in STD_OUT");

    auto result = options.parse(argc, argv);

    if (argc >1)
    {
        if (result.count("version") || result.count("v")){
            showVersion();
            return 0;
        } else if (result.count("help") || result.count("h")){
            std::cout << options.help() << std::endl;
            return 0;
        } else if (result.count("license") || result.count("l")) {
            showLicense();
            return 0;
        } else if (result.count("config") || result.count("c")){
            if (result["config"].as<std::string>().empty()) {
                std::cerr << "Error: Please provide a filename for --config option." << std::endl;
                return 1;
            }
            filename = result["config"].as<std::string>();
        } else if (result.count("monitor") || result.count("m")){//Impor monitor na STD_OUT
            monitor_stdout_arg = true;
        } else {
            std::cout << options.help() << std::endl;
            return 1;
        }
    }


    CONFIG config = readConfigFile(filename);
    if (monitor_stdout_arg) config.monitor_stdout = true;

    TrigaServer server(config.spu_sp1,
                       config.spu_sp2,
                       config.plc_adress,
                       config.plc_conv_file,
                       config.error_interval_spu,
                       config.error_interval_plc);
    std::thread serverThread    (&TrigaServer::createServer, &server, config.port_raw, false);
    std::thread serverJsonThread(&TrigaServer::createServer, &server, config.port_json,true);

    serverThread.detach();
    serverJsonThread.detach();

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
                    screen += "Server  RAW:   PORT=" + std::to_string(config.port_raw)                                + "\n";
                    screen += "Server JSON:   PORT=" + std::to_string(config.port_json)                               + "\n";
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
