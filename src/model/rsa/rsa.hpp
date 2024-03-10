#ifndef CRYPTO_MODEL_RSA_RSA_HPP
#define CRYPTO_MODEL_RSA_RSA_HPP

#include <vector>
#include <memory>
#include <limits>
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

        tools::random::generator_int<int> r_generator(0, std::numeric_limits<int>::max());

        int num_a{r_generator.get_random_value()};
        int num_b{r_generator.get_random_value()};

        while (!IsPrime(num_a))
            num_a = r_generator.get_random_value();

        while (!IsPrime(num_b))
            num_b = r_generator.get_random_value();

        auto [public_a, public_b]{GetPublicKeys(num_a, num_b)};
        auto [private_a, private_b]{GetPrivateKeys(num_a, num_b, public_a)};

        std::string public_key{std::to_string(public_a) + " " + std::to_string(public_b)};
        std::string private_key{std::to_string(private_a) + " " + std::to_string(private_b)};

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
            int64_t p_a{};
            int64_t p_b{};

            key >> p_a >> p_b;

            std::ofstream encoded_file(fs::path(file_path).replace_filename("test_encoded.txt"), std::ios::out);

            for (std::size_t i{}; i < size; ++i)
                encoded_file << EncryptBaseCode(static_cast<int64_t>(file[i]), p_a, p_b) << ' ';
        }
    }

    void Decode(std::string_view file_path, std::string_view key_path) {
        std::ifstream file(fs::path(file_path), std::ios::in);
        std::ifstream key(fs::path(key_path), std::ios::in);

        if (key.is_open() && file.is_open()) {
            int64_t tmp{};
            int64_t p_a{};
            int64_t p_b{};

            key >> p_a >> p_b;

            std::ofstream decoded_file(fs::path(file_path).replace_filename("test_decoded.txt"), std::ios::out);

            while (file >> tmp)
                decoded_file << static_cast<char>(EncryptBaseCode(tmp, p_a, p_b));
        }
    }

private:
    std::pair<int64_t, int64_t> GetPublicKeys(int64_t num_a, int64_t num_b) const {
        int64_t key_b{num_a * num_b};
        int64_t euler_function{(num_a - 1) * (num_b - 1)};
        tools::random::generator_int<int64_t> new_r_generator(0, euler_function);
        int64_t key_a{new_r_generator.get_random_value()};

        while (!IsPrime(key_a) && GetGCD(key_a, euler_function) != 1)
            key_a = new_r_generator.get_random_value();

        return std::make_pair(key_a, key_b);
    }

    std::pair<int64_t, int64_t> GetPrivateKeys(int64_t num_a, int64_t num_b, int64_t public_key_a) const {
        int64_t key_b{num_a * num_b};
        int64_t euler_function{(num_a - 1) * (num_b - 1)};
        auto [x, y]{EuclideanAlgorithm(euler_function, public_key_a)};
        int64_t key_a{euler_function - std::abs(std::min(x, y))};
        return std::make_pair(key_a, key_b);
    }

    bool IsPrime(int64_t number) const {
        if (number <= 1)
            return false;

        if (number <= 3)
            return true;

        if (number % 2 == 0 || number % 3 == 0)
            return false;

        for (int64_t i = 5; i * i <= number; i += 6)
            if (number % i == 0 || number % (i + 2) == 0)
                return false;

        return true;
    }

    int64_t GetGCD(int64_t num_a, int64_t num_b) const {
        while (num_b != 0) {
            int64_t tmp{num_b};
            num_b = num_a % num_b;
            num_a = tmp;
        }

        return num_a;
    }

    std::pair<int64_t, int64_t> EuclideanAlgorithm(int64_t num_a, int64_t num_b) const {
        if (num_b == 0)
            return std::make_pair(1, 0);

        std::pair<int64_t, int64_t> prev{EuclideanAlgorithm(num_b, num_a % num_b)};
        int64_t x{prev.second};
        int64_t y{prev.first - (num_a / num_b) * prev.second};

        return std::make_pair(x, y);
    }

    int64_t EncryptBaseCode(int64_t base, int64_t exp, int64_t mod) {
        int64_t result = 1;

        base = base % mod;

        while (exp > 0) {
            if (exp % 2 == 1)
                result = (result * base) % mod;

            exp = exp >> 1;
            base = (base * base) % mod;
        }

        return result;
    }

private:
    tools::filesystem::monitoring fsm_;
};
}  // namespace s21

#endif // CRYPTO_MODEL_RSA_RSA_HPP
