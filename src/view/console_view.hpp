#ifndef CRYPTO_VIEW_CONSOLE_VIEW_HPP
#define CRYPTO_VIEW_CONSOLE_VIEW_HPP

#include <map>
#include <memory>
#include <iostream>
#include <functional>

#include "tools.hpp"

#include "rsa_controller.hpp"
#include "des_controller.hpp"
#include "enigma_controller.hpp"
#include "huffman_controller.hpp"

namespace s21 {
class ConsoleView {
private:
    using mod   = tools::console::ansi::mods;
    using color = tools::console::ansi::colors;
    using menu  = std::map<int, std::function<void()>>;

public:
    ConsoleView() = default;
    ~ConsoleView() = default;

    void RunApp() {
        while (true) {
            tools::console::console_clear();
            ShowMenu();

            int opt{tools::console::get_correct_int()};

            if (!opt)
                break;

            if (menu_.find(opt) != menu_.end())
                menu_[opt]();
        }
    }

private:
    void RunEnigma() {
        while (true) {
            tools::console::console_clear();
            tools::console::print_text("ENIGMA:\n", color::green, mod::bold);
            tools::console::print_text("1.", color::green, mod::bold, " ");
            tools::console::print_text("Set a random configuration", color::blue);
            tools::console::print_text("2.", color::green, mod::bold, " ");
            tools::console::print_text("Read configuration file\n", color::blue);
            tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
            tools::console::print_text("Select menu item:", color::green, mod::bold, " ");

            int opt{tools::console::get_correct_int()};
            if (opt == 1) {
                tools::console::print_text("\nEnter the number of rotors:", color::green, mod::bold, " ");
                int num_rotors{tools::console::get_correct_int()};
                enigma_controller_ = std::make_unique<EnigmaController>(num_rotors);
            } else if (opt == 2) {
                enigma_controller_ = std::make_unique<EnigmaController>(fsm_.get_file_path());
            } else if (opt == 0) {
                break;
            }

            while (true) {
                tools::console::console_clear();
                tools::console::print_text("ENIGMA:\n", color::green, mod::bold);
                tools::console::print_text("1.", color::green, mod::bold, " ");
                tools::console::print_text("Select a file\n", color::blue);
                tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
                tools::console::print_text("Select menu item:", color::green, mod::bold, " ");

                opt = tools::console::get_correct_int();
                if (opt == 1)
                    enigma_controller_->Encrypt(fsm_.get_file_path());
                else if (opt == 0)
                    break;
            }
        }
    }

    void RunHuffman() {
        while (true) {
            tools::console::console_clear();
            tools::console::print_text("HUFFMAN:\n", color::green, mod::bold);
            tools::console::print_text("1.", color::green, mod::bold, " ");
            tools::console::print_text("Encrypt file", color::blue);
            tools::console::print_text("2.", color::green, mod::bold, " ");
            tools::console::print_text("Decrypt file", color::blue, "", "\n\n");
            tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
            tools::console::print_text("Select menu item:", color::green, mod::bold, " ");

            int opt{tools::console::get_correct_int()};
            if (opt == 1) {
                std::string file_path{fsm_.get_file_path()};

                if (!file_path.empty())
                    huffman_controller_.Encrypt(file_path);
                    
            } else if (opt == 2) {
                std::string file_path{"null"};
                std::string config_path{"null"};
                while (true) {
                    tools::console::console_clear();
                    tools::console::print_text("HUFFMAN:\n", color::green, mod::bold);
                    tools::console::print_text("1.", color::green, mod::bold, " ");
                    tools::console::print_text("Select encoded file\t(" + file_path + ")", color::blue);
                    tools::console::print_text("2.", color::green, mod::bold, " ");
                    tools::console::print_text("Select config file\t(" + config_path + ")\n", color::blue);
                    tools::console::print_text("3. CONFIRM", color::red, mod::bold);
                    tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
                    tools::console::print_text("Select menu item:", color::green, mod::bold, " ");

                    opt = tools::console::get_correct_int();
                    if (opt == 1)
                        file_path = fsm_.get_file_path();
                    else if (opt == 2)
                        config_path = fsm_.get_file_path();
                    else if (opt == 0 || opt == 3)
                        break;
                }

                if (opt != 0 && file_path != "null" && config_path != "null")
                    huffman_controller_.Decrypt(file_path, config_path);
            } else if (opt == 0) {
                break;
            }
        }
    }

