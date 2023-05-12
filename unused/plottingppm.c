#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>


// DEFINITIONS
// image resolution
#define WIDTH 1000
#define HEIGHT 1000
#define BORDER_SIZE 100
#define PLOT_WIDTH WIDTH - 2*BORDER_SIZE
#define PLOT_HEIGHT HEIGHT - 2*BORDER_SIZE
#define PLOT_SIZE WIDTH - 2*BORDER_SIZE

// file name
#define DEFAULT_OUTPUT_FILE_PATH "plot.ppm"
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
int array_length;
int grid_on = 0;
int g_density = GRID_DENSITY;
const float f_plot_size = PLOT_SIZE;
int width = WIDTH;
int height = HEIGHT;

void draw_background(Colour32 color){
    // fill image array with selected color
    for (size_t y = 0; y < HEIGHT; ++y){
        for (size_t x = 0; x < WIDTH; ++x){
            image[y][x] = color;
        }
    }
}

void draw_border(Colour32 colour){
    for (size_t x = BORDER_SIZE; x < WIDTH - BORDER_SIZE; ++x){
        image[BORDER_SIZE][x] = colour;
        image[HEIGHT - BORDER_SIZE][x] = colour;
    }
    for (size_t y = BORDER_SIZE; y < HEIGHT - BORDER_SIZE; ++y){
        image[y][BORDER_SIZE] = colour;
        image[y][WIDTH - BORDER_SIZE] = colour;
    }
}

void grid(int input_density){
    if (input_density != 0)
        g_density = input_density;
    grid_on = 1;
}

void draw_grid(Colour32 colour){
    int plt_height = PLOT_HEIGHT;
    if (grid_on == 0) // check if grid() has been called
        return;
    for (int i = 1;i < g_density; ++i){
        for (size_t coord = BORDER_SIZE; coord < WIDTH - BORDER_SIZE; ++coord){
            if (coord % 2 == 0)
                continue;
            image[BORDER_SIZE + i*(plt_height/g_density)][coord] = colour;
            image[coord][BORDER_SIZE + i*(plt_height/g_density)] = colour;
        }
    }
}

void save_image_as_png(const char *file_path){

    // convert image array to a file using fprint
    FILE *f = fopen(file_path, "wb"); // pointer to file
    if (f == NULL){
        // error check
        fprintf(stderr, "ERROR could not write into file %s: %s \n", file_path, strerror(errno));
        exit(1);
    }
    fprintf(f, "P6\n%d %d 255\n", WIDTH, HEIGHT); // define image size
    for (size_t y = 0; y < HEIGHT; ++y){
        for (size_t x = 0; x < WIDTH; ++x){
            
            uint32_t ppm_pixel = image[HEIGHT - (y + 1)][x]; // edited to change (0,0) position to be bottom left
            uint8_t bytes[3] = {
                (ppm_pixel&0x0000FF)>>8*0,
                (ppm_pixel&0x00FF00)>>8*1,
                (ppm_pixel&0xFF0000)>>8*2,
            };

            fwrite(bytes, sizeof(bytes), 1, f);
            assert(!ferror(f));
        }
    }
    int ret = fclose(f);
    assert(ret == 0);

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
        xval = ((x[i] - min_x)/(max_x - min_x))*f_plot_size;
        yval = ((y[i] - min_y)/(max_y - min_y))*f_plot_size;
        //printf("%d %d\n", xval, yval);
        for (int j = -DOT_SIZE; j <= DOT_SIZE; ++j){
            for(int k = -DOT_SIZE; k <= DOT_SIZE; ++k){
                image[BORDER_SIZE +  yval + j][BORDER_SIZE +  xval + k] = colour;
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
    save_image_as_png(DEFAULT_OUTPUT_FILE_PATH); // convert image to a ppm output
}
