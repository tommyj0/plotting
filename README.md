# Usage 

## Functions
```c
xlabel(char[] xlabel_text) // adds a label to the x axis. Passing an empty string will default to "x-axis"
ylabel(char[] ylabel_text) // adds a label to the y axis. Passing an empty string will default to "y-axis"
title(char[] title_text) // adds a title. Passing an empty string will default to "plot title"
grid(int grid_density) // adds a grid. Passed integer indicates the density of grid lines. Passing 0 will default to 10.
path(char file_path[]) // select path and file name ending in .jpg or .png
plot(float x_values[],float y_values[],int size) // plots arrays, y against x. Pass shared size of arrays.
```

## Example code 
```c
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
        yarr[i] = 2 + (sin(point) + cos(point)*cos(point))*i;
    }
    xlabel("radians");
    ylabel("");
    title("");
    path("out/myplot.jpg"); // optional path specification
    grid(15); // optional to add a grid
    plot(xarr,yarr,N); // plot(x array, y array, size);
    return 0;
}
```