    void RunRSA() {
        while (true) {
            tools::console::console_clear();
            tools::console::print_text("RSA:\n", color::green, mod::bold);
            tools::console::print_text("1.", color::green, mod::bold, " ");
            tools::console::print_text("Generate keys", color::blue);
            tools::console::print_text("2.", color::green, mod::bold, " ");
            tools::console::print_text("Encrypt file", color::blue);
            tools::console::print_text("3.", color::green, mod::bold, " ");
            tools::console::print_text("Decrypt file", color::blue, "", "\n\n");
            tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
            tools::console::print_text("Select menu item:", color::green, mod::bold, " ");

            int opt{tools::console::get_correct_int()};
            if (opt == 0) {
                break;
            } else if (opt == 1) {
                std::string dir{fsm_.get_file_path()};

                if (!dir.empty()) {
                    fs::path dir_fs(dir);

                    if (!fs::is_directory(dir_fs))
                        dir_fs = dir_fs.remove_filename();

                    rsa_controller_.GenerateKeys(dir_fs.generic_string());
                }
            } else if (opt == 2 || opt == 3) {
                std::string option{"ENCRYPT:"};
                if (opt == 3)
                    option = "DECRYPT:";

                std::string file_path{"null"};
                std::string key_path{"null"};
                int file_opt{};
                while (true) {
                    tools::console::console_clear();
                    tools::console::print_text("RSA", color::green, mod::bold, " ");
                    tools::console::print_text(option, color::blue, mod::bold, "\n\n");
                    tools::console::print_text("1.", color::green, mod::bold, " ");
                    tools::console::print_text("Select file\t(" + file_path + ")", color::blue);
                    tools::console::print_text("2.", color::green, mod::bold, " ");
                    tools::console::print_text("Select key\t(" + key_path + ")\n", color::blue);
                    tools::console::print_text("3. CONFIRM", color::red, mod::bold);
                    tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
                    tools::console::print_text("Select menu item:", color::green, mod::bold, " ");

                    file_opt = tools::console::get_correct_int();
                    if (file_opt == 1)
                        file_path = fsm_.get_file_path();
                    else if (file_opt == 2)
                        key_path = fsm_.get_file_path();
                    else if (file_opt == 0 || file_opt == 3)
                        break;
                }

                if (file_opt != 0 && file_path != "null" && key_path != "null") {
                    if (opt == 2)
                        rsa_controller_.Encrypt(file_path, key_path);
                    else if (opt == 3)
                        rsa_controller_.Decrypt(file_path, key_path);
                }
            }
        }
    }

    void RunDES() {
        while (true) {
            tools::console::console_clear();
            tools::console::print_text("DES:\n", color::green, mod::bold);
            tools::console::print_text("1.", color::green, mod::bold, " ");
            tools::console::print_text("Encrypt file", color::blue);
            tools::console::print_text("2.", color::green, mod::bold, " ");
            tools::console::print_text("Decrypt file", color::blue, "", "\n\n");
            tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
            tools::console::print_text("Select menu item:", color::green, mod::bold, " ");

            int opt{tools::console::get_correct_int()};
            if (opt == 0) {
                break;
            } else if (opt == 1 || opt == 2) {
                std::string option{"ENCRYPT:"};
                if (opt == 2)
                    option = "DECRYPT:";

                std::string file_path{"null"};
                std::string key_path{"null"};
                int file_opt{};
                while (true) {
                    tools::console::console_clear();
                    tools::console::print_text("DES", color::green, mod::bold, " ");
                    tools::console::print_text(option, color::blue, mod::bold, "\n\n");
                    tools::console::print_text("1.", color::green, mod::bold, " ");
                    tools::console::print_text("Select file\t(" + file_path + ")", color::blue);
                    tools::console::print_text("2.", color::green, mod::bold, " ");
                    tools::console::print_text("Select key\t(" + key_path + ")\n", color::blue);
                    tools::console::print_text("3. CONFIRM", color::red, mod::bold);
                    tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
                    tools::console::print_text("Select menu item:", color::green, mod::bold, " ");

                    file_opt = tools::console::get_correct_int();
                    if (file_opt == 1)
                        file_path = fsm_.get_file_path();
                    else if (file_opt == 2)
                        key_path = fsm_.get_file_path();
                    else if (file_opt == 0 || file_opt == 3)
                        break;
                }

                if (file_opt != 0 && file_path != "null" && key_path != "null") {
                    if (opt == 1)
                        des_controller_.Encrypt(file_path, key_path);
                    else if (opt == 2)
                        des_controller_.Decrypt(file_path, key_path);
                }
            }
        }
    }

private:
    void ShowMenu() const noexcept {
        tools::console::print_text("MENU:\n", color::green, mod::bold);
        tools::console::print_text("1.", color::green, mod::bold, " ");
        tools::console::print_text("Enigma", color::blue);
        tools::console::print_text("2.", color::green, mod::bold, " ");
        tools::console::print_text("Huffman", color::blue);
        tools::console::print_text("3.", color::green, mod::bold, " ");
        tools::console::print_text("RSA", color::blue);
        tools::console::print_text("4.", color::green, mod::bold, " ");
        tools::console::print_text("DES", color::blue, "", "\n\n");
        tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
        tools::console::print_text("Select menu item:", color::green, mod::bold, " ");
    }

private:
    menu menu_{
        {1, [this]() { RunEnigma(); }},
        {2, [this]() { RunHuffman(); }},
        {3, [this]() { RunRSA(); }},
        {4, [this]() { RunDES(); }}
    };

    tools::filesystem::monitoring fsm_;

    RSAController rsa_controller_;
    DESController des_controller_;
    HuffmanController huffman_controller_;
    std::unique_ptr<EnigmaController> enigma_controller_;
};
}  // namespace s21

#endif // CRYPTO_VIEW_CONSOLE_VIEW_HPP
