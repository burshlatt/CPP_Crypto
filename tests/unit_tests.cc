#include "gtest/gtest.h"

#include "enigma.hpp"
#include "des.hpp"
#include "rsa.hpp"
#include "huffman.hpp"

#include "tools.hpp"

TEST(Enigma, enigma_test_simple_file) {
    s21::Enigma e(20);
    e.Encrypt("../datasets/files/test.txt");
    e.Encrypt("../datasets/files/test_encoded.txt");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../datasets/files/test.txt"))};
    auto file_b{fsm_.read_file(fs::path("../datasets/files/test_encoded_decoded.txt"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

TEST(Enigma, enigma_test_binary_file) {
    s21::Enigma e(20);
    e.Encrypt("../datasets/files/test_binary.bin");
    e.Encrypt("../datasets/files/test_binary_encoded.txt");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../datasets/files/test_binary.bin"))};
    auto file_b{fsm_.read_file(fs::path("../datasets/files/test_binary_encoded_decoded.txt"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
