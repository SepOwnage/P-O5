#include <stdio.h>
#include "globals.h"
#include "quantize.h"

int main()
{
    unsigned short buffer[3] = {0,0,0};
    struct parameters params = {1,1,3};
    struct start_values starts = {0,1,0,0,buffer};
    short input[5] = {20,100,400,3000,2};
    short output[5];
    quantize(output, input,&params, &starts);
    for(int j = 0; j < 5; j++) {
        printf("%d ", output[j]);
    }
    return 0;
}

