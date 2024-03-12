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

        std::bitset<key_bits_size_> key{key_file.get_text()};
        std::string file_text{file.get_text()};
        std::size_t file_size{file.size()};
        std::vector<std::string> encrypted_blocks;

        for (std::size_t i{}; i < file_size; i += char_bits_size_) {
            std::bitset<block_bits_size_> block_bits;
            std::string block_str{file_text.substr(i, char_bits_size_)};

            for (std::size_t j{}; j < block_str.size(); ++j) {
                std::bitset<char_bits_size_> char_bits(block_str[j]);

                for (std::size_t k{}; k < char_bits.size(); ++k)
                    block_bits[j * char_bits_size_ + k] = char_bits[k];
            }

            encrypted_blocks.push_back(EncryptBlock(block_bits, key));
        }

        std::string encrypted_text{std::accumulate(encrypted_blocks.begin(), encrypted_blocks.end(), std::string(""))};

        fsm_.create_file(file_t(GetNewFilePath(file_path, "_encoded"), encrypted_text));
    }

    void DecodeECB(std::string_view file_path, std::string_view key_path) {
        auto file{fsm_.read_file(fs::path(file_path))};
        auto key_file{fsm_.read_file(fs::path(key_path))};

        std::bitset<key_bits_size_> key{key_file.get_text()};
        std::string file_text{file.get_text()};
        std::size_t file_size{file.size()};

        std::vector<std::string> decrypted_blocks;

        for (std::size_t i{}; i < file_size; i += block_bits_size_) {
            std::string block_str{file_text.substr(i, block_bits_size_)};
            std::bitset<block_bits_size_> block_bits(block_str);

            decrypted_blocks.push_back(DecryptBlock(block_bits, key));
        }

        std::string decrypted_text{std::accumulate(decrypted_blocks.begin(), decrypted_blocks.end(), std::string(""))};

        fsm_.create_file(file_t(GetNewFilePath(file_path, "_decoded"), decrypted_text));
    }

