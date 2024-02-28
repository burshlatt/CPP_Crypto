#include <iostream>

#include "model/enigma/rotor.hpp"

int main() {
    s21::Rotor rotor;
    
    for (std::size_t i = 0; i < rotor.Size(); i++)
        std::cout << rotor[i] << " ";

    std::cout << std::endl;
    return 0;
}
