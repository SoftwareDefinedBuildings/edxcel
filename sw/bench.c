
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
#define EPU_IDX 0  // assume only one EPU

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
  return ed25519_verify(sig, msg, MSG_LEN, key);
}


void hw_verify_load(uint8_t *sig, uint8_t *rhash, uint8_t *key)
{
    int i;
    
    //Load signature
    for (i=0;i<SIGNATURE_WORDS;i++)
        xl_epu[EPU_IDX].sig[i] = ((uint32_t*)&sig[0])[i];
    
    //Load message
    for (i=0;i<RHASH_WORDS;i++)
        xl_epu[EPU_IDX].rhash[i] = ((uint32_t*)&rhash[0])[i];
        
    //Load key    
    for (i=0;i<KEY_WORDS;i++)
        xl_epu[EPU_IDX].key[i] = ((uint32_t*)&key[0])[i];
}


void hw_verify_start()
{
    //Start computation
    xl_epu[EPU_IDX].ctl = XL_GO_CODE;
}


uint8_t hw_verify_wait()
{
    uint32_t result;
    
    //Wait until calculation is done
    while(!xl_epu[EPU_IDX].ctl);

    result = xl_epu[EPU_IDX].ctl;
    //printf("result was: %d\n", result);
    //result = xl_epu[epu_idx].ctl;
    //printf("result was: %d\n", result);
    //Codes are 0 for busy, 1 for sig pass and 2 for sig fail
    return result == 1;
}


unsigned int load(const char *fname, uint8_t (*sigs)[SIGNATURE_LEN], uint8_t (*keys)[KEY_LEN], uint8_t (*msgs)[MSG_LEN])
{
    //assume pointers are not NULL
    
    char str[256];
    FILE *fp = fopen(fname, "r");
    // assume the file exists
    
    unsigned int count = 0;
    int i;
    char *pos;
    while (1) {

        if (count >= MAX_TESTS)
            break;

        if (fscanf(fp, "%s", str) == EOF)
            break;

        pos = str + 3; // get rid of heading of each line
        for (i = KEY_LEN - 1; i >= 0; i--) {
            sscanf(pos, "%2hhx", &keys[count][i]);
            pos += 2 * sizeof(char);
        }
    
        fscanf(fp, "%s", str); //skip sk
        
        fscanf(fp, "%s", str);
        pos = str + 4;
        for (i = MSG_LEN - 1; i >= 0; i--) {
            sscanf(pos, "%2hhx", &msgs[count][i]);
            pos += 2 * sizeof(char);
        }
        
        fscanf(fp, "%s", str);
        pos = str + 4;
        for (i = SIGNATURE_LEN - 1; i >= 0; i--) {
            sscanf(pos, "%2hhx", &sigs[count][i]);
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
    __attribute__((aligned(4))) uint8_t rhashs [MAX_TESTS][RHASH_LEN*2]; // RHASH needs twice length space to calculate
  
    uint8_t hw_results[MAX_TESTS];
    uint8_t sw_results[MAX_TESTS];
    
    unsigned int count = load(fname, sigs, keys, msgs);
    unsigned int i;
    clock_t start;
    clock_t stop;

    sw_results[0] = 1;
    // FPGA
    // start timing
     start = clock();
     xl_epu[EPU_IDX].reset = 1;
     xl_epu[EPU_IDX].reset = 0;
     xl_epu[EPU_IDX].reset = 1;
     calc_rhash(sigs[0], msgs[0], MSG_LEN, keys[0], rhashs[0]);
     hw_verify_load(sigs[0], rhashs[0], keys[0]);
     for (i = 0; i < count-1; i++) {
         hw_verify_start();
         calc_rhash(sigs[i+1], msgs[i+1], MSG_LEN, keys[i+1], rhashs[i+1]);
         hw_verify_load(sigs[i+1], rhashs[i+1], keys[i+1]);
         hw_results[i] = hw_verify_wait();
         //printf("HW result %d: %d\n SW result 0: %d\n\n", i, hw_results[i], sw_results[0]);
     }
     hw_verify_start();
     hw_results[i] = hw_verify_wait();
     // printf("HW result %d: %d\n", i, hw_results[i]);
     // stop timing
     stop = clock();
     printf("HW takes %f ms for %u signatures.\n", ((double)(stop - start))/(CLOCKS_PER_SEC/1000), count);
    
    // ARM
    // start timing
    start = clock();
    for (i = 0; i < count; i++) {
        sw_results[i] = sw_verify(sigs[i], msgs[i], keys[i]);
        // printf("SW result %d: %d\n", i, sw_results[i]);
    }
    // stop timing
    stop = clock();
    printf("SW takes %f ms for %u signatures.\n", ((double)(stop - start))/(CLOCKS_PER_SEC/1000), count);

    int match = 1;
    for (i = 0; i < count; i++) {
        if (sw_results[i] != hw_results[i]) {
            match = 0;
            printf("HW (%d) and SW (%d) results don't match at index %d!.\n", hw_results[i], sw_results[i], i);
        }
    }
    if (match)
        printf("HW and SW matchs!");
}
