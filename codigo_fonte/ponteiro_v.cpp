#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(void) {
    int v[30], *vptr;
    printf("v=%p\n",v);
    vptr = v;
    printf("vptr=%p\n",vptr);

    *vptr = 10; // == *v == *(v+0) = v[0]
    printf("v[0]=%d\n",v[0]);
    *(vptr+20) = -10; // == *(v+20) = v[20]
    printf("v[20]=%d\n",v[20]);
    return 0;
}
