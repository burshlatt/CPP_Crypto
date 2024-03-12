#ifndef CRYPTO_CONTROLLER_DES_CONTROLLER_HPP
#define CRYPTO_CONTROLLER_DES_CONTROLLER_HPP

#include <string_view>

#include "des.hpp"

namespace s21 {
class DESController {
public:
    DESController() = default;
    ~DESController() = default;

public:
    void Encrypt(std::string_view file_path, std::string_view key_path) {
        rsa_.EncodeECB(file_path, key_path);
    }

    void Decrypt(std::string_view file_path, std::string_view key_path) {
        rsa_.DecodeECB(file_path, key_path);
    }
    
private:
    DES rsa_;
};
}  // namespace s21

#endif // CRYPTO_CONTROLLER_DES_CONTROLLER_HPP
