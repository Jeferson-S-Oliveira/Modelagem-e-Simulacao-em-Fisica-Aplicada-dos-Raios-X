#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(void) {
    int v[30], *vptr, i;
    
    printf("v=%p\n",v);
    
    vptr = v;
    printf("vptr=%p\n",vptr);

    for ( i=0; i<30; i++ ) *(vptr+i)=0; // vptr[i] = v[i] = 0

    *(vptr+0) = 10; // vptr[0] = v[0] = 10
    *(v+20) = -10; // v[20] = v[20] = -10
    for ( i=0; i<30; i++ )
        printf("vptr[%d]=%d\n",i,vptr[i]);
     
    return 0;
}
