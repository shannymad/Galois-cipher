#include "GF2n.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

static void test_constructor() {
    GF2n f(3, 0b011);
    assert(f.n()    == 3);
    assert(f.poly() == 0b011);

    bool thrown = false;
    try { GF2n bad(0, 0b011); } catch (const std::invalid_argument&) { thrown = true; }
    assert(thrown);

    thrown = false;
    try { GF2n bad(64, 0); } catch (const std::invalid_argument&) { thrown = true; }
    assert(thrown);
}

static void test_xtime_table() {
    GF2n f(3, 0b011);
    assert(f.xtime(0b000) == 0b000);
    assert(f.xtime(0b001) == 0b010);
    assert(f.xtime(0b010) == 0b100);
    assert(f.xtime(0b011) == 0b110);
    assert(f.xtime(0b100) == 0b011);
    assert(f.xtime(0b101) == 0b001);
    assert(f.xtime(0b110) == 0b111);
    assert(f.xtime(0b111) == 0b101);
}

static void test_xtime_period() {
    GF2n f(3, 0b011);
    uint64_t s = 1;
    for (int i = 1; i <= 6; ++i) {
        s = f.xtime(s);
        assert(s != 1);
    }
    s = f.xtime(s);
    assert(s == 1);
}

static void test_multiply_is_commutative() {
    GF2n f(3, 0b011);
    for (uint64_t a = 0; a < 8; ++a)
        for (uint64_t b = 0; b < 8; ++b)
            assert(f.multiply(a, b) == f.multiply(b, a));
}

static void test_multiply_is_associative() {
    GF2n f(3, 0b011);
    for (uint64_t a = 0; a < 8; ++a)
        for (uint64_t b = 0; b < 8; ++b)
            for (uint64_t c = 0; c < 8; ++c)
                assert(f.multiply(f.multiply(a, b), c) ==
                       f.multiply(a, f.multiply(b, c)));
}

static void test_multiply_is_distributive() {
    GF2n f(3, 0b011);
    for (uint64_t a = 0; a < 8; ++a)
        for (uint64_t b = 0; b < 8; ++b)
            for (uint64_t c = 0; c < 8; ++c)
                assert(f.multiply(a, b ^ c) ==
                       (f.multiply(a, b) ^ f.multiply(a, c)));
}

static void test_multiply_neutral_and_zero() {
    GF2n f(3, 0b011);
    for (uint64_t a = 0; a < 8; ++a) {
        assert(f.multiply(a, 1) == a);
        assert(f.multiply(1, a) == a);
        assert(f.multiply(a, 0) == 0);
        assert(f.multiply(0, a) == 0);
    }
}

static void test_multiply_concrete() {
    GF2n f(3, 0b011);
    assert(f.multiply(1, 1) == 1);
    assert(f.multiply(2, 2) == 4);
    assert(f.multiply(4, 2) == 3);
    assert(f.multiply(5, 3) == 4);
}

static void test_pow_basic() {
    GF2n f(3, 0b011);
    for (uint64_t g = 1; g < 8; ++g) {
        assert(f.pow(g, 0) == 1);
        assert(f.pow(g, 1) == g);
        assert(f.pow(g, 2) == f.multiply(g, g));
    }
}

static void test_pow_additive_in_exponent() {
    GF2n f(3, 0b011);
    for (uint64_t g = 1; g < 8; ++g)
        for (uint64_t a = 0; a < 8; ++a)
            for (uint64_t b = 0; b < 8; ++b)
                assert(f.pow(g, a + b) ==
                       f.multiply(f.pow(g, a), f.pow(g, b)));
}

static void test_pow_fermat_n3() {
    GF2n f(3, 0b011);
    for (uint64_t g = 1; g < 8; ++g)
        assert(f.pow(g, 7) == 1);
}

static void test_pow_concrete() {
    GF2n f(3, 0b011);
    assert(f.pow(2, 3) == 3);
    assert(f.pow(2, 7) == 1);
}

static void test_gf256_fermat() {
    GF2n f(8, 0x1D);
    for (uint64_t g = 1; g < 256; ++g)
        assert(f.pow(g, 255) == 1);
}

static void test_gf256_xtime_cycle() {
    GF2n f(8, 0x1D);
    uint64_t s = 1;
    for (int i = 0; i < 255; ++i)
        s = f.xtime(s);
    assert(s == 1);
}

static void test_gf256_xtime_known_values() {
    GF2n f(8, 0x1D);
    assert(f.xtime(0x01) == 0x02);
    assert(f.xtime(0x40) == 0x80);
    assert(f.xtime(0x80) == 0x1D);
    assert(f.xtime(0x1D) == 0x3A);
}

int main() {
    test_constructor();
    test_xtime_table();
    test_xtime_period();
    test_multiply_is_commutative();
    test_multiply_is_associative();
    test_multiply_is_distributive();
    test_multiply_neutral_and_zero();
    test_multiply_concrete();
    test_pow_basic();
    test_pow_additive_in_exponent();
    test_pow_fermat_n3();
    test_pow_concrete();
    test_gf256_fermat();
    test_gf256_xtime_cycle();
    test_gf256_xtime_known_values();
    return 0;
}
