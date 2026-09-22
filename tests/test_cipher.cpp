#include "GaloisCipher.h"
#include "GF2n.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <vector>


static void write_file(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
}

static std::vector<uint8_t> read_file(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(in)),
                              std::istreambuf_iterator<char>());
    return data;
}

static std::vector<uint8_t> make_data(std::size_t n) {
    std::vector<uint8_t> v(n);
    for (std::size_t i = 0; i < n; ++i)
        v[i] = static_cast<uint8_t>((i * 31 + 7) & 0xFF);
    return v;
}


static void test_process_size_zero() {
    GF2n f(8, 0x1D);
    GaloisCipher c(f, 1);
    uint8_t in[4]  = {1, 2, 3, 4};
    uint8_t out[4] = {9, 9, 9, 9};
    c.process(in, out, 0);
    assert(out[0] == 9 && out[1] == 9 && out[2] == 9 && out[3] == 9);
}

static void test_process_matches_manual_xor() {
    GF2n f(8, 0x1D);
    GaloisCipher c(f, 42);
    LFSR l(f, 42);

    std::vector<uint8_t> in = make_data(64);
    std::vector<uint8_t> out(64);
    c.process(in.data(), out.data(), in.size());

    for (std::size_t i = 0; i < in.size(); ++i) {
        assert(out[i] == (in[i] ^ l.next_byte()));
    }
}

static void test_process_empty_vector() {
    GF2n f(8, 0x1D);
    GaloisCipher c(f, 1);
    std::vector<uint8_t> in;
    std::vector<uint8_t> out = c.process(in);
    assert(out.empty());
}

static void test_roundtrip_vector() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> in = make_data(1000);

    GaloisCipher enc(f, 42);
    std::vector<uint8_t> cipher = enc.process(in);

    GaloisCipher dec(f, 42);
    std::vector<uint8_t> plain = dec.process(cipher);

    assert(plain == in);
}

static void test_same_seed_same_ciphertext() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> in = make_data(100);

    GaloisCipher a(f, 123), b(f, 123);
    std::vector<uint8_t> ca = a.process(in);
    std::vector<uint8_t> cb = b.process(in);

    assert(ca == cb);
}

static void test_different_seed_different_ciphertext() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> in = make_data(64);

    GaloisCipher a(f, 1), b(f, 2);
    std::vector<uint8_t> ca = a.process(in);
    std::vector<uint8_t> cb = b.process(in);

    assert(ca != cb);
}

static void test_ciphertext_differs_from_plaintext() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> in = make_data(64);

    GaloisCipher c(f, 42);
    std::vector<uint8_t> cipher = c.process(in);

    bool any_diff = false;
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] != cipher[i]) { any_diff = true; break; }
    }
    assert(any_diff);
}

static void test_inplace_process() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> in = make_data(128);

    // two buffers
    GaloisCipher a(f, 7);
    std::vector<uint8_t> out_a(in.size());
    a.process(in.data(), out_a.data(), in.size());

    // in-place
    GaloisCipher b(f, 7);
    std::vector<uint8_t> out_b = in;
    b.process(out_b.data(), out_b.data(), out_b.size());

    assert(out_a == out_b);
}



static void test_encrypt_missing_input() {
    GF2n f(8, 0x1D);
    bool ok = GaloisCipher::encrypt_file(
        "/nonexistent/definitely/not/here.bin",
        "/tmp/should_not_be_created.bin",
        f, 1);
    assert(!ok);
}

static void test_roundtrip_file_small() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> data = make_data(10);

    write_file("test_in.bin", data);
    bool ok1 = GaloisCipher::encrypt_file("test_in.bin", "test_enc.bin", f, 42);
    bool ok2 = GaloisCipher::decrypt_file("test_enc.bin", "test_dec.bin", f, 42);
    assert(ok1 && ok2);

    std::vector<uint8_t> result = read_file("test_dec.bin");
    assert(result == data);

    std::remove("test_in.bin");
    std::remove("test_enc.bin");
    std::remove("test_dec.bin");
}

static void test_roundtrip_file_block_boundary() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> data = make_data(4096);

    write_file("test_in.bin", data);
    assert(GaloisCipher::encrypt_file("test_in.bin", "test_enc.bin", f, 42));
    assert(GaloisCipher::decrypt_file("test_enc.bin", "test_dec.bin", f, 42));
    assert(read_file("test_dec.bin") == data);

    std::remove("test_in.bin");
    std::remove("test_enc.bin");
    std::remove("test_dec.bin");
}

static void test_roundtrip_file_just_over_block() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> data = make_data(4097);

    write_file("test_in.bin", data);
    assert(GaloisCipher::encrypt_file("test_in.bin", "test_enc.bin", f, 42));
    assert(GaloisCipher::decrypt_file("test_enc.bin", "test_dec.bin", f, 42));
    assert(read_file("test_dec.bin") == data);

    std::remove("test_in.bin");
    std::remove("test_enc.bin");
    std::remove("test_dec.bin");
}

static void test_roundtrip_file_multi_block() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> data = make_data(10000);

    write_file("test_in.bin", data);
    assert(GaloisCipher::encrypt_file("test_in.bin", "test_enc.bin", f, 42));
    assert(GaloisCipher::decrypt_file("test_enc.bin", "test_dec.bin", f, 42));
    assert(read_file("test_dec.bin") == data);

    std::remove("test_in.bin");
    std::remove("test_enc.bin");
    std::remove("test_dec.bin");
}

static void test_roundtrip_empty_file() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> empty;

    write_file("test_in.bin", empty);
    assert(GaloisCipher::encrypt_file("test_in.bin", "test_enc.bin", f, 42));
    assert(GaloisCipher::decrypt_file("test_enc.bin", "test_dec.bin", f, 42));
    assert(read_file("test_dec.bin").empty());

    std::remove("test_in.bin");
    std::remove("test_enc.bin");
    std::remove("test_dec.bin");
}

static void test_encrypt_file_creates_different_output() {
    GF2n f(8, 0x1D);
    std::vector<uint8_t> data = make_data(100);

    write_file("test_in.bin", data);
    assert(GaloisCipher::encrypt_file("test_in.bin", "test_enc.bin", f, 42));

    std::vector<uint8_t> cipher = read_file("test_enc.bin");
    assert(cipher.size() == data.size());
    assert(cipher != data);

    std::remove("test_in.bin");
    std::remove("test_enc.bin");
}



int main() {
    test_process_size_zero();
    test_process_matches_manual_xor();
    test_process_empty_vector();
    test_roundtrip_vector();
    test_same_seed_same_ciphertext();
    test_different_seed_different_ciphertext();
    test_ciphertext_differs_from_plaintext();
    test_inplace_process();

    test_encrypt_missing_input();
    test_roundtrip_file_small();
    test_roundtrip_file_block_boundary();
    test_roundtrip_file_just_over_block();
    test_roundtrip_file_multi_block();
    test_roundtrip_empty_file();
    test_encrypt_file_creates_different_output();

    return 0;
}