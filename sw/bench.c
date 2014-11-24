
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
	//printf("result was: %d\n", result);
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

void ldhex(int bytes, unsigned char* dst, const char* str)
{
    int i;
    for (i = 0; i < bytes;i++)
        dst[bytes-i - 1] = (hexnibble_to_int(str[i*2]) << 4) +  hexnibble_to_int(str[i*2 + 1]);
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

void check_hex_sig_nhash(const char* sig_big_endian, const char* key_big_endian, const char* message_big_endian)
{
    __attribute__((aligned(4))) uint8_t sig_arr [64];
    __attribute__((aligned(4))) uint8_t key_arr [32];
    __attribute__((aligned(4))) uint8_t message_arr [256];
    __attribute__((aligned(4))) uint8_t rhash_arr [32];
    int mlen = strlen(message_big_endian) / 2;
    ldhex(64, sig_arr, sig_big_endian);
    ldhex(32, key_arr, key_big_endian);
    ldhex(mlen, message_arr, message_big_endian);
    calc_rhash(sig_arr, message_arr, mlen, key_arr, rhash_arr);
    int result_sw = sw_verify_rhash(sig_arr, rhash_arr, key_arr);
    int result_hw = hw_verify_sync(sig_arr, rhash_arr, key_arr);
    if (result_sw == result_hw)
        printf("[PASS] sw=%d hw=%d\n",result_sw, result_hw);
    else
        printf("[FAIL] sw=%d hw=%d\n",result_sw, result_hw);  
}

void go()
{

    check_hex_sig_nhash("0ef76c68eb6f701bf8cea68374289d7a6f823f838a17146477e8ce3006611eca746f14704962eefd28d5bf77441a4d6cdd3f0616fe7f318ee6f5aa733d738925",
                        "b8359b3533c7c40d843f184684b7818972908097e36a16a1dc3208f53d145f24", //valid
                        "632654daf0a293977294bf5107032bed0f2d515ca2e99d4517d890cab8fe10f0");
    check_hex_sig_nhash("006c2431136dbf3e7db5a0863d48c799d6771ff250d2dee4a68bf2a21beda46801b93ac987e98f90e5abce6ce43804ab89e5b05ee48dafac40a79cb78b815136",
                        "f9fa56295cdcca91ecf68e7f77b6a5c622ca636624c7e05b513fa7a2eafe1a87", //valid
                        "610e1f17fe001434f17666b1c36d547d2138b146a8dd59b42775b3325c67d448");
    check_hex_sig_nhash("0b92dd69a0079e6a20b07f644a944cccec312be5840f41b60a31430d02f919fad01a7c53418b88d9ba70c2511917b0cb5a4e9b26e371508e2e8d8cd48f103159",
                        "821ce3167aa4db1af7ad533ec1ae12dcf8f821dfe1da6f0449a778cf335a7d49", //valid
                        "4beed6159aac305b8dfc58756c9e0896b3c30e600967c51ed1cd9a15734a92c8");
                        
    check_hex_sig_nhash("f5b5d9940978c42a02df9bc34e1699234eee0118fbf61a0e82a7871a37fe6c84938121d92a3d050e325f13709795a4540da332916ea394236f41d93749b8c801",
                        "119fed26bea9a0d8be69441d7b6851f741835d5ad7cb7f7e51f0bb5ce2240502", //invalid
                        "b06047a0fc505e616ade408cb528baeaa4b524dc77fc9baee7189132ba6d3854");
    check_hex_sig_nhash("098cc6e66900cc15e336be3c456c630c89160a2c1b532e1dc6f9f294ed11937e0bf66c3ddc562d8db08950e19fe48fd6c1499bff1a15d8f9373218a901f49849",
                        "4ddf1dc888c612243ddd5dfbac7b5bc78a4a9b1ca2447d3ffce8807e06a65ed7", //invalid
                        "50107d46a5c6c7f57fce32ce8e659ade70121aca2b72867b3290d937faa0aa58");
    check_hex_sig_nhash("0f373a1ba87dc97b0d8ac1b10cd6117fc875599aec8cb16e3aeb74999758347dd68e11dc8ef47147e0548aeb4c44aeb8c5b3b4baccbfa201ff3f9d8c0d84b642",
                        "1cd184f37ee0e4bd6cc596bf2b22f600411d910c3a00b0296f2c49a2397b7f11", //valid
                        "58cf48bd9b36432ee396c13c166ee0e898dda21b3595507da5fc4df364147510");

    //correct
    check_hex_sig("0e722c657a46123fd9e362f592424e7922262adddec344dfcef6414133fc022b1715bd6c30b45f864fa49da3e3abf950cf2a61795c2e9e830b6a009effdffff2",
                  "973ddda3cda397385e46a3bc714e2f1572e3ea6eb09667e3604520ce9638bf06",
                  "070ff3673edd6d54f00317d8c7c68b4f8dfbcf400a61fae3c24bcbfda1fb914a");
    //invalid       
    check_hex_sig("0e722c657a46123fd9e362f592424e7922262adddec344dfcef6414133fc022b1715bd6c30b45f864fa49da3e3abf950cf2a61795c2e9e830b6a009effdffffe",
                  "973ddda3cda397385e46a3bc714e2f1572e3ea6eb09667e3604520ce9638bf06",
                  "070ff3673edd6d54f00317d8c7c68b4f8dfbcf400a61fae3c24bcbfda1fb914a");
}


