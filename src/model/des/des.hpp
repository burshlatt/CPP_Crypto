#ifndef CRYPTO_MODEL_DES_DES_HPP
#define CRYPTO_MODEL_DES_DES_HPP

#include <vector>
#include <bitset>
#include <string_view>

#include "tables.hpp"

#include "tools.hpp"

namespace s21 {
class DES {
private:
    using file_t = tools::filesystem::file_t;

public:
    DES() = default;
    ~DES() = default;

public:
    void EncodeECB(std::string_view file_path, std::string_view key_path) {
        auto file{fsm_.read_file(fs::path(file_path))};
        auto key_file{fsm_.read_file(fs::path(key_path))};

        std::bitset<64> key{key_file.get_text()};
        std::string file_text{file.get_text()};
        std::size_t file_size{file.size()};
        std::vector<std::string> encrypted_blocks;

        for (std::size_t i{}; i < file_size; i += char_bits_size_) {
            std::bitset<64> block_bits;
            std::string block_str{file_text.substr(i, char_bits_size_)};

            for (std::size_t j{}; j < block_str.size(); ++j) {
                std::bitset<8> char_bits(block_str[j]);

                for (std::size_t k{}; k < char_bits.size(); ++k)
                    block_bits[j * 8 + k] = char_bits[k];
            }

            encrypted_blocks.push_back(EncryptBlock(block_bits, key));
        }

        std::string encrypted_text{std::accumulate(encrypted_blocks.begin(), encrypted_blocks.end(), std::string(""))};

        fsm_.create_file(file_t(GetNewFilePath(file_path, "_encoded"), encrypted_text));
    }

    void DecodeECB(std::string_view file_path, std::string_view key_path) {
        auto file{fsm_.read_file(fs::path(file_path))};
        auto key_file{fsm_.read_file(fs::path(key_path))};

        std::bitset<64> key{key_file.get_text()};
        std::string file_text{file.get_text()};
        std::size_t file_size{file.size()};

        std::vector<std::string> decrypted_blocks;

        for (std::size_t i{}; i < file_size; i += block_bits_size_) {
            std::string block_str{file_text.substr(i, block_bits_size_)};
            std::bitset<64> block_bits(block_str);

            decrypted_blocks.push_back(DecryptBlock(block_bits, key));
        }

        std::string decrypted_text{std::accumulate(decrypted_blocks.begin(), decrypted_blocks.end(), std::string(""))};

        fsm_.create_file(file_t(GetNewFilePath(file_path, "_decoded"), decrypted_text));
    }

private:
    std::string EncryptBlock(const std::bitset<64>& block, const std::bitset<64>& key) {
        // Начальная перестановка
        std::bitset<64> cipher_text{Permutation(block, tbl::initial_permutation_table, 64)};

        // Генерация подключей
        std::vector<std::bitset<48>> round_keys{GenerateRoundKeys(key)};

        // 16 раундов шифрования
        std::bitset<32> left_half(cipher_text.to_string().substr(0, 32));
        std::bitset<32> right_half(cipher_text.to_string().substr(32, 32));

        for (int i = 0; i < 16; ++i) {
            std::bitset<32> new_right_half{left_half ^ Round(right_half, round_keys[i])};
            left_half = right_half;
            right_half = new_right_half;
        }

        // Финальная перестановка
        std::bitset<64> encrypted_text{Permutation(cipher_text, tbl::final_permutation_table, 64)};

        return encrypted_text.to_string();
    }

    std::string DecryptBlock(const std::bitset<64>& block, const std::bitset<64>& key) {
        // Начальная перестановка
        std::bitset<64> decrypted_text{Permutation(block, tbl::initial_permutation_table, 64)};

        // Генерация подключей
        std::vector<std::bitset<48>> round_keys{GenerateRoundKeys(key)};

        // 16 раундов дешифрования
        std::bitset<32> left_half(decrypted_text.to_string().substr(0, 32));
        std::bitset<32> right_half(decrypted_text.to_string().substr(32, 32));

        for (int i = 15; i >= 0; --i) { // идем в обратном порядке по ключам
            std::bitset<32> new_right_half{left_half ^ Round(right_half, round_keys[i])};
            left_half = right_half;
            right_half = new_right_half;
        }

        // Финальная перестановка
        std::bitset<64> decrypted_plain_text{Permutation(decrypted_text, tbl::final_permutation_table, 64)};

        // Преобразование битов в символьную строку
        std::string decrypted_string;

        for (size_t i = 0; i < char_bits_size_; ++i) {
            std::bitset<8> byte;

            for (size_t j = 0; j < char_bits_size_; ++j)
                byte[j] = decrypted_plain_text[i * char_bits_size_ + j];

            if (!byte.none())
                decrypted_string += static_cast<char>(byte.to_ulong());
        }

        return decrypted_string;
    }

