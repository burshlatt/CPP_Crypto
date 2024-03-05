#ifndef CRYPTO_CONTROLLER_ENIGMA_CONTROLLER_HPP
#define CRYPTO_CONTROLLER_ENIGMA_CONTROLLER_HPP

#include <memory>
#include <string_view>

#include "enigma.hpp"

namespace s21 {
class EnigmaController {
public:
    explicit EnigmaController(uint8_t num_rotors) {
        enigma_ = std::make_unique<Enigma>(num_rotors);
    }

    explicit EnigmaController(std::string_view path) {
        enigma_ = std::make_unique<Enigma>(path);
    }

    ~EnigmaController() = default;

public:
    void Encrypt(std::string_view path) {
        enigma_->Encrypt(path);
    }

private:
    std::unique_ptr<Enigma> enigma_;
};
}  // namespace s21

#endif // CRYPTO_CONTROLLER_ENIGMA_CONTROLLER_HPP
