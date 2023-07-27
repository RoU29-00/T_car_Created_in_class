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
ID mbfid;
T_CFLG cflg;

    typedef struct 
    {
        INT sw;
        SYSTIM time;
    }T_MSG_SW;

void task(INT stacd, void* exinf)
{
    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    *(_UW*)PECR &= ~(1<<7);
    *(_UW*)PEIE |= (1<<7);

    *(_UW*)PECR |= (1<<3);
    *(_UW*)PEIE &= ~(1<<2);
 	*(_UW*)PECR |= (1<<2);

    T_MSG_SW sws;
    while(1)
    {
        tk_rcv_mbf(mbfid, &sws, TMO_FEVR);
        
        if(sws.sw == 3)
        {
    			*(_UB*)PEDATA |= (1<<3);
		        tk_dly_tsk(1000);
                *(_UB*)PEDATA &= ~(1<<3);
                tm_printf(*sws.time);
        }
        else if(sws.sw == 4)
        {
                *(_UB*)PEDATA ^= (1<<2);
		        tk_dly_tsk(1000);
                *(_UB*)PEDATA ^= (1<<2);
                tm_printf(*sws.time);
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
    

    //message
    T_CMBF cmbf;

    cmbf.mbfatr = TA_TFIFO;
    cmbf.bufsz = sizeof(INT) * 10;
    cmbf.maxmsz = sizeof(INT);

    mbfid = tk_cre_mbf(&cmbf);


    while(1)
    {
        sw3 = *(_UW*)PADATA & (1<<3);
        tm_printf("SW3 = %d\n", sw3);
        if(sw3 == 0)
        {

            T_MSG_SW sws;
            sws.sw = 3;
            tk_get_tim(&sws.time);
            tk_snd_mbf(mbfid,&sws, sizeof(sws), TMO_FEVR);
        }
        sw4 = *(_UW*)PEDATA & (1<<7);
        tm_printf("SW4 = %d\n", sw4);
        if(sw4 == 0)
        {
            T_MSG_SW sws;
            sws.sw = 3;
            tk_get_tim(&sws.time);
            tk_snd_mbf(mbfid, &sws, sizeof(sws), TMO_FEVR);
        }
        tk_dly_tsk(10);
    }

}
