#include "ed25519.h"
#include "sha512.h"
#include "ge.h"
#include "sc.h"
#include <stdio.h>
static int consttime_equal(const unsigned char *x, const unsigned char *y) {
    unsigned char r = 0;

    r = x[0] ^ y[0];
    #define F(i) r |= x[i] ^ y[i]
    F(1);
    F(2);
    F(3);
    F(4);
    F(5);
    F(6);
    F(7);
    F(8);
    F(9);
    F(10);
    F(11);
    F(12);
    F(13);
    F(14);
    F(15);
    F(16);
    F(17);
    F(18);
    F(19);
    F(20);
    F(21);
    F(22);
    F(23);
    F(24);
    F(25);
    F(26);
    F(27);
    F(28);
    F(29);
    F(30);
    F(31);
    #undef F

    return !r;
}

int ed25519_verify(const unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key) {
    unsigned char h[64];
    unsigned char checker[32];
    sha512_context hash;
    ge_p3 A;
    ge_p2 R;
    int i;

    sha512_init(&hash);
    sha512_update(&hash, signature, 32);
    sha512_update(&hash, public_key, 32);
    sha512_update(&hash, message, message_len);
    sha512_final(&hash, h);

    sc_reduce(h);
    
    if (signature[63] & 224) {
        return 0;
    }
    sc_reduce(h);
    printf("\nrhash:");
    for (i=256/8 - 1;i>=0;i--)
        printf("%02x", (uint8_t) h[i]);
    printf("\n");
    printf("public_key:");
    for (i=256/8 - 1;i>=0;i--)
        printf("%02x", (uint8_t) public_key[i]);
    printf("\n");
    printf("signature: ");
    for (i=512/8 - 1;i>=0;i--)
        printf("%02x", (uint8_t) signature[i]);
    printf("\n");
    //TO FPGA: public key, h[..32], sig[..32]
    if (ge_frombytes_negate_vartime(&A, public_key) != 0) {
        return 0;
    }

    ge_double_scalarmult_vartime(&R, h, &A, signature + 32);
    ge_tobytes(checker, &R);
    //From FPGA: checker [..32]

    printf(" checker :");
    for (i = 256/8 - 1; i >= 0; i--)
        printf("%02x", (uint8_t) checker[i]);

    if (!consttime_equal(checker, signature)) {
        return 0;
    }

    return 1;
}