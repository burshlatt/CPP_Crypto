#include <iostream>

#include "model/enigma/enigma.hpp"

int main() {
    tools::filesystem::monitoring fsm;
    s21::Enigma enigma(10);

    while (true) {
        std::string path{fsm.get_file_path()};

        if (path.empty())
            break;

        enigma.EncryptFile(path);
    }
    return 0;
}