private:
    std::string EncryptBlock(const std::bitset<64>& block, const std::bitset<64>& key) {
        std::bitset<block_bits_size_> cipher_text{Permutation(block, tbl::initial_permutation_table, 64)};

        std::vector<std::bitset<subkey_bits_size_>> round_keys{GenerateRoundKeys(key)};

        std::bitset<half_bits_size_> left_half(cipher_text.to_string().substr(0, half_bits_size_));
        std::bitset<half_bits_size_> right_half(cipher_text.to_string().substr(half_bits_size_, half_bits_size_));

        for (std::size_t i = 0; i < num_rounds_; ++i) {
            std::bitset<half_bits_size_> new_right_half{left_half ^ Round(right_half, round_keys[i])};
            left_half = right_half;
            right_half = new_right_half;
        }

        std::bitset<block_bits_size_> encrypted_text{Permutation(cipher_text, tbl::final_permutation_table, 64)};

        return encrypted_text.to_string();
    }

    std::string DecryptBlock(const std::bitset<64>& block, const std::bitset<64>& key) {
        std::bitset<block_bits_size_> decrypted_text{Permutation(block, tbl::initial_permutation_table, 64)};

        std::vector<std::bitset<subkey_bits_size_>> round_keys{GenerateRoundKeys(key)};

        std::bitset<half_bits_size_> left_half(decrypted_text.to_string().substr(0, half_bits_size_));
        std::bitset<half_bits_size_> right_half(decrypted_text.to_string().substr(half_bits_size_, half_bits_size_));

        for (int i = 15; i >= 0; --i) {
            std::bitset<half_bits_size_> new_right_half{left_half ^ Round(right_half, round_keys[i])};
            left_half = right_half;
            right_half = new_right_half;
        }

        std::bitset<block_bits_size_> decrypted_plain_text{Permutation(decrypted_text, tbl::final_permutation_table, 64)};

        std::string decrypted_string;

        for (size_t i = 0; i < char_bits_size_; ++i) {
            std::bitset<char_bits_size_> byte;

            for (size_t j = 0; j < char_bits_size_; ++j)
                byte[j] = decrypted_plain_text[i * char_bits_size_ + j];

            if (!byte.none())
                decrypted_string += static_cast<char>(byte.to_ulong());
        }

        return decrypted_string;
    }

    std::bitset<64> Permutation(const std::bitset<64>& input, const int* table, int table_size) {
        std::bitset<block_bits_size_> permuted_text;

        for (int i = 0; i < table_size; ++i)
            permuted_text[i] = input[table[i] - 1];

        return permuted_text;
    }

    std::bitset<32> Round(const std::bitset<32>& input, const std::bitset<48>& round_key) {
        std::bitset<subkey_bits_size_> expanded_input;

        for (std::size_t i = 0; i < subkey_bits_size_; ++i)
            expanded_input[i] = input[tbl::expansion_table[i] - 1];

        expanded_input ^= round_key;
        
        std::vector<std::bitset<miniblock_bits_size_>> input_blocks;

        for (std::size_t i = 0; i < subkey_bits_size_; i += miniblock_bits_size_)
            input_blocks.push_back(expanded_input.to_ulong() >> (subkey_bits_size_ - (i + miniblock_bits_size_)));
        
        std::bitset<half_bits_size_> output;
        for (std::size_t i = 0; i < char_bits_size_; ++i) {
            int row = (input_blocks[i][0] << 1) + input_blocks[i][5];
            int col = (input_blocks[i].to_ulong() >> 1) & 0x0F;
            int val = tbl::s_box[i][row][col];
            output |= (std::bitset<half_bits_size_>(val) << (key_half_bits_size_ - (i * 4)));
        }
        
        std::bitset<half_bits_size_> permuted_output;

        for (std::size_t i = 0; i < half_bits_size_; ++i)
            permuted_output[i] = output[tbl::permutation_table[i] - 1];
        
        return permuted_output;
    }

    std::vector<std::bitset<48>> GenerateRoundKeys(const std::bitset<64>& main_key) {
        std::vector<std::bitset<subkey_bits_size_>> round_keys(num_rounds_);
        std::bitset<round_key_bits_size_> key_permuted;

        for (std::size_t i = 0; i < round_key_bits_size_; ++i)
            key_permuted[i] = main_key[tbl::key_permutation_table[i] - 1];

        std::bitset<key_half_bits_size_> left_key_half, right_key_half;

        for (std::size_t i = 0; i < key_half_bits_size_; ++i) {
            left_key_half[i] = key_permuted[i];
            right_key_half[i] = key_permuted[i + key_half_bits_size_];
        }

        for (std::size_t i = 0; i < num_rounds_; ++i) {
            left_key_half = (left_key_half << tbl::key_shift_table[i]) | (left_key_half >> (key_half_bits_size_ - tbl::key_shift_table[i]));
            right_key_half = (right_key_half << tbl::key_shift_table[i]) | (right_key_half >> (key_half_bits_size_ - tbl::key_shift_table[i]));
            
            std::bitset<round_key_bits_size_> combined_key_half;

            for (std::size_t j = 0; j < key_half_bits_size_; ++j) {
                combined_key_half[j] = left_key_half[j];
                combined_key_half[j + key_half_bits_size_] = right_key_half[j];
            }
            
            std::bitset<subkey_bits_size_> compressed_key;

            for (std::size_t j = 0; j < subkey_bits_size_; ++j)
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
    static constexpr const std::size_t num_rounds_{16};
    static constexpr const std::size_t char_bits_size_{8};
    static constexpr const std::size_t key_bits_size_{64};
    static constexpr const std::size_t half_bits_size_{32};
    static constexpr const std::size_t block_bits_size_{64};
    static constexpr const std::size_t subkey_bits_size_{48};
    static constexpr const std::size_t miniblock_bits_size_{6};
    static constexpr const std::size_t key_half_bits_size_{28};
    static constexpr const std::size_t round_key_bits_size_{56};

    tools::filesystem::monitoring fsm_;
};
} // namespace s21

#endif // CRYPTO_MODEL_DES_DES_HPP
