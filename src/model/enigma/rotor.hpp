#ifndef CRYPTO_MODEL_ENIGMA_ROTOR_HPP
#define CRYPTO_MODEL_ENIGMA_ROTOR_HPP

#include <vector>
#include <memory>

// #include "tools.hpp"
#include "../../third_party/tools/src/tools.hpp"

namespace s21 {
class Rotor {
public:
    using size_type = std::size_t;

public:
    Rotor() {
        tools::random::shuffle(data_.begin(), data_.end());
    }

    ~Rotor() = default;

public:
    char& operator[](int index) {
        return data_[index];
    }

    char operator[](int index) const {
        return data_[index];
    }

    [[nodiscard]] size_type Size() const noexcept {
        return size_;
    }

public:
    void Shift() {
        char tmp{data_[size_ - 1]};

        for (size_type i{size_ - 1}; i > 0; i--)
            data_[i] = data_[i - 1];

        data_[0] = tmp;
    }

    char Find(char code) {
        for (size_type i{}; i < size_; i++)
            if (data_[i] == code)
                return i;

        throw std::invalid_argument("Incorrect code: " + std::to_string(code));
    }

public:
    // void LoadConfig() {

    // }

private:
    std::vector<char> data_ {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
        31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
        51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
        71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
        91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
        101, 102, 103, 104, 105, 106, 107, 108, 109,
        110, 111, 112, 113, 114, 115, 116, 117, 118,
        119, 120, 121, 122, 123, 124, 125, 126, 127
    };
    static constexpr const size_type size_{128};
};
} // namespace s21

#endif // CRYPTO_MODEL_ENIGMA_ROTOR_HPP
