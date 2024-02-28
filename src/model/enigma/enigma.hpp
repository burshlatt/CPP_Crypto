#ifndef CRYPTO_MODEL_ENIGMA_ENIGMA_HPP
#define CRYPTO_MODEL_ENIGMA_ENIGMA_HPP

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include <string_view>

#include "rotor.hpp"

namespace fs = std::filesystem;

namespace s21 {
class Enigma {
public:
    using size_type = std::size_t;

public:
    Enigma() = default;

    explicit Enigma(uint8_t num) : num_rotors_(num) {
        std::vector<uint8_t> tmp_reflector_(alphabet_size_);
        reflector_ = std::move(tmp_reflector_);
        
        std::vector<Rotor> tmp_rotors_(num_rotors_);
        rotors_ = std::move(tmp_rotors_);
    }

    ~Enigma() = default;

    // void SetReflector(const std::vector<uint8_t>& ref) {
    //     for (int i = 0; i < alphabet_size_; ++i)
    //         reflector_[i] = ref[i];
    // }

    // void SetRotor(uint8_t num, const std::vector<uint8_t>& rotor) {
    //     for (int i = 0; i < alphabet_size_; ++i)
    //         rotors_[num][i] = rotor[i];
    // }

    // Функция обычного сдвига массива на одну единицу вправо.
    void RotorShift(uint8_t num) {
        char temp = rotors_[num][alphabet_size_-1];
        for (int i = alphabet_size_-1; i > 0; --i) {
            rotors_[num][i] = rotors_[num][i-1];
        }
        rotors_[num][0] = temp;
    }

    // Функция обычного нахождения элемента в массиве.
    uint8_t RotorFind(uint8_t num, uint8_t code) {
        for (int i = 0; i < alphabet_size_; ++i)
            if (rotors_[num][i] == code)
                return i;

        return -1;
    }

    void Encode(std::string_view path) {
        // std::ifstream file(fs::path(path), std::ios::in);
        std::ifstream file(fs::path(path), std::ios::binary | std::ios::in);
        if (file.is_open()) {
            // std::string line;
            // while (file >> line) {}

            file.seekg(0, std::ios::end);
            std::size_t file_size = file.tellg();
            file.seekg(0, std::ios::beg);
            char *buffer = new char[file_size]{};
            file.read(buffer, file_size);

            char *cipher = new char[file_size]{};
            for (std::size_t i{}; i < file_size; ++i) {
                uint8_t new_code = buffer[i]; 
                // Проходим сквозь все роторы в сторону рефлектора, постоянно обновляя состояние символа.
                // Символ в такой концепции представляет собой индекс массива, по которому находим новый символ как значение массива.
                for (int i = 0; i < num_rotors_; ++i)
                    new_code = rotors_[i][new_code];

                // Получаем новое состояние символа от рефлектора.
                new_code = reflector_[new_code];

                // Теперь проходим сквозь все роторы, но уже от рефлектора, также постоянно обновляя состояние символа.
                // Символ в такой концепции представляет собой значение массива, по которому находим новый символ как индекс массива.
                for (int i = num_rotors_ - 1; i >= 0; --i) {
                    new_code = RotorFind(i, new_code);
                    if (new_code == -1)
                        return;
                }

                // После того, как прошли по роторам в сторону рефлектора и обратно
                // настало время обновить состояние самих роторов, их позицию.
                // Увеличиваем счётчик шифрованных символов и устанавливаем
                // по умолчанию очередь ротора = 1. 
                // Это говорит о том, что первый ротор должен обновлять своё состояние
                // каждый введённый символ.
                uint64_t rotor_queue = 1;
                counter_ += 1;
                for (int i = 0; i < num_rotors_; ++i) {
                    // Обновляем состояние ротора по его очереди.
                    if (counter_ % rotor_queue == 0)
                        RotorShift(i);
                    // Увеличиваем очередь каждого последующего ротора, т.к.
                    // поворот одного ротора должен производиться только после полного
                    // цикла предыдущего ротора.
                    rotor_queue *= alphabet_size_;
                }

                cipher[i] = new_code;
            }

            delete buffer;
            buffer = nullptr;

            SaveFile(path, cipher);

            delete cipher;
            cipher = nullptr;
        }
        file.close();
    }

private:
    void SaveFile(std::string_view path, const char* cipher) {
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
};
} // namespace s21

#endif // CRYPTO_MODEL_ENIGMA_ENIGMA_HPP