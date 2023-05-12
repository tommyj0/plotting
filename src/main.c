#include <stdio.h>
#include <math.h>
#include "plotting.c"
#define N 5000

int main(void){
    // test plots
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
    path("out/myplot.jpg"); // optional path specification
    grid(15); // optional to add a grid
    plot(xarr,yarr,N); // plot(x array, y array, size);
    return 0;
}
