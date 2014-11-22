
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

#define RHASH_WORDS 	(RHASH_LEN/4)
#define SIGNATURE_WORDS (SIGNATURE_LEN/4)
#define KEY_WORDS 		(KEY_LEN/4)

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

//Return 1 if sig is ok, 0 if sig fails
inline int sw_verify(uint8_t *sig, uint8_t *msg, uint8_t *key)
{
	return ed25519_verify(sig, msg, 32, key);
}

inline int sw_verify_rhash(uint8_t *sig, uint8_t *rhash, uint8_t *key)
{
	return ed25519_verify_rhash(sig, rhash, key);
}

uint8_t hw_verify_sync(uint8_t *sig, uint8_t *rhash, uint8_t *key)
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

	//Wait until calculation is done
	while(!xl_epu[epu_idx].ctl);

	result = xl_epu[epu_idx].ctl;
	printf("result was: %d\n", result);
	//result = xl_epu[epu_idx].ctl;
	//printf("result was: %d\n", result);
	//Codes are 0 for busy, 1 for sig pass and 2 for sig fail
	return result == 1;
}

int hexnibble_to_int(char nibble) {
    if (nibble >= '0' && nibble <= '9')
        return nibble - '0';
    if (nibble >= 'A' && nibble <= 'F')
        return (nibble - 'A') + 10;
    if (nibble >= 'a' && nibble <= 'f')
        return (nibble - 'a') + 10;
    return 0;
}
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


void go()
{
    //correct
    check_hex_sig("0e722c657a46123fd9e362f592424e7922262adddec344dfcef6414133fc022b1715bd6c30b45f864fa49da3e3abf950cf2a61795c2e9e830b6a009effdffff2",
                  "973ddda3cda397385e46a3bc714e2f1572e3ea6eb09667e3604520ce9638bf06",
                  "070ff3673edd6d54f00317d8c7c68b4f8dfbcf400a61fae3c24bcbfda1fb914a");
    //invalid       
    check_hex_sig("0e722c657a46123fd9e362f592424e7922262adddec344dfcef6414133fc022b1715bd6c30b45f864fa49da3e3abf950cf2a61795c2e9e830b6a009effdffffe",
                  "973ddda3cda397385e46a3bc714e2f1572e3ea6eb09667e3604520ce9638bf06",
                  "070ff3673edd6d54f00317d8c7c68b4f8dfbcf400a61fae3c24bcbfda1fb914a");
}


