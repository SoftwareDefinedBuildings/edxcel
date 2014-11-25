
#include <ed25519.h>
#include <global.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sha512.h>
#include <sc.h>


#define RHASH_LEN 32
#define SIGNATURE_LEN 64
#define KEY_LEN 32

#define RHASH_WORDS     (RHASH_LEN/4)
#define SIGNATURE_WORDS (SIGNATURE_LEN/4)
#define KEY_WORDS       (KEY_LEN/4)

#define MSG_LEN 32
#define MAX_TESTS 100

void calc_rhash(const unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key, unsigned char* rhash)
{
    sha512_context hash;
    sha512_init(&hash);
    sha512_update(&hash, signature, 32);
    sha512_update(&hash, public_key, 32);
    sha512_update(&hash, message, message_len);
    sha512_final(&hash, rhash);
    sc_reduce(rhash);
}

uint8_t hw_verify_async(uint8_t *sig, uint8_t *rhash, uint8_t *key)
{
    //Find available EPU
    xl_epu[0].reset = 1;
    xl_epu[0].reset = 0;
    xl_epu[0].reset = 1;
    int i;
    uint32_t result;
    uint32_t epu_idx = *xl_available_idx;
    assert (epu_idx != XL_AVAILABLE_NONE);
    
    //Load signature
    for (i=0;i<SIGNATURE_WORDS;i++)
        xl_epu[epu_idx].sig[i] = ((uint32_t*)&sig[0])[i];
    
    //Load message
    for (i=0;i<RHASH_WORDS;i++)
        xl_epu[epu_idx].rhash[i] = ((uint32_t*)&rhash[0])[i];
        
    //Load key    
    for (i=0;i<KEY_WORDS;i++)
        xl_epu[epu_idx].key[i] = ((uint32_t*)&key[0])[i];
    
    //Start computation
    xl_epu[epu_idx].ctl = XL_GO_CODE;
}

uint8_t hw_verify_wait()
{
    //Wait until calculation is done
    while(!xl_epu[epu_idx].ctl);

    result = xl_epu[epu_idx].ctl;
    //printf("result was: %d\n", result);
    //result = xl_epu[epu_idx].ctl;
    //printf("result was: %d\n", result);
    //Codes are 0 for busy, 1 for sig pass and 2 for sig fail
    return result == 1;
}

// int hexnibble_to_int(char nibble) {
//     if (nibble >= '0' && nibble <= '9')
//         return nibble - '0';
//     if (nibble >= 'A' && nibble <= 'F')
//         return (nibble - 'A') + 10;
//     if (nibble >= 'a' && nibble <= 'f')
//         return (nibble - 'a') + 10;
//     return 0;
// }

void check_sig(uint8_t *sig, uint8_t *key, uint8_t* message, size_t message_len) {
    __attribute__((aligned(4))) uint8_t rhash_arr [32];
    calc_rhash(sig, message, message_len, key, rhash_arr);
    int result_sw = sw_verify_rhash(sig, rhash_arr, key);
    int result_hw = hw_verify_sync(sig, rhash_arr, key);
    if (result_sw == result_hw)
        printf("[PASS] sw=%d hw=%d\n",result_sw, result_hw);
    else
        printf("[FAIL] sw=%d hw=%d\n",result_sw, result_hw);
}

// void ldhex(int bytes, unsigned char* dst, const char* str)
// {
//     int i;
//     for (i = 0; i < bytes;i++)
//         dst[bytes-i - 1] = (hexnibble_to_int(str[i*2]) << 4) +  hexnibble_to_int(str[i*2 + 1]);
// }

