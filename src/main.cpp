#include "GaloisCipher.h"
#include "GF2n.h"

#include <iostream>
#include <string>
#include <exception>




int main(int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <enc|dec> <input> <output> <seed>\n";
        return 1;
    }


    std::string mode = argv[1];
    if (mode != "enc" && mode != "dec") {
        std::cerr << "Error: mode must be 'enc' or 'dec'\n";
        return 1;
    }
    bool encrypt = (mode == "enc");

    std::string in_path = argv[2];
    std::string out_path = argv[3];


    uint64_t seed = 0;
    try {
        seed = std::stoull(argv[4]);
    } catch (const std::exception& e) {
        std::cerr << "Error: invalid seed: " << e.what() << "\n";
        return 1;
    }

    constexpr int n = 8;
    constexpr uint64_t poly = 0x1D;
    GF2n field(n, poly);

    bool ok = false;
    if (encrypt) {
        ok = GaloisCipher::encrypt_file(in_path, out_path, field, seed);
    } else {
        ok = GaloisCipher::decrypt_file(in_path, out_path, field, seed);
    }


    if (!ok) {
        std::cerr << "Error: cannot process file\n";
        return 2;
    }
    std::cout << "Done: " << out_path << "\n";
    return 0;
}