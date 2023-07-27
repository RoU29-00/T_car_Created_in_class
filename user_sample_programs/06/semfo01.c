 #include <tk/tkernel.h>
#include <tm/tmonitor.h>

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

/* GPIO(Port-E) */
#define PEDATA 		0x400C0400		// Data Register
#define PECR 		0x400C0404		// Output Control register
#define PEIE 		0x400C0438		// Input Control register

void task_01(INT stacd, void* exinf)
{
    tk_slp_tsk(TMO_FEVR);
    int  i = 0;
 	*(_UW*)PEIE &= ~(1<<2);
 	*(_UW*)PECR |= (1<<2);
	while(i <= 5) 
    {
    	*(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(100);
        *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(100);
        *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(100);
        *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(100);
        i++;
  	}		

}

void task_05(INT stacd, void* exinf)
{
    tk_slp_tsk(TMO_FEVR);
    int  m = 0;
 	*(_UW*)PEIE &= ~(1<<2);
 	*(_UW*)PECR |= (1<<2);
	while(m <= 5) 
    {
    	*(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(500);
        *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(500);
        *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(500);
        *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(500);
        m++;
  	}		

}

void task_10(INT stacd, void* exinf)
{
    tk_slp_tsk(TMO_FEVR);
    int  n = 0;
 	*(_UW*)PEIE &= ~(1<<2);
 	*(_UW*)PECR |= (1<<2);
	while(n <= 5) 
    {
    	*(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(1000);
        *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(1000);
        *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(1000);
        *(_UW*)PEDATA ^= (1<<2);
		tk_dly_tsk(1000);
        n++;
  	}		

}

EXPORT INT usermain(void)
{
    UW  sw3;
    // button
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    *(_UW*)PECR &= ~(1<<7);
    *(_UW*)PEIE |= (1<<7);

    //task managemant01
    T_CTSK task01;
    ID task_ID_01;

    task01.tskatr = TA_HLNG | TA_RNG3;
    task01.task = task_01;
    task01.itskpri = 10;
    task01.stksz = 1024;

    task_ID_01 = tk_cre_tsk( &task01);
    tk_sta_tsk(task_ID_01, 0);

    //task managemant05
    T_CTSK task05;
    ID task_ID_05;

    task05.tskatr = TA_HLNG | TA_RNG3;
    task05.task = task_05;
    task05.itskpri = 10;
    task05.stksz = 1024;

    task_ID_05 = tk_cre_tsk( &task05);
    tk_sta_tsk(task_ID_05, 0);

    //task managemant10
    T_CTSK task10;
    ID task_ID_10;

    task10.tskatr = TA_HLNG | TA_RNG3;
    task10.task = task_10;
    task10.itskpri = 10;
    task10.stksz = 1024;

    task_ID_10 = tk_cre_tsk( &task10);
    tk_sta_tsk(task_ID_10, 0);

    while(1)
    {
        sw3 = *(_UW*)PADATA & (1<<3);
        tm_printf("SW3 = %d\n", sw3);
        {
            tk_wup_tsk(task_ID_01);
            tk_wup_tsk(task_ID_05);
            tk_wup_tsk(task_ID_10);
        }
        tk_dly_tsk(100);
    }

    return 0;      // 本関数は終了しない
}