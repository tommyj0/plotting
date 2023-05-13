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
        float point = (2*PI*i*reps)/N;
        xarr[i] = i;
        yarr[i] = 2 + (-sin(point) + cos(point)*cos(point))*i;
    }
    xlabel("radians");
    ylabel("");
    title("random sinusoidal");
    path("out/myplot.png"); 
    grid(15); 
    plot(xarr,yarr,N); // plot called last
    return 0;
}
