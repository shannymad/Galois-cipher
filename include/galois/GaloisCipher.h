#pragma once

#include "GF2n.h"
#include "LFSR.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

class GaloisCipher {
public:
  GaloisCipher(GF2n field, uint64_t seed) : lfsr_(field, seed) {}
  void process(const uint8_t *in, uint8_t *out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
      out[i] = in[i] ^ lfsr_.next_byte();
    }
  }

  std::vector<uint8_t> process(const std::vector<uint8_t> &in) {
    std::vector<uint8_t> out(in.size());
    process(in.data(), out.data(), in.size());
    return out;
  }

  static bool encrypt_file(const std::string &in_path,
                           const std::string &out_path, GF2n field,
                           uint64_t seed) {
    std::ifstream in(in_path, std::ios::binary);
    if (!in) {
      return false;
    }
    std::ofstream out(out_path, std::ios::binary);
    if (!out) {
      return false;
    }

    GaloisCipher cipher(field, seed);
    constexpr std::size_t BUFSIZE = 4096;
    uint8_t buf[BUFSIZE];
    while (in) {
      in.read(reinterpret_cast<char *>(buf), BUFSIZE);
      std::streamsize got = in.gcount();
      if (got == 0)
        break;
      cipher.process(buf, buf, static_cast<size_t>(got));
      out.write(reinterpret_cast<const char *>(buf), got);
    }
    return true;
  }
  static bool decrypt_file(const std::string &in_path,
                           const std::string &out_path, GF2n field,
                           uint64_t seed) {
    return encrypt_file(in_path, out_path, field, seed);
  }

private:
  LFSR lfsr_;
};
