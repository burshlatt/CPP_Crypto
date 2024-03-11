#ifndef CRYPTO_CONTROLLER_RSA_CONTROLLER_HPP
#define CRYPTO_CONTROLLER_RSA_CONTROLLER_HPP

#include <string_view>

#include "rsa.hpp"

namespace s21 {
class RSAController {
public:
    RSAController() = default;
    ~RSAController() = default;

public:
    void GenerateKeys(std::string_view dir) {
        rsa_.GenerateKeys(dir);
    }

    void Encrypt(std::string_view file_path, std::string_view key_path) {
        rsa_.Encode(file_path, key_path);
    }

    void Decrypt(std::string_view file_path, std::string_view key_path) {
        rsa_.Decode(file_path, key_path);
    }
    
private:
    RSA rsa_;
};
}  // namespace s21

#endif // CRYPTO_CONTROLLER_RSA_CONTROLLER_HPP