void check_hex_sig(const char* sig_big_endian, const char* key_big_endian, const char* rhash_big_endian) {
    __attribute__((aligned(4))) uint8_t sig_arr [64];
    __attribute__((aligned(4))) uint8_t key_arr [32];
    __attribute__((aligned(4))) uint8_t rhash_arr [32];
    int i;
    for (i = 0; i < 64;i++)
        sig_arr[63-i] = (hexnibble_to_int(sig_big_endian[i*2]) << 4) +  hexnibble_to_int(sig_big_endian[i*2 + 1]);
    for (i = 0; i < 32;i++)
        key_arr[31-i] = (hexnibble_to_int(key_big_endian[i*2]) << 4) +  hexnibble_to_int(key_big_endian[i*2 + 1]);
    for (i = 0; i < 32;i++)
        rhash_arr[31-i] = (hexnibble_to_int(rhash_big_endian[i*2]) << 4) +  hexnibble_to_int(rhash_big_endian[i*2 + 1]);
    int result_sw = sw_verify_rhash(sig_arr, rhash_arr, key_arr);
    int result_hw = hw_verify_sync(sig_arr, rhash_arr, key_arr);
    if (result_sw == result_hw)
        printf("[PASS] sw=%d hw=%d\n",result_sw, result_hw);
    else
        printf("[FAIL] sw=%d hw=%d\n",result_sw, result_hw);  
}

// void check_hex_sig_nhash(const char* sig_big_endian, const char* key_big_endian, const char* message_big_endian)
// {
//     __attribute__((aligned(4))) uint8_t sig_arr [64];
//     __attribute__((aligned(4))) uint8_t key_arr [32];
//     __attribute__((aligned(4))) uint8_t message_arr [256];
//     __attribute__((aligned(4))) uint8_t rhash_arr [32];
//     int mlen = strlen(message_big_endian) / 2;
//     ldhex(64, sig_arr, sig_big_endian);
//     ldhex(32, key_arr, key_big_endian);
//     ldhex(mlen, message_arr, message_big_endian);
//     calc_rhash(sig_arr, message_arr, mlen, key_arr, rhash_arr);
//     int result_sw = sw_verify_rhash(sig_arr, rhash_arr, key_arr);
//     int result_hw = hw_verify_sync(sig_arr, rhash_arr, key_arr);
//     if (result_sw == result_hw)
//         printf("[PASS] sw=%d hw=%d\n",result_sw, result_hw);
//     else
//         printf("[FAIL] sw=%d hw=%d\n",result_sw, result_hw);  
// }


unsigned int load(const char *fname, uint8_t **sigs, uint8_t **keys, uint8_t **msgs)
{
    //assume pointers are not NULL
    
    unsigned char str[256];
    FILE *fp = fopen(fname, "r");
    // assume the file exists
    
    unsigned int count = 0;
    int i;
    while (1) {
        unsigned char *pos;

        if (count <= MAX_TESTS)
            break;

        if (fscanf(fp, "%s", str) == EOF)
            break;

        pos = str + 3; // get rid of heading of each line
        for (i = KEY_LEN - 1; i >= 0; i--) {
            sscanf(pos, "%2hhx", &keys[count][j]);
            pos += 2 * sizeof(char);
        }
    
        fscanf(fp, "%s", str); //skip sk
        
        fscanf(fp, "%s", str);
        pos = str + 4;
        for (i = MSG_LEN - 1; i >= 0; i--) {
            sscanf(pos, "%2hhx", &msgs[count][j]);
            pos += 2 * sizeof(char);
        }
        
        fscanf(fp, "%s", str);
        pos = str + 4;
        for (i = SIGNATURE_LEN - 1; i >= 0; i--) {
            sscanf(pos, "%2hhx", &sigs[count][j]);
            pos += 2 * sizeof(char);
        }

        count++;
    }

    return count;
}

void go(const char* fname)
{
    __attribute__((aligned(4))) uint8_t sigs [MAX_TESTS][SIGNATURE_LEN];
    __attribute__((aligned(4))) uint8_t keys [MAX_TESTS][KEY_LEN];
    __attribute__((aligned(4))) uint8_t msgs [MAX_TESTS][MSG_LEN];
    __attribute__((aligned(4))) uint8_t rhashs [MAX_TESTS][RHASH_LEN];
  
    uint8_t results[MAX_TESTS];
    
    unsigned int count = load(fname, keys, msgs, sigs);
    unsigned int i;
    // start timing
    calc_rhash(sigs[0], msgs[0], MSG_LEN, keys[0], rhashs[0]);
    for (i = 0; i < count; i++) {
        hw_verify_async(sigs[i], rhashs[i], keys[i]);
        calc_rhash(sigs[i"], msgs[i], MSG_LEN, keys[i], rhashs[i]);
        results[i] = hw_verify_wait();
    }

    // stop timing
}
