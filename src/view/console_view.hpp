#ifndef CRYPTO_VIEW_CONSOLE_VIEW_HPP
#define CRYPTO_VIEW_CONSOLE_VIEW_HPP

#include <map>
#include <memory>
#include <iostream>
#include <functional>

#include "tools.hpp"

#include "enigma_controller.hpp"

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

private:
    void ShowMenu() const noexcept {
        tools::console::print_text("MENU:\n", color::green, mod::bold);
        tools::console::print_text("1.", color::green, mod::bold, " ");
        tools::console::print_text("Enigma", color::blue, "", "\n\n");
        tools::console::print_text("0. EXIT", color::red, mod::bold, "\n\n");
        tools::console::print_text("Select menu item:", color::green, mod::bold, " ");
    }

private:
    menu menu_{
        {1, [this]() { RunEnigma(); }}
    };

    tools::filesystem::monitoring fsm_;

    std::unique_ptr<EnigmaController> enigma_controller_;
};
}  // namespace s21

#endif // CRYPTO_VIEW_CONSOLE_VIEW_HPP
