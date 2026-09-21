#pragma once

#include <cstdint>
#include <stdexcept>

class GF2n {
public:
  GF2n() = default;

  GF2n(int n, uint64_t poly) : n_(n), poly_(poly) {
    if (n <= 0 || n >= 64) {
      throw std::invalid_argument("incorrect dimension");
    };
  }

  int n() const noexcept { return n_; }

  uint64_t poly() const noexcept { return poly_; }

  // Multiplication by x with reduction mod f
  // state = a_0 + a_1·x + a_2·x² + ... + a_{n-1}·x^{n-1}
  uint64_t xtime(uint64_t state) const noexcept {
    state = state & mask(); // reduce state to n bits
    uint64_t carry = (state >> (n_ - 1)) &
                     1; // remembering the eldest state bit ~ coeff by x ^ {n-1}
    state = state << 1; // shift left
    state = state & mask(); // removing the unwanted bit
    // checking if reduction is necessary and reducing
    if (carry) {
      state = state ^ poly_;
    }
    return state;
  }

  // Multiplication of two elements in the field
  // b = b_0 + b_1·x + b_2·x² + ... + b_{n-1}·x^{n-1}
  //  a · b = b_0·(a·x^0) + b_1·(a·x^1) + ... + b_{n-1}·(a·x^{n-1}) (distributivity)
  uint64_t multiply(uint64_t a, uint64_t b) const noexcept {
    uint64_t result = 0;
    while (b != 0) {
      if ((b & 1) == 1) {
        result = result ^ a;
      }
      a = xtime(a);
      b = b >> 1;
    }
    result &= mask();
    return result;
  }

  uint64_t pow(uint64_t g, uint64_t e) const noexcept {
    uint64_t result = 1;
    uint64_t base = g; // g ^ (e ^ 0)
    while (e != 0) {
      if ((e & 1) == 1) {
        result = multiply(result, base);
      }
      base = multiply(base, base);
      e = e >> 1;
    }
    return result;
  }


  uint64_t mod(uint64_t v) const noexcept {
    return v& mask();
  }

private:
  int n_;         // dimension
  uint64_t poly_; // irreducible polynomial generating our field

  uint64_t mask() const noexcept { return (1ull << n_) - 1; }
};
