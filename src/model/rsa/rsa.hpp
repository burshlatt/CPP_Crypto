#ifndef CRYPTO_MODEL_RSA_RSA_HPP
#define CRYPTO_MODEL_RSA_RSA_HPP

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

        tools::random::generator<int> r_generator(0, std::numeric_limits<int>::max());

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

private:
    std::pair<int, int> GetPublicKeys(int num_a, int num_b) const {
        int key_b{num_a * num_b};

        int euler_function{(num_a - 1) * (num_b - 1)};

        tools::random::generator<int> new_r_generator(0, euler_function);

        int key_a{new_r_generator.get_random_value()};

        while (!IsPrime(key_a) && GetGCD(key_a, euler_function) != 1)
            key_a = new_r_generator.get_random_value();

        return std::make_pair(key_a, key_b);
    }

    std::pair<int, int> GetPrivateKeys(int num_a, int num_b, int public_key_a) const {
        int key_b{num_a * num_b};

        int euler_function{(num_a - 1) * (num_b - 1)};

        auto [x, y]{EuclideanAlgorithm(euler_function, public_key_a)};

        int key_a{euler_function - std::abs(std::min(x, y))};

        return std::make_pair(key_a, key_b);
    }

    bool IsPrime(int number) const {
        if (number <= 1)
            return false;

        if (number <= 3)
            return true;

        if (number % 2 == 0 || number % 3 == 0)
            return false;

        for (int i = 5; i * i <= number; i += 6)
            if (number % i == 0 || number % (i + 2) == 0)
                return false;

        return true;
    }

    int GetGCD(int num_a, int num_b) const {
        while (num_b != 0) {
            int tmp{num_b};
            num_b = num_a % num_b;
            num_a = tmp;
        }
        return num_a;
    }

    std::pair<int, int> EuclideanAlgorithm(int num_a, int num_b) const {
        if (num_b == 0)
            return std::make_pair(1, 0);

        std::pair<int, int> prev{EuclideanAlgorithm(num_b, num_a % num_b)};
        int x{prev.second};
        int y{prev.first - (num_a / num_b) * prev.second};

        return std::make_pair(x, y);
    }

private:
    tools::filesystem::monitoring fsm_;
};
}  // namespace s21

#endif // CRYPTO_MODEL_RSA_RSA_HPP
