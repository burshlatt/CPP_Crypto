#ifndef CRYPTO_MODEL_RSA_RSA_HPP
#define CRYPTO_MODEL_RSA_RSA_HPP

#include <memory>
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
        fs::path dir_fs(dir);

        static constexpr const int range_min{100};
        static constexpr const int range_max{10000};

        tools::random::generator_int<int64_t> r_generator(range_min, range_max);

        int64_t p{r_generator.get_random_value()};
        int64_t q{r_generator.get_random_value()};

        while (!IsPrime(p))
            p = r_generator.get_random_value();

        while (!IsPrime(q) || q == p)
            q = r_generator.get_random_value();

        int64_t n{p * q};
        int64_t phi{(p - 1) * (q - 1)};

        tools::random::generator_int<int64_t> new_r_generator(2, phi - 1);
        int64_t e{new_r_generator.get_random_value()};

        while (GCD(e, phi) != 1)
            e = new_r_generator.get_random_value();

        int64_t d{};

        while ((e * d) % phi != 1)
            d++;

        std::string public_key{std::to_string(e) + " " + std::to_string(n)};
        std::string private_key{std::to_string(d) + " " + std::to_string(n)};

        file_t public_file(dir_fs / "public_key", public_key);
        file_t private_file(dir_fs / "private_key", private_key);

        fsm_.create_file(public_file);
        fsm_.create_file(private_file);
    }

    void Encode(std::string_view file_path, std::string_view key_path) {
        auto file{fsm_.read_file(fs::path(file_path))};
        std::size_t size{file.size()};

        std::ifstream key(fs::path(key_path), std::ios::in);

        if (key.is_open()) {
            int64_t k_a{};
            int64_t k_b{};

            key >> k_a >> k_b;

            std::string filename(file_path);
            auto pos{filename.find_last_of(".")};
            if (pos != std::string_view::npos)
                filename.insert(pos, "_encoded");
            else
                filename += "_encoded";

            std::ofstream encoded_file(fs::path(filename), std::ios::out);

            for (std::size_t i{}; i < size; ++i)
                encoded_file << EncryptBaseCode(static_cast<int64_t>(file[i]), k_a, k_b) << ' ';
        }
    }

    void Decode(std::string_view file_path, std::string_view key_path) {
        std::ifstream file(fs::path(file_path), std::ios::in);
        std::ifstream key(fs::path(key_path), std::ios::in);

        if (key.is_open() && file.is_open()) {
            int64_t tmp{};
            int64_t k_a{};
            int64_t k_b{};

            key >> k_a >> k_b;

            std::string filename(file_path);
            auto pos{filename.find_last_of(".")};
            if (pos != std::string_view::npos)
                filename.insert(pos, "_decoded");
            else
                filename += "_decoded";

            std::ofstream decoded_file(fs::path(filename), std::ios::out);

            while (file >> tmp)
                decoded_file << static_cast<char>(EncryptBaseCode(tmp, k_a, k_b));
        }
    }

private:
    bool IsPrime(int64_t n) {
        if (n <= 1)
            return false;

        if (n <= 3)
            return true;

        if (n % 2 == 0 || n % 3 == 0)
            return false;

        for (int64_t i = 5; i * i <= n; i += 6)
            if (n % i == 0 || n % (i + 2) == 0)
                return false;

        return true;
    }

    int64_t GCD(int64_t num_a, int64_t num_b) const {
        while (num_b != 0) {
            int64_t tmp{num_b};
            num_b = num_a % num_b;
            num_a = tmp;
        }

        return num_a;
    }

    int64_t EncryptBaseCode(int64_t base, int64_t exponent, int64_t modulus) {
        int64_t result = 1;

        while (exponent > 0) {
            if (exponent % 2 == 1)
                result = (result * base) % modulus;

            base = (base * base) % modulus;
            exponent /= 2;
        }

        return result;
    }

private:
    tools::filesystem::monitoring fsm_;
};
}  // namespace s21

#endif // CRYPTO_MODEL_RSA_RSA_HPP
