#include <stdio.h>
#include <math.h>
#include "plotting.h"
#define N 5000

int main(void){
    // test plot
    int reps = 10;
    float xarr[N];
    float yarr[N];

    for (int i = 0; i < N; ++i){
        float x = (2*PI*(i - N/2)*reps)/N;
        xarr[i] = i;
        if (x != 0)
            yarr[i] = 1/cosh(x); 
    }

    xlabel("testing new");
    // ylabel("");
    title("this is a graph");
    path("out/myplot.png"); 
    grid(15); 
    plot(xarr,yarr,N); // plot called last
    return 0;
}
