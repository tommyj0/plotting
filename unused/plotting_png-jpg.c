/*
# Usage #

Functions:

plot(float x_values[],float y_values[],int size); - plots arrays, y against x. Pass shared size of arrays.
grid(int); - adds a grid if called before plot(). passed integer indicates the density of grid lines. Passing 0 will default to 10.
path(char file_path[]); - select path and file name ending in .jpg or .png

# Example code #

#include <stdio.h>
#include <math.h>
#include "plotting.c"
#define N 5000

int main(void){
    // test plots
    int reps = 2;
    float x[N];
    float y[N];
    for (int i = 0; i < N; ++i){
        float point = (2*PI*i*reps)/N;
        x[i] = i;
        y[i] = sin(point)*sin(point) + cos(point);
    }
    // plotting.c functions: 
    path("out/myplot.png"); // optional path specification
    grid(7); // optional to add a grid
    plot(x,y,N); // plot(x array, y array, size);
    return 0;
}
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// DEFINITIONS
// image resolution
#define WIDTH 1000
#define HEIGHT 1000
#define BORDER 100
#define BORDER_AREA HEIGHT - 2*BORDER
#define PLOT_BORDER 20
#define PLOT_WIDTH WIDTH - 2*PLOT_BORDER - 2*BORDER
#define PLOT_HEIGHT HEIGHT - 2*PLOT_BORDER - 2*BORDER
#define PLOT_SIZE PLOT_WIDTH
#define CHANNEL_NUM 3

// 
#define DEFAULT_FILE_PATH "plot.png"
#define DOT_SIZE 3
#define GRID_DENSITY 10
#define PI 3.14

// colours in hex 
// 0xAABBGGRR
// standard is RR GG BB 
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0xFF000000
#define COLOR_RED   0xFF0000FF
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE  0xFFFF0000
#define COLOR_PURPLE 0xFFFF00AF
#define COLOR_GREY  0xFFA0A0A0
#define COLOR_DARKGREY 0xFF505050

// useless typedefs
typedef uint32_t Colour32;

// globals
static Colour32 image[HEIGHT][WIDTH]; // define image size
const int width = WIDTH;
const int height = HEIGHT;
int array_length;
int grid_on = 0;
int g_density = GRID_DENSITY;
const float f_plot_size = PLOT_SIZE;
char file_path[] = DEFAULT_FILE_PATH;
const int plot_area = PLOT_SIZE;
const int border_area = BORDER_AREA;


void path(char new_path[]){
    strcpy(file_path, new_path);
}

void grid(int input_density){
    if (input_density != 0)
        g_density = input_density;
    grid_on = 1;
}

void draw_grid(Colour32 colour){
    if (grid_on == 0) // check if grid() has been called
        return;
    for (int i = 1;i < g_density; ++i){
        for (size_t coord = BORDER; coord < WIDTH - BORDER; ++coord){
            if (coord % 2 == 0)
                continue;
            image[BORDER + i*(border_area/g_density)][coord] = colour;
            image[coord][BORDER + i*(border_area/g_density)] = colour;
        }
    }
}

void draw_background(Colour32 color){
    // fill image array with selected color
    for (size_t y = 0; y < HEIGHT; ++y){
        for (size_t x = 0; x < WIDTH; ++x){
            image[y][x] = color;
        }
    }
}

void draw_border(Colour32 colour){
    for (size_t x = BORDER; x < WIDTH - BORDER; ++x){
        image[BORDER][x] = colour;
        image[HEIGHT - BORDER][x] = colour;
    }
    for (size_t y = BORDER; y < HEIGHT - BORDER; ++y){
        image[y][BORDER] = colour;
        image[y][WIDTH - BORDER] = colour;
    }
}
void save_image_as_png(const char *path){
    uint8_t *image_write;
    int image_size = width * height * CHANNEL_NUM;
    image_write = malloc(image_size* sizeof(uint8_t));
    int index = 0;

    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x){
            uint32_t pixel = image[height - (y + 1)][x]; // edited to change (0,0) position to be bottom left
            uint8_t bytes[3] = {
                (pixel&0x0000FF)>>8*0,
                (pixel&0x00FF00)>>8*1,
                (pixel&0xFF0000)>>8*2,
            };

            image_write[index++] = bytes[0];
            image_write[index++] = bytes[1];
            image_write[index++] = bytes[2];
        }
    }
    int p_len = (int)strlen(path);

    if (path[p_len - 4] == '.' && path[p_len - 3] == 'p' && path[p_len - 2] == 'n' && path[p_len - 1] == 'g'){
        stbi_write_png(path, WIDTH, WIDTH, CHANNEL_NUM, image_write, WIDTH * CHANNEL_NUM);
        printf("-- PNG file successfully created and saved as %s --\n", path);
    }
    else if (path[p_len - 4] == '.' && path[p_len - 3] == 'j' && path[p_len - 2] == 'p' && path[p_len - 1] == 'g'){
        stbi_write_jpg(path, WIDTH, HEIGHT, 3, image_write, 100);
        printf("-- JPEG file successfully created and saved as %s --\n", path);
    }
    else {
        printf("ERROR: invalid path - ensure the path string ends in .png or .jpg\n");
        exit(1);
    }

    free(image_write);
}

float max_value(float input_array[]){
    float max = input_array[0];
    for (int i = 0; i < array_length; ++i){
        if (input_array[i] > max)
            max = input_array[i];
    }
    return max;
}

float min_value(float input_array[]){
    float min = input_array[0];
    for (int i = 0; i < array_length; ++i){
        if (input_array[i] < min)
            min = input_array[i];
    }
    return min;
}

void plot_scatter(float x[], float y[], Colour32 colour){
    // plots the given points individually
    int xval,yval;
    for (int i = 0; i < array_length; ++i){
        float min_x = min_value(x);
        float max_x = max_value(x);
        float min_y = min_value(y);
        float max_y = max_value(y);
        if (min_x == max_x)
            xval = plot_area/2;
        else
            xval = ((x[i] - min_x)/(max_x - min_x))*f_plot_size;
        if (min_y == max_y)
            yval = plot_area/2;
        else
            yval = ((y[i] - min_y)/(max_y - min_y))*f_plot_size;

        for (int j = -DOT_SIZE; j <= DOT_SIZE; ++j){
            for(int k = -DOT_SIZE; k <= DOT_SIZE; ++k){
                image[PLOT_BORDER + BORDER + yval + j][PLOT_BORDER + BORDER + xval + k] = colour;
            }
        }
    }
}

/*--------------------------------------------------------------*/ 
/*--------------------MAIN PLOTTING FUNCTION--------------------*/ 
/*--------------------------------------------------------------*/ 

void plot(float xarr[], float yarr[], int size_array){ 
    // input should be of the form - plot(x array, y array, size of array)
    array_length = size_array;
    draw_background(COLOR_GREY); // fill in background 
    draw_border(COLOR_BLACK); // draw a plot area
    draw_grid(COLOR_DARKGREY); // draw a grid if requested
    plot_scatter(xarr,yarr,COLOR_RED); // plot individual points
    save_image_as_png(file_path); // convert image to a ppm output
}
