 #include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

EXPORT  INT usermain(void){
    UW  data;
    SZ  asz;
    ER  err;
    ID dd;

ER set_steer(ID dd, INT steer){
  UW data;
  SZ asz;
  ER err;

  if(steer < -500 || steer > 500) return E_PAR;
  data = steer + 1500;
  err = tk_swri_dev(dd, PMD_DATA_PULSE1, &data, 1, &asz);

  return err;
}

    dd = tk_opn_dev("pmda", TD_UPDATE);
    INT steer;

    data = 14000;
    err = tk_swri_dev(dd, PMD_DATA_PERIOD, &data, 1, &asz);

    data = 14000-1500;
    err = tk_swri_dev(dd, PMD_DATA_PULSE0, &data, 1, &asz);

    data = 1500;
    err = tk_swri_dev(dd, PMD_DATA_PULSE1, &data, 1, &asz);

    data = 1;
    err = tk_swri_dev(dd, PMD_DATA_EN, &data, 1, &asz);

    while(1){
      while(steer < 500){
        steer += 100;
        set_steer(dd,steer);
        tk_dly_tsk(500);
      }
      while(steer > -500){
        steer -= 100;
        set_steer(dd,steer);
        tk_dly_tsk(500);
      }
    }
    
}