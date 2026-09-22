#pragma once 


#include "GF2n.h"
class LFSR {
    public: 

    LFSR(GF2n field, uint64_t seed) : field_(field), state_(seed){
        set_state(seed);
    }


    uint8_t next_bit() noexcept {
        uint8_t random = state_ & 1;
        state_ = field_.xtime(state_);
        return random;
    }
    
    uint8_t next_byte() noexcept {
        uint8_t byte = 0;
        for (size_t k = 0; k < 8; ++k) {
            uint8_t b = next_bit();
            byte |= (b << k);
        }
        return byte;
    }

    void fill(uint8_t* buf, size_t size) noexcept{
        for (size_t i = 0; i < size; ++i) {
            buf[i] = next_byte();
        }
    }

    uint64_t state() const noexcept{
        return state_;
    }

    void set_state(uint64_t state) noexcept {
        state = field_.mod(state);
        if (state == 0) {
            state = 1;
        }
        state_ = state;
    }


    private:
    GF2n field_;
    uint64_t state_; // current state 
};
