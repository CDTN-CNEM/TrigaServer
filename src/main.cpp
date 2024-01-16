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

void readConfigFile(const std::string& filename, const char*& spu_sp1, const char*& spu_sp2, const char*& clp_ip, u_int16_t& clp_port, short& port) {
    std::ifstream configFile(filename);
    
    if (!configFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de configuração: " << filename << std::endl;
        return;
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
                spu_sp1 = value.c_str();
            } else if (key == "spu_sp2") {
                spu_sp2 = value.c_str();
            } else if (key == "clp_ip") {
                clp_ip = value.c_str();
            } else if (key == "clp_port") {
                clp_port = std::stoi(value);
            } else if (key == "port") {
                port = std::stoi(value);
            }
        }
    }

    configFile.close();
}

int main(int argc, char* argv[])
{
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
        } else {
            std::cout << options.help() << std::endl;
            return 1;
        }
    }

    const char* spu_sp1;//SPU_CH_A serial port
    const char* spu_sp2;//SPU_CH_B serial port
    const char* clp_ip; //CLP IP
    u_int16_t clp_port; //CPL Port
    short port; //Port of server
    readConfigFile("TrigaServer.conf", spu_sp1, spu_sp2, clp_ip, clp_port, port);
    try {
        boost::asio::io_service io_service;
        TrigaServer server(spu_sp1, spu_sp2, clp_ip, clp_port, io_service, port);
        io_service.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
