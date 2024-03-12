#include "gtest/gtest.h"

#include "enigma.hpp"
#include "des.hpp"
#include "rsa.hpp"
#include "huffman.hpp"

#include "tools.hpp"

TEST(Enigma, enigma_test_simple_file) {
    s21::Enigma e(20);
    e.Encrypt("../../datasets/files/test.txt");
    e.Encrypt("../../datasets/files/test_encoded.txt");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../../datasets/files/test.txt"))};
    auto file_b{fsm_.read_file(fs::path("../../datasets/files/test_encoded_decoded.txt"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

TEST(Enigma, enigma_test_binary_file) {
    s21::Enigma e(20);
    e.Encrypt("../../datasets/files/test_binary.bin");
    e.Encrypt("../../datasets/files/test_binary_encoded.bin");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../../datasets/files/test_binary.bin"))};
    auto file_b{fsm_.read_file(fs::path("../../datasets/files/test_binary_encoded_decoded.bin"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

TEST(Enigma, enigma_test_simple_file_load_config) {
    s21::Enigma e_a("../../datasets/configurations/enigma_config.cfg");
    s21::Enigma e_b("../../datasets/configurations/enigma_config.cfg");
    e_a.Encrypt("../../datasets/files/test.txt");
    e_b.Encrypt("../../datasets/files/test_encoded.txt");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../../datasets/files/test.txt"))};
    auto file_b{fsm_.read_file(fs::path("../../datasets/files/test_encoded_decoded.txt"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

TEST(Enigma, enigma_test_binary_file_load_config) {
    s21::Enigma e_a("../../datasets/configurations/enigma_config.cfg");
    s21::Enigma e_b("../../datasets/configurations/enigma_config.cfg");
    e_a.Encrypt("../../datasets/files/test_binary.bin");
    e_b.Encrypt("../../datasets/files/test_binary_encoded.bin");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../../datasets/files/test_binary.bin"))};
    auto file_b{fsm_.read_file(fs::path("../../datasets/files/test_binary_encoded_decoded.bin"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

TEST(Huffman, huffman_test_simple_file) {
    s21::Huffman h;
    h.Encode("../../datasets/files/test.txt");
    h.Decode("../../datasets/files/test_encoded.txt", "../../datasets/files/test_encoded.cfg");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../../datasets/files/test.txt"))};
    auto file_b{fsm_.read_file(fs::path("../../datasets/files/test_encoded_decoded.txt"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

TEST(Huffman, huffman_test_binary_file) {
    s21::Huffman h;
    h.Encode("../../datasets/files/test_binary.bin");
    h.Decode("../../datasets/files/test_binary_encoded.bin", "../../datasets/files/test_binary_encoded.cfg");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../../datasets/files/test_binary.bin"))};
    auto file_b{fsm_.read_file(fs::path("../../datasets/files/test_binary_encoded_decoded.bin"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

TEST(RSA, rsa_test_simple_file) {
    s21::RSA r;
    r.GenerateKeys("../../datasets/configurations/");
    r.Encode("../../datasets/files/test.txt", "../../datasets/configurations/public_key");
    r.Decode("../../datasets/files/test_encoded.txt", "../../datasets/configurations/private_key");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../../datasets/files/test.txt"))};
    auto file_b{fsm_.read_file(fs::path("../../datasets/files/test_encoded_decoded.txt"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

TEST(RSA, rsa_test_binary_file) {
    s21::RSA r;
    r.GenerateKeys("../../datasets/configurations/");
    r.Encode("../../datasets/files/test_binary.bin", "../../datasets/configurations/public_key");
    r.Decode("../../datasets/files/test_binary_encoded.bin", "../../datasets/configurations/private_key");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../../datasets/files/test_binary.bin"))};
    auto file_b{fsm_.read_file(fs::path("../../datasets/files/test_binary_encoded_decoded.bin"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

TEST(DES, des_test_simple_file) {
    s21::DES d;
    d.EncodeECB("../../datasets/files/test.txt", "../../datasets/configurations/des_key.txt");
    d.DecodeECB("../../datasets/files/test_encoded.txt", "../../datasets/configurations/des_key.txt");
    tools::filesystem::monitoring fsm_;
    auto file_a{fsm_.read_file(fs::path("../../datasets/files/test.txt"))};
    auto file_b{fsm_.read_file(fs::path("../../datasets/files/test_encoded_decoded.txt"))};
    EXPECT_EQ(file_a.get_text(), file_b.get_text());
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
