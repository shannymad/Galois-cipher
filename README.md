# Galois Stream Cipher

A stream cipher based on the **Galois Linear Feedback Shift Register (LFSR)**
over finite fields, implemented in C++. The project includes:

- A Galois LFSR random bit generator.
- A stream cipher using XOR with the generated pseudorandom bit stream.
- Encryption and decryption of arbitrary binary files.

---

## Theoretical Background

### 1. Galois Random Generator

There is a concept of a **linear feedback shift register (LFSR)** — a special
kind of register producing a sequence of bits. Based on it, one can build
pseudorandom number generators. The following describes the Galois generator.

Suppose we have a finite alphabet `Z_p = {0, 1, ..., p-1}` and we want to
create a random sequence of elements from this alphabet. There are many ways
to do this, but here is the approach based on finite fields.

Fix a finite field `F_{p^n}`. We need to produce a random sequence of elements
`a_1, a_2, ... ∈ F_{p^n}`, and then, for each such element, construct an
element `φ(a_1), φ(a_2), ... ∈ Z_p`. A good sequence of `a_i` can be built
using **exponentiation in the finite field**. To compute elements of `Z_p`
from the sequence `a_i`, recall that `F_{p^n}` is a **vector space over `Z_p`**.
In this case, `φ` can be any procedure computing one of the coordinates.

More precisely:

- Let `p` be a prime number and fix some natural `n`.
- Take an **irreducible polynomial** `f ∈ Z_p[x]` of degree `n` and construct
  the field `F_{p^n} = Z_p[x] / (f)`.

By definition:

```
F_{p^n} = { a_0 + a_1 x + ... + a_{n-1} x^{n-1} | a_i ∈ Z_p }
```

That is, each element of the field is determined by the sequence
`(a_0, a_1, ..., a_{n-1})`.

Now choose a **generator** `g ∈ F_{p^n}^*`. Usually, `f` is chosen so that `x` is a
generator. Then we produce the sequence `g, g^2, g^3, g^4, ...`. Each power of
`g` corresponds to a sequence of coefficients `a_i` as above. Then as a random
element of `Z_p` we can take `a_0`.

Moreover, we can start the sequence not necessarily from the zeroth power.
We can start from an arbitrary power `k`: `g^k, g^{k+1}, g^{k+2}, ...`.
From a practical point of view, this means we chose an element
`h ∈ F_{p^n}^*` and build the sequence `h g, h g^2, h g^3, h g^4, ...`.
Since `h = g^k` for some `k`, this is an equivalent approach.

### 2. Matrix Form

Let us rewrite the construction above in coordinates. As before, assume:

```
F_{p^n} = Z_p[x] / (f) = { a_0 + a_1 x + ... + a_{n-1} x^{n-1} | a_i ∈ Z_p }
```

In this case `1, x, x^2, ..., x^{n-1}` is a **basis** of `F_{p^n}` over `Z_p`.
Suppose the polynomial

```
f = x^n + c_{n-1} x^{n-1} + ... + c_1 x + c_0
```

is chosen so that `x` is a generator of `F_{p^n}^*`. The map
`φ : F_{p^n} → F_{p^n}` defined by `h ↦ x h` is **linear**. Its matrix in the
basis of powers of `x` is:

```
        ⎡ 0    0   ...  0   c_0    ⎤
        ⎢ 1    0   ...  0   c_1    ⎥
A  =    ⎢ 0    1   ...  0   c_2    ⎥
        ⎢ ...  ...  ... ...  ...   ⎥
        ⎣ 0    0   ...  1   c_{n-1}⎦
```

Denote the matrix of `φ` by `A`. An element
`h = a_0 + a_1 x + ... + a_{n-1} x^{n-1}` is described in coordinates by the
column vector `v = (a_0, a_1, ..., a_{n-1})^T`. Then `x h` is described by
the product `A v`.

Therefore, the random generator works as follows. We fix some initial vector
`v = (a_0, a_1, ..., a_{n-1})^T` and build the sequence of vectors
`v, A v, A^2 v, A^3 v, ...`. Then, for each vector, we take the **first
coordinate** and obtain the required random sequence.


### 3. Stream Encryption

There is an application of random generators in cryptography. Consider
**stream encryption**. Note that when transmitting data, encryption imposes
additional overhead. Transmitting data over an encrypted channel can be
expensive in the sense that we cannot afford to do it often. Nevertheless, we
still do not want to transmit plaintext over an insecure channel.

Here is what can be done. Suppose the alphabet consists of two symbols
`Z_2 = {0, 1}`. Our message is a sequence of bits `a_0, a_1, a_2, a_3, ...`.
Suppose we have a random generator `S : N → Z_2`. Then we can construct a
sequence of random bits using it:

```
s_0 = S(0), s_1 = S(1), s_2 = S(2), ...
```

Now each bit of the original message can be encrypted with a random bit as
follows:

```
d_k = a_k + s_k  (mod 2)
```

and transmit `d_k` instead of `a_k`. To recover the message, compute:

```
a_k = d_k + s_k  (mod 2)
```

For this, we need to know exactly the sequence of random bits `s_k`. Instead
of transmitting `s_k` over the encrypted channel, we transmit a "copy" of
our random generator — more precisely, we transmit the information needed to
initialize an identical random generator. Then, given two synchronized random
generators, we can freely encrypt and decrypt messages using the scheme above.

Modern cryptography uses more complex encryption schemes, but the idea above
lies at their core.

## Project Description

This project implements a minimal stream cipher in C++ based on the Galois
LFSR. It consists of:

- **`LFSR`** — a Galois linear feedback shift register over `GF(2^n)`.
- **`GaloisCipher`** — a stream cipher that XORs the input data with the
  bit stream produced by the LFSR.
- **`main`** — a demonstration program that encrypts and decrypts a file.

### Features

- Encrypts and decrypts arbitrary binary files.
- Deterministic: same `seed` produces the same keystream.
- XOR-based: encryption and decryption are the same operation.
- No external dependencies (only the C++ standard library).


## Known Limitations

The following limitations are intentional and reflect the educational scope
of the project. They are documented here so that the reader is not misled.
This project is an **educational implementation** of a stream cipher based on
the Galois LFSR over `GF(2^n)`. Its purpose is to demonstrate the idea, not
to provide a secure encryption tool.

- It is **not cryptographically secure**. A single LFSR is a linear system,
  which means the whole keystream can be reconstructed from a small piece of
  known plaintext. For `n = 8`, sixteen bits — just two bytes — are enough to
  recover the seed and break the cipher completely.

- It is **not optimized for speed**. The code is written to be easy to read,
  not to be fast. Every byte of the keystream is built one bit at a time,
  through eight separate calls to `next_bit`, and each of those calls does
  one multiplication by `x`.

- It is **not resistant to key reuse**. The same `seed` always produces the
  same keystream. If you encrypt two different messages with the same seed,
  the two ciphertexts are related in a simple way: their XOR equals the XOR
  of the two original messages. Anyone who knows one of them can recover the
  other. Every stream cipher has this weakness unless it uses a fresh IV for
  each message, and this project does not use an IV.

## Building and Running

Requirements:

- C++17 compiler (GCC, Clang, MSVC).
- CMake ≥ 3.15.