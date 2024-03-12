#ifndef CRYPTO_MODEL_ENIGMA_REFLECTOR_HPP
#define CRYPTO_MODEL_ENIGMA_REFLECTOR_HPP

#include <vector>

#include "tools.hpp"

namespace s21 {
class Reflector {
public:
    using size_type = std::size_t;

public:
    Reflector() = default;
    ~Reflector() = default;

public:
    char& operator[](int index) {
        return data_[index];
    }

    char operator[](int index) const {
        return data_[index];
    }

private:
    std::vector<char> data_ {
        127, 126, 125, 124, 123, 122, 121, 120,
        119, 118, 117, 116, 115, 114, 113, 112,
        111, 110, 109, 108, 107, 106, 105, 104,
        103, 102, 101, 100, 99, 98, 97, 96, 95,
        94, 93, 92, 91, 90, 89, 88, 87, 86, 85,
        84, 83, 82, 81, 80, 79, 78, 77, 76, 75,
        74, 73, 72, 71, 70, 69, 68, 67, 66, 65,
        64, 63, 62, 61, 60, 59, 58, 57, 56, 55,
        54, 53, 52, 51, 50, 49, 48, 47, 46, 45,
        44, 43, 42, 41, 40, 39, 38, 37, 36, 35,
        34, 33, 32, 31, 30, 29, 28, 27, 26, 25,
        24, 23, 22, 21, 20, 19, 18, 17, 16, 15,
        14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3,
        2, 1, 0
    };
    static constexpr const size_type size_{128};
};
} // namespace s21

#endif // CRYPTO_MODEL_ENIGMA_REFLECTOR_HPP
