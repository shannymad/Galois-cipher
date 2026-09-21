#include "GF2n.h"
#include "LFSR.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <set>
#include <vector>

static void test_seed_zero_is_replaced() {
    GF2n f(8, 0x1D);
    LFSR l(f, 0);
    assert(l.state() != 0);
}

static void test_set_state_masks_and_protects() {
    GF2n f(8, 0x1D);
    LFSR l(f, 1);

    l.set_state(0xFFFF);
    assert(l.state() == 0xFF);

    l.set_state(0x100);
    assert(l.state() == 1);

    l.set_state(0);
    assert(l.state() == 1);

    l.set_state(0x42);
    assert(l.state() == 0x42);
}

static void test_same_seed_same_stream() {
    GF2n f(8, 0x1D);
    LFSR a(f, 42), b(f, 42);
    for (int i = 0; i < 1000; ++i)
        assert(a.next_bit() == b.next_bit());
}

static void test_next_bit_is_binary() {
    GF2n f(8, 0x1D);
    LFSR l(f, 1);
    for (int i = 0; i < 1000; ++i) {
        uint8_t b = l.next_bit();
        assert(b == 0 || b == 1);
    }
}

static void test_stream_not_all_zero() {
    GF2n f(8, 0x1D);
    LFSR l(f, 1);
    bool any = false;
    for (int i = 0; i < 100 && !any; ++i)
        if (l.next_byte() != 0) any = true;
    assert(any);
}

static void test_period_is_255() {
    GF2n f(8, 0x1D);
    LFSR l(f, 1);
    assert(l.state() == 1);

    std::set<uint64_t> seen;
    seen.insert(1);

    for (int i = 0; i < 255; ++i) {
        l.next_bit();
        uint64_t s = l.state();
        assert(s != 0);
        if (i < 254) {
            if (seen.count(s) != 0) {
    std::fprintf(stderr, "repeat at step %d: state = 0x%llX\n",
                 i, (unsigned long long)s);
    std::fflush(stderr);
    std::exit(1);
}
        }
        seen.insert(s);
    }

    assert(l.state() == 1);
    assert(seen.size() == 255);
}

static void test_next_byte_manual_assembly() {
    GF2n f(8, 0x1D);
    LFSR a(f, 123), b(f, 123);
    for (int i = 0; i < 50; ++i) {
        uint8_t manual = 0;
        for (int k = 0; k < 8; ++k)
            manual |= static_cast<uint8_t>(b.next_bit() << k);
        assert(a.next_byte() == manual);
    }
}

static void test_first_byte_for_seed_one() {
    GF2n f(8, 0x1D);
    LFSR l(f, 1);
    assert(l.next_byte() == 0x01);
}

static void test_fill_matches_next_byte_loop() {
    GF2n f(8, 0x1D);
    LFSR a(f, 7), b(f, 7);
    const std::size_t N = 64;
    std::vector<uint8_t> va(N), vb(N);
    a.fill(va.data(), N);
    for (std::size_t i = 0; i < N; ++i) vb[i] = b.next_byte();
    assert(va == vb);
}

int main() {
    test_seed_zero_is_replaced();
    test_set_state_masks_and_protects();
    test_same_seed_same_stream();
    test_next_bit_is_binary();
    test_stream_not_all_zero();
    test_period_is_255();
    test_next_byte_manual_assembly();
    test_first_byte_for_seed_one();
    test_fill_matches_next_byte_loop();
    return 0;
}
