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

namespace s21 {
class Enigma {
public:
    using size_type = std::size_t;

private:
    using file_t = tools::filesystem::file_t;

public:
    Enigma() : Enigma(1) {}

    explicit Enigma(int num_rotors) {
        std::vector<Rotor> tmp_rotors(num_rotors);
        rotors_ = std::move(tmp_rotors);
        config_ = Config(reflector_, rotors_);
    }

    explicit Enigma(std::string_view path) {
        LoadConfig(path);
    }

    ~Enigma() = default;

public:
    void Encrypt(std::string_view path) {
        ResetConfig();
        
        auto file{fsm_.read_file(path)};
        if (!file.empty()) {
            size_type size{file.size()};
            std::vector<char> encoded(size);
            int num_rotors{static_cast<int>(rotors_.size())};

            for (size_type i{}; i < size; i++) {
                int code{static_cast<int>(file[i])};

                if (code < 0 || code > 127) {
                    encoded[i] = file[i];
                    continue;
                }

                for (int i{}; i < num_rotors; i++)
                    code = rotors_[i][code];

                code = reflector_[code];

                for (int i{num_rotors - 1}; i >= 0; i--) {
                    code = rotors_[i].Find(code);
                    rotors_[i].Shift();
                }

                encoded[i] = static_cast<char>(code);
            }

            SaveFile(path, encoded);
        }
    }

    void SaveConfig(std::string_view dir) const {
        fs::path fs_path(dir);

        if (!fs::is_directory(fs_path))
            fs_path = fs_path.remove_filename();

        fs_path /= "enigma_config.cfg";

        std::ofstream file(fs_path, std::ios::out);

        if (!file.is_open()) {
            std::string error_text{"Error: Cannot create file: "};
            std::string filename{fs_path.filename().generic_string()};
            throw std::ios_base::failure(error_text + filename);
        }

        for (const auto& rotor : rotors_) {
            for (std::size_t i{}; i < alphabet_size_; ++i)
                file << static_cast<int>(rotor[i]) << ' ';

            file << '\n';
        }
    }

private:
    void SaveFile(std::string_view path, const std::vector<char>& cipher) const {
        std::string postfix;
        auto encoded_pos{path.rfind("_encoded")};
        auto decoded_pos{path.rfind("_decoded")};
        if (encoded_pos == std::string::npos)
            postfix = "_encoded";
        else if (decoded_pos == std::string::npos)
            postfix = "_decoded";
        else if (encoded_pos < decoded_pos)
            postfix = "_encoded";
        else if (encoded_pos > decoded_pos)
            postfix = "_decoded";

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
            std::vector<char> tmp_cfg(alphabet_size_);
            int index{};
            int code{};

            while (substring_stream >> code) {
                tmp_cfg[index] = static_cast<char>(code);
                ++index;
            }

            Rotor rotor(tmp_cfg);
            rotors_.push_back(rotor);
        }

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
    Reflector reflector_;
    std::vector<Rotor> rotors_;
    tools::filesystem::monitoring fsm_;
    static constexpr const size_type alphabet_size_{128};
};
} // namespace s21

#endif // CRYPTO_MODEL_ENIGMA_ENIGMA_HPP
