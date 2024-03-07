#ifndef CRYPTO_MODEL_RSA_RSA_HPP
#define CRYPTO_MODEL_RSA_RSA_HPP

#include <string_view>

#include "tools.hpp"

namespace s21 {
class RSA {
private:
    using file_t = tools::filesystem::file_t;

public:
    RSA() = default;
    ~RSA() = default;

public:
    void GenerateKeys(std::string_view dir) {
        fs::path public_path(dir);
        fs::path private_path(dir);

        public_path /= "public_key";
        private_path /= "private_key";

        file_t public_file(public_path);
        file_t private_file(private_path);

        fsm_.create_file(public_file);
        fsm_.create_file(private_path);
    }

private:
    tools::filesystem::monitoring fsm_;
};
}  // namespace s21

#endif // CRYPTO_MODEL_RSA_RSA_HPP
