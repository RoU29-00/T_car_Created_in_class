 /*
 *	usermain.c (usermain)
 *	User Main
 */
#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define PEDATA 		0x400C0400		// Data Register
#define PECR 		0x400C0404		// Output Control register
#define PEIE 		0x400C0438		// Input Control register


EXPORT INT usermain( void )
{

   	_UW i;

 	*(_UW*)PECR |= (1<<3);
	tm_printf("Please open http://127.0.0.1:8888\n");
        int q = 0;

 	while(q < 3) {
    		*(_UW*)PEDATA |= (1<<3);
		tk_dly_tsk(500);
                *(_UW*)PEDATA &= ~(1<<3);
		tk_dly_tsk(500);

                *(_UW*)PEIE &= ~(1<<2);
 	        *(_UW*)PECR |= (1<<2);
                *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(500);
                *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(500);

                q++;
  	}		


	return 0;
}