    // Функция для выполнения начальной и финальной перестановки
    std::bitset<64> Permutation(const std::bitset<64>& input, const int* table, int table_size) {
        std::bitset<64> permuted_text;

        for (int i = 0; i < table_size; ++i)
            permuted_text[i] = input[table[i] - 1];

        return permuted_text;
    }

    // Функция для выполнения раунда шифрования DES
    std::bitset<32> Round(const std::bitset<32>& input, const std::bitset<48>& round_key) {
        // Расширяем входную половинку до 48 битов
        std::bitset<48> expanded_input;

        for (int i = 0; i < 48; ++i)
            expanded_input[i] = input[tbl::expansion_table[i] - 1];

        // Применяем операцию XOR с раундовым ключом
        expanded_input ^= round_key;
        
        // Разбиваем на блоки по 6 бит
        std::vector<std::bitset<6>> input_blocks;

        for (int i = 0; i < 48; i += 6)
            input_blocks.push_back(expanded_input.to_ulong() >> (48 - (i + 6)));
        
        // Применяем S-блоки
        std::bitset<32> output;
        for (int i = 0; i < 8; ++i) {
            int row = (input_blocks[i][0] << 1) + input_blocks[i][5];
            int col = (input_blocks[i].to_ulong() >> 1) & 0x0F;
            int val = tbl::s_box[i][row][col];
            output |= (std::bitset<32>(val) << (28 - (i * 4)));
        }
        
        // Применяем перестановку P
        std::bitset<32> permuted_output;

        for (int i = 0; i < 32; ++i)
            permuted_output[i] = output[tbl::permutation_table[i] - 1];
        
        return permuted_output;
    }

    // Функция для генерации подключей DES
    std::vector<std::bitset<48>> GenerateRoundKeys(const std::bitset<64>& main_key) {
        std::vector<std::bitset<48>> round_keys(16);
        std::bitset<56> key_permuted;

        for (int i = 0; i < 56; ++i)
            key_permuted[i] = main_key[tbl::key_permutation_table[i] - 1];

        std::bitset<28> left_key_half, right_key_half;

        for (int i = 0; i < 28; ++i) {
            left_key_half[i] = key_permuted[i];
            right_key_half[i] = key_permuted[i + 28];
        }

        for (int i = 0; i < 16; ++i) {
            // Сдвиги
            left_key_half = (left_key_half << tbl::key_shift_table[i]) | (left_key_half >> (28 - tbl::key_shift_table[i]));
            right_key_half = (right_key_half << tbl::key_shift_table[i]) | (right_key_half >> (28 - tbl::key_shift_table[i]));
            
            // Комбинирование ключей
            std::bitset<56> combined_key_half;

            for (int j = 0; j < 28; ++j) {
                combined_key_half[j] = left_key_half[j];
                combined_key_half[j + 28] = right_key_half[j];
            }
            
            // Сжатие ключа
            std::bitset<48> compressed_key;

            for (int j = 0; j < 48; ++j)
                compressed_key[j] = combined_key_half[tbl::key_compression_table[j] - 1];
            
            round_keys[i] = compressed_key;
        }

        return round_keys;
    }

private:
    fs::path GetNewFilePath(std::string_view path, std::string_view postfix) {
        std::string filename(path);
        auto pos{filename.find_last_of(".")};
        if (pos != std::string_view::npos)
            filename.insert(pos, postfix);
        else
            filename += postfix;
        
        return fs::path(filename);
    }

private:
    static constexpr const std::size_t char_bits_size_{8};
    static constexpr const std::size_t block_bits_size_{64};

    tools::filesystem::monitoring fsm_;
};
} // namespace s21

#endif // CRYPTO_MODEL_DES_DES_HPP
