#ifndef CRYPTO_MODEL_ENIGMA_ENIGMA_HPP
#define CRYPTO_MODEL_ENIGMA_ENIGMA_HPP

#include <string>
#include <vector>
#include <memory>
#include <string_view>

#include "rotor.hpp"
#include "reflector.hpp"

// #include "tools.hpp"
#include "../../third_party/tools/src/tools.hpp"

namespace fs = std::filesystem;

namespace s21 {
class Enigma {
public:
    using size_type = std::size_t;

private:
    using file_t = tools::filesystem::file_t;

public:
    Enigma() : Enigma(1) {}

    explicit Enigma(uint8_t num_rotors) {
        std::vector<Rotor> tmp_rotors(num_rotors);
        rotors_ = std::move(tmp_rotors);

        configs_ = Configs(counter_, reflector_, rotors_);
    }

    ~Enigma() = default;

public:
    void EncryptFile(std::string_view path) {
        ResetConfig();
        auto file{fsm_.read_file(path)};
        if (!file.empty()) {
            size_type size{file.size()};
            std::vector<char> encoded(size);

            for (size_type i{}; i < size; i++)
                encoded[i] = Encrypt(file[i]);

            SaveFile(path, "_encoded", encoded);
        }
    }

private:
    char Encrypt(char code) {
        char new_code{code};

        int num_rotors{static_cast<int>(rotors_.size())};

        for (int i{}; i < num_rotors; i++)
            new_code = rotors_[i][new_code];

        new_code = reflector_[new_code];

        for (int i{num_rotors - 1}; i >= 0; i--)
            new_code = rotors_[i].Find(new_code);

        ++counter_;
        uint64_t rotor_queue{1};
        for (int i{}; i < num_rotors; i++) {
            if (counter_ % rotor_queue == 0)
                rotors_[i].Shift();

            rotor_queue *= alphabet_size_;
        }

        return new_code;
    }

private:
    void SaveFile(std::string_view path, std::string_view postfix, const std::vector<char>& cipher) {
        std::string filename(path);
        auto pos{filename.find_last_of(".")};
        if (pos != std::string_view::npos)
            filename.insert(pos, postfix);
        else
            filename += postfix;

        fs::path full_path(filename);
        size_type size{cipher.size()};

        fsm_.create_file(file_t(full_path, cipher.data(), size));
    }

private:
    void ResetConfig() {
        counter_ = configs_.counter_conf;
        reflector_ = configs_.reflector_conf;
        rotors_ = configs_.rotors_conf;
    }

    // void LoadConfig() {
        
    // }

private:
    struct Configs {
        Configs() = default;

        Configs(uint64_t c, Reflector ref, const std::vector<Rotor>& rot) :
            counter_conf(c),
            reflector_conf(ref),
            rotors_conf(rot)
        {}

        ~Configs() = default;

        uint64_t counter_conf{};
        Reflector reflector_conf;
        std::vector<Rotor> rotors_conf;
    };
    

private:
    Configs configs_;
    uint64_t counter_{};
    Reflector reflector_;
    std::vector<Rotor> rotors_;
    tools::filesystem::monitoring fsm_;
    static constexpr const size_type alphabet_size_{128};
};
} // namespace s21

#endif // CRYPTO_MODEL_ENIGMA_ENIGMA_HPP