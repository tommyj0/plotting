#include <stdio.h>
#include <math.h>
#include "plotting.c"

#define N 5000

int main(void){
    // test plot
    int reps = 1;
    float xarr[N];
    float yarr[N];
    for (int i = 0; i < N; ++i){
        float point = (2*PI*i*reps)/N - PI;
        xarr[i] = i;
        yarr[i] = 1.0f / (1.0f + expf(2*point));
    }
    xlabel("radians");
    // ylabel("");
    title("random sinusoidal");
    path("out/myplot.png"); 
    grid(15); 
    plot(xarr,yarr,N); // plot called last
    return 0;
}
