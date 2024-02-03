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
    std::string clp_ip    = "192.168.0.1"; //CLP IP
    u_int16_t   clp_port  = 4843; //CPL Port
    short       port_raw  = 1234; //Port of server
    short       port_json = 12345; //Port of server
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

            if (key == "spu_sp1") {
                config.spu_sp1 = value.c_str();
            } else if (key == "spu_sp2") {
                config.spu_sp2 = value.c_str();
            } else if (key == "clp_ip") {
                config.clp_ip = value.c_str();
            } else if (key == "clp_port") {
                config.clp_port = std::stoi(value);
            } else if (key == "port_raw") {
                config.port_raw = std::stoi(value);
            } else if (key == "port") {
                config.port_json = std::stoi(value);
            }
        }
    }

    configFile.close();
    return config;

}

int main(int argc, char* argv[])
{
    std::string filename = "/etc/trigaserver.conf";
    cxxopts::Options options("TrigaServer","TrigaServer is a software for GNU operating system to get the real-time values of the Nuclear Reator Triga IPR-R1 and share in network.");

    options.add_options()
        ("v,version", "Show the program version")
        ("h,help", "Show this help message")
        ("l,license", "Show info of the license");

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
        } else {
            std::cout << options.help() << std::endl;
            return 1;
        }
    }


    CONFIG config = readConfigFile(filename);
    TrigaServer server(config.spu_sp1,
                       config.spu_sp2, 
                       config.clp_ip, 
                       config.clp_port);
    std::thread serverThread    (&TrigaServer::startServer, &server, config.port_raw, false);
    std::thread serverJsonThread(&TrigaServer::startServer, &server, config.port_json,true);

    serverThread.detach();
    serverJsonThread.detach();

    std::string input;
    while (true) {
        std::cout << "Enter command ('exit' to quit, 'reset' to reset server): " << std::endl;
        std::cin >> input;

        if (input == "exit") {
            break;
        } else if (input == "reset") {
            server.~TrigaServer();
            new (&server) TrigaServer(config.spu_sp1,
                       config.spu_sp2, 
                       config.clp_ip, 
                       config.clp_port);
        }
    }
    return 0;
}
