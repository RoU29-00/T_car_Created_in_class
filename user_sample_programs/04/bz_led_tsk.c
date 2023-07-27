#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// GPIOポートE
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)

// ADC定義
#define ADA_BASE 0x40050000
#define ADACLK  (ADA_BASE + 0x00)
#define ADAMOD0 (ADA_BASE + 0x04)
#define ADAMOD1 (ADA_BASE + 0x08)
#define ADAMOD3 (ADA_BASE + 0x10)
#define ADAMOD4 (ADA_BASE + 0x14)
#define ADAMOD5 (ADA_BASE + 0x18)
#define ADAREG00 (ADA_BASE + 0x34)
#define ADAREG01 (ADA_BASE + 0x38)
#define ADAREG02 (ADA_BASE + 0x3c)
#define ADAREG03 (ADA_BASE + 0x40)

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

// MPT2タイマ
#define	MT2_BASE	(0x400C7200UL)
#define	MT2EN   (MT2_BASE + 0x00)
#define	MT2RUN	(MT2_BASE + 0x04)
#define	MT2IGCR	(MT2_BASE + 0x30)
#define	MT2IGOCR	(MT2_BASE + 0x40)
#define	MT2IGRG2	(MT2_BASE + 0x44)
#define	MT2IGRG3	(MT2_BASE + 0x48)
#define	MT2IGRG4	(MT2_BASE + 0x4C)

void bz_task(INT stacd, void* exinf)
{

  while(1024)
  {

    UW  sw3, sw4;

    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);

      // MTP2初期設定
  *(_UW*)MT2EN     |= 0x81;
  *(_UW*)MT2IGOCR  |= 0x22;
  *(_UW*)MT2IGCR   &= 0x30;  // 0b10110000

  // 端子をMTP2に設定
  *(_UW*)PHFR3   |=  (1 << 2);
  *(_UW*)PHIE    &= ~(1 << 2);
  *(_UW*)PHCR   |=  (1 << 2);
  
  // MTP2 出力波形の設定 
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 18000;
  *(_UW*)MT2IGRG3  = 9000;

  sw3 = *(_UW*)PADATA & (1<<3);

  while (sw3 != 0);
  
  *(_UW*)MT2RUN    |= 0x5;   // ブザー鳴動開始
  
  tk_dly_tsk(1000);

  *(_UW*)MT2RUN    &= 0x2;  // ブザー鳴動停止
  }

}



void led_task(INT stacd, void* exinf)
{
    while(1024)
    {
 	*(_UW*)PEIE &= ~(1<<2);
 	*(_UW*)PECR |= (1<<2);

    UINT d;

    /* ADC */
    *(_UW*)ADACLK  = 0x01;  // クロック供給
    *(_UW*)ADAMOD1 = 0x80;  // ADC有効化
    tk_dly_tsk(1);   // 3マイクロ秒以上待つ
    *(_UW*)ADAMOD3 = 0x01;  // チャンネルスキャンモード
    *(_UW*)ADAMOD4 = 0x00;  // AINA0の1チャンネルのみ

    while (1);
    {
        /* A/D変換を行う */
        *(_UW*)ADAMOD0 = 0x01; // 変換開始
        while ((*(_UW*)ADAMOD5 & (1 << 1)) == 0);  // 変換完了待ち
        d = *(_UW*)ADAREG00 & 12;         // AINA0の変換結果 12ビット分のみを読み出す

        if(d <1000)
        {
             *(_UW*)PEDATA ^= (1<<2); //つけたり、消したり。
        }
    }
        while (1);
    {
        /* A/D変換を行う */
        *(_UW*)ADAMOD0 = 0x01; // 変換開始
        while ((*(_UW*)ADAMOD5 & (1 << 1)) == 0);  // 変換完了待ち
        d = *(_UW*)ADAREG00 & 12;         // AINA0の変換結果 12ビット分のみを読み出す

        if(d >1000)
        {
             *(_UW*)PEDATA ^= (1<<2); //つけたり、消したり。
        }
    }

    }
 
}




/* 初期化処理 */
EXPORT INT usermain(void)
{
    T_CTSK led_ctsk;
    ID led_ctsk;

    led_ctsk.tskatr = TA_HLNG | TA_RNG3;
    led_ctsk.task = led_task;
    led_ctsk.itskpri = 10;
    led_ctsk.stksz = 1024;

    bz_tskid = tk_cre_tsk( &led_ctsk);
    tk_sta_tsk(led_ctsk, 0);

    tk_slp_tsk(TMO_FEVR);   
    
    T_CTSK bz_ctsk;
    ID bz_ctsk;

    bz_ctsk.tskatr = TA_HLNG | TA_RNG3;
    bz_ctsk.task = bz_task;
    bz_ctsk.itskpri = 10;
    bz_ctsk.stksz = 1024;

    bz_tskid = tk_cre_tsk( &bz_ctsk);
    tk_sta_tsk(bz_ctsk, 0);

    tk_slp_tsk(TMO_FEVR);
    
    return 0;      
     
}
