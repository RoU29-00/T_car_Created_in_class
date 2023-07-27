 #include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

// GPIOポートE
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)

ID sw_flgid;
T_CFLG cflg;

void task(INT stacd, void* exinf)
{
    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    *(_UW*)PECR &= ~(1<<7);
    *(_UW*)PEIE |= (1<<7);

    UINT flg = 0;
    while(1)
    {

        tk_wai_flg(sw_flgid, (1 << 0)|(1 << 1), (TWF_ANDW | TWF_BITCLR), &flg, TMO_FEVR);

        if(flg & (1 << 0))
        {
            tm_printf("MMMMMMMMMMMMMMMMMMMMM");
            *(_UW*)PECR |= (1<<3);
    			*(_UB*)PEDATA |= (1<<3);
		        tk_dly_tsk(1000);
                *(_UB*)PEDATA &= ~(1<<3);
        }
        else
        {
         tm_printf("MMMMMMMMMMMMMMMMMMMMM");
            *(_UW*)PEIE &= ~(1<<2);
 	        *(_UW*)PECR |= (1<<2);
                *(_UB*)PEDATA ^= (1<<2);
		tk_dly_tsk(500);
                *(_UB*)PEDATA ^= (1<<2);
        }


    }

}


/* 初期化処理 */
EXPORT INT usermain(void)
{
    UW  sw3, sw4;

    //task management
    T_CTSK taskA;
    ID task_ID;

    taskA.tskatr = TA_HLNG | TA_RNG3;
    taskA.task = task;
    taskA.itskpri = 10;
    taskA.stksz = 1024;

    task_ID = tk_cre_tsk( &taskA);
    tk_sta_tsk(task_ID, 0);
    
    //flag management


    cflg.flgatr =  TA_WMUL | TA_TFIFO;
    cflg.iflgptn = 0;

    sw_flgid = tk_cre_flg( &cflg);

    while(1)
    {
        sw3 = *(_UW*)PADATA & (1<<3);
        tm_printf("SW3 = %d\n", sw3);
        if(sw3 == 0)
        {
            tk_set_flg(sw_flgid, ( 1 << 0));
        }
        sw4 = *(_UW*)PEDATA & (1<<7);
        tm_printf("SW4 = %d\n", sw4);
        if(sw4 == 0)
        {
            tk_set_flg(sw_flgid, ( 1 << 1));
        }
        tk_dly_tsk(100);
    }

    return 0;      // 本関数は終了しない
}
