#include <stdio.h>
#include <math.h>
#include "plotting.h"
#define N 5000

int main(void){
    // test plot
    int reps = 15;
    float xarr[N];
    float yarr[N];
    for (int i = 0; i < N; ++i){
        float x = (2*PI*i*reps)/N;
        xarr[i] = i;
        yarr[i] = cos(x/6)*cos(x*PI/6);
    }
    xlabel("radians");
    // ylabel("");
    title("random sinusoidal");
    path("out/myplot.png"); 
    grid(15); 
    plot(xarr,yarr,N); // plot called last
    return 0;
}
