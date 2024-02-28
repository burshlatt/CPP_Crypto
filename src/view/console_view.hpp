#ifndef CRYPTO_VIEW_CONSOLE_VIEW_HPP
#define CRYPTO_VIEW_CONSOLE_VIEW_HPP

#include <functional>
#include <iostream>
#include <map>

#include "console_tools.hpp"

namespace s21 {
class ConsoleView {
private:
    using mod   = console_tools::ansi::mods;
    using color = console_tools::ansi::colors;
    using menu  = std::map<int, std::function<void()>>;

public:
    ConsoleView() = default;
    ~ConsoleView() = default;

    void RunApp() {
        while (true) {
            this->ShowMenu();
            int opt{console_tools::get_correct_int()};

            if (opt == 0) break;

            // auto is_correct{menu_.find(opt)};
            // if (is_correct == menu_.end())
            //     continue;

            // this->menu_[opt]();
        }
    }

private:
    void ShowMenu() const noexcept {
        console_tools::console_clear();
        std::cout << mod::bold    << color::green          << "MENU:\n\n" << mod::reset;
        std::cout << color::green << "1.\t" << color::blue << "...\n"     << mod::reset;
        std::cout << color::red   << "0.\t" << mod::bold   << "Exit\n"    << mod::reset;
        std::cout << color::green << "\nSelect menu item: "               << mod::reset;
    }

private:
    // menu menu_{
    //     {1, [this]() { this->controller_->Set(); }}
    // };

    // Controller controller_;
};
}  // namespace s21

#endif // CRYPTO_VIEW_CONSOLE_VIEW_HPP
