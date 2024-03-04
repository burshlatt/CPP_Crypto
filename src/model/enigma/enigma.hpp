#ifndef CRYPTO_MODEL_ENIGMA_ENIGMA_HPP
#define CRYPTO_MODEL_ENIGMA_ENIGMA_HPP

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <string_view>

#include "tools.hpp"

#include "rotor.hpp"
#include "reflector.hpp"

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
        num_rotors_ = static_cast<int>(rotors_.size());
        config_ = Config(reflector_, rotors_);
    }

    explicit Enigma(std::string_view path) {
        LoadConfig(path);
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

            auto encoded_pos{path.rfind("_encoded")};
            auto decoded_pos{path.rfind("_decoded")};
            if (encoded_pos == std::string::npos)
                SaveFile(path, "_encoded", encoded);
            else if (decoded_pos == std::string::npos)
                SaveFile(path, "_decoded", encoded);
            else if (encoded_pos < decoded_pos)
                SaveFile(path, "_encoded", encoded);
            else if (encoded_pos > decoded_pos)
                SaveFile(path, "_decoded", encoded);
        }
    }

private:
    char Encrypt(char code) {
        for (int i{}; i < num_rotors_; i++)
            code = rotors_[i][code];

        code = reflector_[code];

        for (int i{num_rotors_ - 1}; i >= 0; i--)
            code = rotors_[i].Find(code);

        ++counter_;
        uint64_t rotor_queue{1};
        for (int i{}; i < num_rotors_; i++) {
            if (counter_ % rotor_queue == 0)
                rotors_[i].Shift();

            rotor_queue *= alphabet_size_;
        }

        return code;
    }

private:
    void SaveFile(std::string_view path, std::string postfix, const std::vector<char>& cipher) {
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
        counter_ = uint64_t();
        reflector_ = config_.reflector_conf;
        rotors_ = config_.rotors_conf;
    }

    void LoadConfig(std::string_view path) {
        fs::path fs_path(path);
        std::ifstream file(fs_path, std::ios::in);

        if (!file.is_open()) {
            std::string error_text{"Error: Cannot open file: "};
            std::string filename{fs_path.filename().generic_string()};
            throw std::ios_base::failure(error_text + filename);
        }

        std::string line;

        while (std::getline(file, line)) {
            std::stringstream substring_stream(line);
            int i{};
            int code{};
            std::vector<char> tmp_cfg(128);

            while (substring_stream >> code) {
                tmp_cfg[i] = static_cast<char>(code);
                ++i;
            }

            Rotor rotor(tmp_cfg);
            rotors_.push_back(rotor);
        }

        num_rotors_ = static_cast<int>(rotors_.size());
        config_ = Config(reflector_, rotors_);
    }

private:
    struct Config {
        Config() = default;

        Config(const Reflector& refl, const std::vector<Rotor>& rot) :
            reflector_conf(refl),
            rotors_conf(rot)
        {}

        ~Config() = default;

        Reflector reflector_conf;
        std::vector<Rotor> rotors_conf;
    };
    
private:
    Config config_;

    int num_rotors_{};
    uint64_t counter_{};

    Reflector reflector_;
    std::vector<Rotor> rotors_;
    tools::filesystem::monitoring fsm_;
    static constexpr const size_type alphabet_size_{128};
};
} // namespace s21

#endif // CRYPTO_MODEL_ENIGMA_ENIGMA_HPP