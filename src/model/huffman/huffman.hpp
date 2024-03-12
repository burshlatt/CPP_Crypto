#ifndef CRYPTO_MODEL_HUFFMAN_HUFFMAN_HPP
#define CRYPTO_MODEL_HUFFMAN_HUFFMAN_HPP

#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "tools.hpp"

namespace s21 {
class Huffman {
private:
    using file_t = tools::filesystem::file_t;

private:
    struct Node {
    public:
        Node() :
            value('\0'),
            frequency(0),
            left(nullptr),
            right(nullptr)
        {}

        explicit Node(char val, int freq) :
            value(val),
            frequency(freq),
            left(nullptr),
            right(nullptr)
        {}

        explicit Node(char val, int freq, Node* l_child, Node* r_child) :
            value(val),
            frequency(freq),
            left(l_child),
            right(r_child)
        {}

        ~Node() = default;

        char value{};
        int frequency{};
        Node* left{nullptr};
        Node* right{nullptr};
    };

    struct Comp {
        bool operator()(Node* left, Node* right) {
            return left->frequency > right->frequency;
        }
    };

    enum class Mode : bool { kEncode, kDecode };

public:
    Huffman() = default;

    ~Huffman() {
        Clear(root_);
    }

public:
    void Encode(std::string_view path) {
        ResetState();

        root_ = CreateTree(path);

        CalculateHuffmanCodes(root_, "");

        for (auto let : decoded_text_)
            encoded_text_ += huffman_codes_[let];

        SaveFile(path, Mode::kEncode);
        SaveConfig(path);
    }

    void Decode(std::string_view path_file, std::string_view path_config) {
        if (fs::path(path_config).extension() != ".cfg")
            throw std::invalid_argument("The configuration file does not have a .cfg extension");

        ResetState();

        root_ = LoadConfig(path_config);

        auto file{fsm_.read_file(path_file)};
        encoded_text_ = file.get_text();

        int index{-1};
        int size{static_cast<int>(encoded_text_.size()) - 2};
        while (index < size)
            DecryptText(root_, encoded_text_, index);

        SaveFile(path_file, Mode::kDecode);
    }

private:
    Node* CreateTree(std::string_view path) {
        auto file{fsm_.read_file(fs::path(path))};
        decoded_text_ = file.get_text();
        std::unordered_map<char, int> frequency;

        for (char ch : decoded_text_)
            frequency[ch]++;

        std::priority_queue<Node*, std::vector<Node*>, Comp> queue;

        for (auto [value, num] : frequency)
            queue.push(new Node(value, num));

        while (queue.size() != 1) {
            Node *left = queue.top();
            queue.pop();

            Node *right = queue.top();
            queue.pop();

            int sum = left->frequency + right->frequency;
            queue.push(new Node('\0', sum, left, right));
        }

        return queue.top();
    }

    void CalculateHuffmanCodes(Node* node, std::string str) {
        if (!node)
            return;

        if (!node->left && !node->right) {
            huffman_codes_[node->value] = str;
            return;
        }

        CalculateHuffmanCodes(node->left, str + "0");
        CalculateHuffmanCodes(node->right, str + "1");
    }

    void DecryptText(Node* node, std::string_view str, int &index) {
        if (!node)
            return;

        if (!node->left && !node->right) {
            decoded_text_ += node->value;
            return;
        }

        index++;

        if (str[index] == '0')
            DecryptText(node->left, str, index);
        else
            DecryptText(node->right, str, index);
    }

private:
    void SaveFile(std::string_view path, Mode mode) {
        std::string postfix{mode == Mode::kEncode ? "_encoded" : "_decoded"};
        std::string filename(path);

        auto pos{filename.find_last_of(".")};
        if (pos != std::string_view::npos)
            filename.insert(pos, postfix);
        else
            filename += postfix;

        if (mode == Mode::kEncode)
            fsm_.create_file(file_t(fs::path(filename), encoded_text_));
        else
            fsm_.create_file(file_t(fs::path(filename), decoded_text_));
    }

    void SaveConfig(std::string_view path) {
        std::string filename(path);
        auto pos{filename.find_last_of(".")};
        if (pos != std::string_view::npos)
            filename.insert(pos, "_encoded_huffman");
        else
            filename += "_encoded_huffman";

        std::ostringstream string_stream;

        for (const auto& [value, code] : huffman_codes_)
            string_stream << static_cast<int>(value) << ' ' << code << '\n';

        fs::path config_path{fs::path(filename).replace_extension("cfg")};

        fsm_.create_file(file_t(config_path, string_stream.str()));
    }

    Node* LoadConfig(std::string_view path) {
        std::ifstream file(fs::path(path), std::ios::in);

        if (!file.is_open())
            return nullptr;

        int symbol_code{};
        std::string code;

        Node* root_node{new Node()};

        while (file >> symbol_code >> code) {
            Node* current{root_node};
            for (auto var : code) {
                if (var == '0') {
                    if (!current->left)
                        current->left = new Node();

                    current = current->left;
                } else if (var == '1') {
                    if (!current->right)
                        current->right = new Node();

                    current = current->right;
                } else {
                    return nullptr;
                }
            }

            if (current && !current->left && !current->right)
                current->value = static_cast<char>(symbol_code);
        }

        return root_node;
    }

private:
    void Clear(Node* node) {
        if (node) {
            Clear(node->left);
            Clear(node->right);

            delete node;
            node = nullptr;
        }
    }

    void ResetState() {
        Clear(root_);

        decoded_text_ = std::string();
        encoded_text_ = std::string();
        huffman_codes_.clear();
    }

private:
    Node* root_{nullptr};

    std::string decoded_text_;
    std::string encoded_text_;
    std::unordered_map<char, std::string> huffman_codes_;

    tools::filesystem::monitoring fsm_;
};
} // namespace s21

#endif // CRYPTO_MODEL_HUFFMAN_HUFFMAN_HPP
