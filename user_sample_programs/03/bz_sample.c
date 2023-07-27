#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// GPIOポートH
#define GPIOH_BASE (0x400C0700)
#define PHCR    (GPIOH_BASE + 0x04)
#define PHFR3   (GPIOH_BASE + 0x10)
#define PHIE    (GPIOH_BASE + 0x38)

// MPT2タイマ
#define	MT2_BASE	(0x400C7200UL)
#define	MT2EN   (MT2_BASE + 0x00)
#define	MT2RUN	(MT2_BASE + 0x04)
#define	MT2IGCR	(MT2_BASE + 0x30)
#define	MT2IGOCR	(MT2_BASE + 0x40)
#define	MT2IGRG2	(MT2_BASE + 0x44)
#define	MT2IGRG3	(MT2_BASE + 0x48)
#define	MT2IGRG4	(MT2_BASE + 0x4C)


EXPORT	INT	usermain( void )
{
  // MTP2初期設定
  *(_UW*)MT2EN     |= 0x81;
  *(_UW*)MT2IGOCR  |= (1 << 1);
  *(_UW*)MT2IGOCR  &= ~(1 << 5);
  *(_UW*)MT2IGCR   &= 0x30;   //??

  // 端子をMTP2に設定
  *(_UW*)PHFR3   |=  (1 << 2);
  *(_UW*)PHIE    &= ~(1 << 2);
  *(_UW*)PHCR    |=  (1 << 2);
  
  // MTP2 出力波形の設定 
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 18000;
  *(_UW*)MT2IGRG3  = 9000;

  *(_UW*)MT2RUN    |= 0x05;   // ブザー鳴動開始
  
  tk_dly_tsk(1000);

  *(_UW*)MT2RUN    &= 0x02;  // ブザー鳴動停止

  return 0;
}