#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdint.h>

void initialize_pl();
extern uint32_t PL_BASE;


typedef volatile struct
{
    uint32_t sig        [16];
    uint32_t _reserved1 [16];
    uint32_t key        [8];
    uint32_t rhash      [8];
    uint32_t ctl;
    uint32_t reset;
    uint32_t _reserved2 [14];
} __attribute__((__packed__)) epu_t;

extern uint32_t volatile *  xl_version;
extern uint32_t volatile *  xl_opts;
extern uint32_t volatile *  xl_ready_flags;
extern uint32_t volatile *  xl_available_idx;
extern uint32_t volatile *  xl_signok_flags;
extern epu_t volatile *  xl_epu;

#define XL_AVAILABLE_NONE	0xFF
#define XL_GO_CODE 			0x410

#endif
