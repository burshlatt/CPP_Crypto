#ifndef CRYPTO_VIEW_CONSOLE_VIEW_HPP
#define CRYPTO_VIEW_CONSOLE_VIEW_HPP

#include <map>
#include <memory>
#include <iostream>
#include <functional>

#include "tools.hpp"

#include "enigma_controller.hpp"
#include "huffman_controller.hpp"

namespace s21 {
class ConsoleView {
private:
    using mod   = tools::console::ansi::mods;
    using color = tools::console::ansi::colors;
    using menu  = std::map<int, std::function<void()>>;

public:
    ConsoleView() :
        huffman_controller_(std::make_unique<HuffmanController>())
    {}

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
            tools::console::print_text("Read configuration file", color::blue);
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
                tools::console::print_text("Select a file", color::blue);
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
                    huffman_controller_->Encrypt(fsm_.get_file_path());
                    
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
                    huffman_controller_->Decrypt(file_path, config_path);
            } else if (opt == 0) {
                break;
            }
        }
    }

private:
    void ShowMenu() const noexcept {
        tools::console::print_text("MENU:\n", color::green, mod::bold);
        tools::console::print_text("1.", color::green, mod::bold, " ");
        tools::console::print_text("Enigma", color::blue, "");
        tools::console::print_text("2.", color::green, mod::bold, " ");
        tools::console::print_text("Huffman", color::blue, "", "\n\n");
        tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
        tools::console::print_text("Select menu item:", color::green, mod::bold, " ");
    }

private:
    menu menu_{
        {1, [this]() { RunEnigma(); }},
        {2, [this]() { RunHuffman(); }}
    };

    tools::filesystem::monitoring fsm_;

    std::unique_ptr<EnigmaController> enigma_controller_;
    std::unique_ptr<HuffmanController> huffman_controller_;
};
}  // namespace s21

#endif // CRYPTO_VIEW_CONSOLE_VIEW_HPP
