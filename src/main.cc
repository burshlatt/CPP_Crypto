// #include <iostream>

// #include "console_view.hpp"

// int main() {
//     s21::ConsoleView app;
//     app.RunApp();
//     return 0;
// }

#include <iostream>

#include "rsa.hpp"

int main() {
    s21::RSA rsa_c;
    rsa_c.GenerateKeys(fs::current_path().generic_string());
    return 0;
}
