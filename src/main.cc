#include <iostream>

#include "console_view.hpp"

// int main() {
//     s21::ConsoleView app;
//     app.RunApp();
//     return 0;
// }

int main () {
    int x{};
    std::cin >> x;
    s21::Enigma en(x);
    tools::filesystem::monitoring fsm;
    en.EncryptFile(fsm.get_file_path());
    return 0;
}
