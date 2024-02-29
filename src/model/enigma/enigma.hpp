#ifndef CRYPTO_MODEL_ENIGMA_ENIGMA_HPP
#define CRYPTO_MODEL_ENIGMA_ENIGMA_HPP

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include <string_view>

#include "rotor.hpp"

// #include "tools.hpp"
#include "../../third_party/tools/src/tools.hpp"

namespace fs = std::filesystem;

namespace s21 {
class Enigma {
public:
    using size_type = std::size_t;

public:
    Enigma() : Enigma(1) {}

    explicit Enigma(uint8_t num_rot) : num_rotors_(num_rot) {
        std::vector<uint8_t> tmp_reflector_(alphabet_size_);
        reflector_ = std::move(tmp_reflector_);
        
        std::vector<Rotor> tmp_rotors_(num_rotors_);
        rotors_ = std::move(tmp_rotors_);
    }

    ~Enigma() = default;

public:
    // void SetReflector(const std::vector<uint8_t>& ref) {
    //     for (int i = 0; i < alphabet_size_; ++i)
    //         reflector_[i] = ref[i];
    // }

    // void SetRotor(uint8_t num, const std::vector<uint8_t>& rotor) {
    //     for (int i = 0; i < alphabet_size_; ++i)
    //         rotors_[num][i] = rotor[i];
    // }

    void Encode(std::string_view path) {
        auto file{fsm_.read_file(path)};
        size_type size{file.size()};
        std::unique_ptr<uint8_t[]> cipher{std::make_unique<uint8_t[]>(size)};

        for (size_type i{}; i < size; ++i)
            cipher[i] = Encrypt(static_cast<uint8_t>(file[i]));

        // SaveCipherFile(path, cipher);
    }

private:
    uint8_t Encrypt(uint8_t code) {
        uint8_t new_code{code}; 

        for (int i{}; i < num_rotors_; ++i)
            new_code = rotors_[i][new_code];

        new_code = reflector_[new_code];

        for (int i{num_rotors_ - 1}; i >= 0; --i)
            new_code = rotors_[i].Find(new_code);

        counter_ += 1;
        uint64_t rotor_queue{1};
        for (int i{}; i < num_rotors_; ++i) {
            if (counter_ % rotor_queue == 0)
                rotors_[i].Shift();

            rotor_queue *= alphabet_size_;
        }

        return new_code;
    }

    void SaveCipherFile(std::string_view path, const char* cipher) {
        std::string filename(path);
        auto pos{filename.find_last_of(".")};
        if (pos != std::string_view::npos)
            filename = filename.substr(0, pos) + "_encoded" + filename.substr(pos);

        std::ofstream file(fs::path(filename), std::ios::out);

        if (file.is_open())
            file << cipher;

        file.close();
    }

private:
    uint64_t counter_{};
    uint8_t num_rotors_{};
    std::vector<Rotor> rotors_;

    std::vector<uint8_t> reflector_;
    
    static constexpr const size_type alphabet_size_{128};

    tools::filesystem::monitoring fsm_;
};
} // namespace s21

#endif // CRYPTO_MODEL_ENIGMA_ENIGMA_HPP