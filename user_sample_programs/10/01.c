#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define HEADLIGHT_ADR 0x18

EXPORT INT usermain(void)
{
    ID dd;
    UB data[2];
    SZ asz;
    ER err;

    data[0] = 0; //mode register1
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 1; //mode register2
    data[1] = 0x05;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 0x0c; //LED right
    data[1] = 0x2a;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0] = 0x0d; //LED left
    data[1] = 0x2a;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);  

    INT uint;
    UB red, green, blue;

    red = 255, green = 0, blue = 0;
    for(uint = 0; uint < 2 ; uint++)
    {
        data[0] = (uint ? 0x02 : 0x06);
        data[1] = red;
        err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

        data[0]++;
        data[1] = green;
        err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

        data[0]++;
        data[1] = blue;
        err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    }
    
    return 0;
}