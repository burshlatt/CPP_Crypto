// #include <iostream>

// #include "console_view.hpp"

// int main() {
//     s21::ConsoleView app;
//     app.RunApp();
//     return 0;
// }

#include <iostream>
#include <cmath>

#include "rsa.hpp"

int main() {
    s21::RSA rsa_c;
    rsa_c.GenerateKeys("/mnt/d/Projects/CPP_Crypto/test");
    rsa_c.Encode("/mnt/d/Projects/CPP_Crypto/test/test_text.txt", "/mnt/d/Projects/CPP_Crypto/test/public_key");
    rsa_c.Decode("/mnt/d/Projects/CPP_Crypto/test/test_encoded.txt", "/mnt/d/Projects/CPP_Crypto/test/private_key");
    return 0;
}
