#include <iostream>

#include "model/enigma/rotor.hpp"

int main() {
    tools::filesystem::monitoring fsm;
    tools::filesystem::file_t file;
    // file.set_path(std::string("/mnt/d/Projects/CPP_Crypto/temporary_file.txt"));
    fsm.create_file(fs::path("/mnt/d/Projects/CPP_Crypto/temporary_file.txt"));
    // fsm.read_file(file);
    // std::string p{file.get_path()};
    // file.set_path(p.substr(0, p.size() - 4) + "_copy" + p.substr(p.size() - 4, p.size()));
    // fsm.create_file(file);
    std::cout << "SIZE: " << file.size() << std::endl << std::endl;
    std::cout << "PATH: " << file.get_path() << std::endl << std::endl;
    std::cout << "TEXT:\n" << file.get_text() << std::endl;
    return 0;
}
