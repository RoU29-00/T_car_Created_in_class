#include <tk/tkernel.h>
#include <tm/tmonitor.h>

/* GPIO(Port-E) */
#define PEDATA 		0x400C0400		// Data Register
#define PECR 		0x400C0404		// Output Control register
#define PEIE 		0x400C0438		// Input Control register

INT  usermain(void)
{
 	*(_UW*)PEIE &= ~(1<<2);
 	*(_UW*)PECR |= (1<<2);
	tm_printf("Please open http://127.0.0.1:8888\n");
 	while(1) {
    		*(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(500);
  	}		
}