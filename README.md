# Usage 

## Functions

```c
xlabel(char[] xlabel_text) // optional - adds a label to the x axis, defaults to "x-axis"
ylabel(char[] ylabel_text) // optional - adds a label to the y axis, defaults to "y-axis"
title(char[] title_text) // optional - adds a plot title, defaults to "plot title"
grid(int grid_density) // optional - adds a grid in the density of the passed integer, defaults to 10
path(char file_path[]) // optional - set the file path and name, defaults to "plot.png"
plot(float x_values[],float y_values[],int size) // plots arrays, y against x, pass shared size of arrays
```

## Example code 

```c
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
```

## Compiling

Copy the include folder and run the following: 
```bash
gcc -Wall -Wextra -o [OUTPUT PATH] [FILE PATH].c -lm -std=c17 -I include 

.\bin\main
```