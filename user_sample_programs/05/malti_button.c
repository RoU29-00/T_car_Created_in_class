#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

/* GPIO(Port-E) */
#define PEDATA 		0x400C0400		// Data Register
#define PECR 		0x400C0404		// Output Control register
#define PEIE 		0x400C0438		// Input Control register

void task(INT stacd, void* exinf)
{
    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    //winker port
 	*(_UW*)PEIE &= ~(1<<2);
 	*(_UW*)PECR |= (1<<2);

    while(1)
    {

        tk_slp_tsk(TMO_FEVR);

        *(_UW*)PEDATA |= (1<<2);
        tk_dly_tsk(1000);
        *(_UW*)PEDATA &= ~(1<<2);

    }

}


/* 初期化処理 */
EXPORT INT usermain(void)
{
    UW  sw3;

    T_CTSK taskA;
    ID task_ID;

    taskA.tskatr = TA_HLNG | TA_RNG3;
    taskA.task = task;
    taskA.itskpri = 10;
    taskA.stksz = 1024;

    task_ID = tk_cre_tsk( &taskA);
    tk_sta_tsk(task_ID, 0);

    while(1)
    {
        sw3 = *(_UW*)PADATA & (1<<3);
        tm_printf("SW3 = %d\n", sw3);
        if(sw3 == 0)
        {
            tk_wup_tsk(task_ID);
        }
        tk_dly_tsk(100);
    }

    return 0;      // 本関数は終了しない
}
