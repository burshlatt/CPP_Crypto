#ifndef CRYPTO_CONTROLLER_HUFFMAN_CONTROLLER_HPP
#define CRYPTO_CONTROLLER_HUFFMAN_CONTROLLER_HPP

#include <memory>
#include <string_view>

#include "huffman.hpp"

namespace s21 {
class HuffmanController {
public:
    HuffmanController() {
        huffman_ = std::make_unique<Huffman>();
    }

    ~HuffmanController() = default;

public:
    void Encrypt(std::string_view path) {
        huffman_->Encode(path);
    }

    void Decrypt(std::string_view path_file, std::string_view path_config) {
        huffman_->Decode(path_file, path_config);
    }
    
private:
    std::unique_ptr<Huffman> huffman_;
};
}  // namespace s21

#endif // CRYPTO_CONTROLLER_HUFFMAN_CONTROLLER_HPP
