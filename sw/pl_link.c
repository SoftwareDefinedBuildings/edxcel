
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>     
#include <global.h>  
char* banner = "\033[34;1m"
"  ______    ___   __        _ \n"
" |  ____|  | \\ \\ / /       | |\n"
" | |__   __| |\\ V / ___ ___| |\n"
" |  __| / _` | > < / __/ _ \\ |\n"
" | |___| (_| |/ . \\ (_|  __/ |\n"
" |______\\__,_/_/ \\_\\___\\___|_|\n";   
                           
uint32_t *PL;

#define PL_ADDR 0x43C00000
#define PL_SIZE 0x10000
uint32_t pl_base;

uint32_t volatile *  xl_version;
uint32_t volatile *  xl_opts;
uint32_t volatile *  xl_ready_flags;
uint32_t volatile *  xl_available_idx;
uint32_t volatile *  xl_signok_flags;
epu_t volatile *  xl_epu;

void initialize_pl()
{

    /* Open /dev/mem file */
	int fd = open ("/dev/mem", O_RDWR);
	if (fd < 1) 
	{
		perror("Could not open /dev/mem");
		exit(1);
	}
	PL = mmap(NULL, PL_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PL_ADDR);
	if (!PL) 
	{
	    printf("Could not map PL\n");
	    exit(1);
	}
	
	pl_base = (uint32_t) PL;
	xl_version          = (uint32_t volatile * )(pl_base + 0x0000);
	xl_opts             = (uint32_t volatile * )(pl_base + 0x0004);
	xl_ready_flags      = (uint32_t volatile * )(pl_base + 0x0010);
	xl_available_idx    = (uint32_t volatile * )(pl_base + 0x0014);
	xl_signok_flags     = (uint32_t volatile * )(pl_base + 0x0014);
	xl_epu              = (epu_t volatile * )(pl_base + 0x2000);
	
	printf("PLBASE is %x\n",(uint32_t)PL);
	printf("PLBASE is %x\n",(uint32_t)pl_base);
	printf("XL_EPU is %x\n",(uint32_t)xl_epu);
	printf("GO[0] is %x  (%x)\n",(uint32_t)(&xl_epu[0].ctl), (uint32_t)(&xl_epu[0].ctl) - pl_base);
	uint32_t v = *xl_version;
	if (v >> 16 == 0x0410)
	{
	    printf("%sPL linked (%d EPU's), version 0x%08x\n\033[0m",banner, *xl_opts & 0xFF, v);
	}
	
}

