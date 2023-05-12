/*
# Usage #

Functions:

xlabel(char[]) - adds a label to the x axis. Passing an empty string will default to "x-axis"
ylabel(char[]) - adds a label to the y axis. Passing an empty string will default to "y-axis"
title(char[]) - adds a title. Passing an empty string will default to "plot title"
grid(int) - adds a grid. Passed integer indicates the density of grid lines. Passing 0 will default to 10.
path(char file_path[]) - select path and file name ending in .jpg or .png
plot(float x_values[],float y_values[],int size) - plots arrays, y against x. Pass shared size of arrays.

# Example code #

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
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <stdbool.h>

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
#define CHANNEL_NUM 3
#define PI 3.14

// defaults
#define DEFAULT_FILE_PATH "plot.png"
#define DEFAULT_TITLE "plot title"
#define DEFAULT_X_LABEL "x-axis"
#define DEFAULT_Y_LABEL "y-axis"
#define DOT_SIZE 3
#define GRID_DENSITY 10

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
const float f_plot_size = PLOT_WIDTH;
char file_path[] = DEFAULT_FILE_PATH;
char plot_title[40] = DEFAULT_TITLE;
char plot_xlabel[40] = DEFAULT_X_LABEL;
char plot_ylabel[40] = DEFAULT_Y_LABEL;
const int plot_area = PLOT_WIDTH;
const int border_area = BORDER_AREA;
const int label_font_size = 4;
const int title_font_size = 6;


typedef struct {
    size_t width, height;
    const char *glyphs;
} Font;

#define FONT_HEIGHT 6
#define FONT_WIDTH 6
static char default_glyphs[128][FONT_HEIGHT][FONT_WIDTH] = {
    ['a'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
    },
    ['b'] = {
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 0, 0},
    },
    ['c'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['d'] = {
        {0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
    },
    ['e'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
    },
    ['f'] = {
        {0, 0, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
    },
    ['g'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['h'] = {
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
    },
    ['i'] = {
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    ['j'] = {{0}},
    ['k'] = {
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 1, 0, 1, 0},
    },
    ['l'] = {
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
    },
    ['m'] = {
        {0, 0, 0, 0, 0},
        {1, 1, 0, 1, 0},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
    },
    ['n'] = {
        {0, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
    },
    ['o'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['p'] = {
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    ['q'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
    },
    ['r'] = {
        {0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    ['s'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {1, 1, 1, 0, 0},
    },
    ['t'] = {
        {0, 1, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
    },
    ['u'] = {
        {0, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
    },
    ['v'] = {
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
    },    
    ['w'] = {
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {0, 1, 1, 1, 1},
    },
    ['x'] = {
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 0, 0, 1},
    },    
    ['y'] = {
        {0, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },  
    ['z'] = {{0}},

    ['A'] = {{0}},
    ['B'] = {{0}},
    ['C'] = {{0}},
    ['D'] = {{0}},
    ['E'] = {{0}},
    ['F'] = {{0}},
    ['G'] = {{0}},
    ['H'] = {{0}},
    ['I'] = {{0}},
    ['J'] = {{0}},
    ['K'] = {{0}},
    ['L'] = {{0}},
    ['M'] = {{0}},
    ['N'] = {{0}},
    ['O'] = {{0}},
    ['P'] = {{0}},
    ['Q'] = {{0}},
    ['R'] = {{0}},
    ['S'] = {{0}},
    ['T'] = {{0}},
    ['U'] = {{0}},
    ['V'] = {{0}},
    ['W'] = {{0}},
    ['X'] = {{0}},
    ['Y'] = {{0}},
    ['Z'] = {{0}},

    ['0'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['1'] = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
    },
    ['2'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
    },
    ['3'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['4'] = {
        {0, 0, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
    },
    ['5'] = {
        {1, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['6'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['7'] = {
        {1, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
    },
    ['8'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},

    },
    ['9'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },

    [','] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
    },

    ['.'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
    },
    ['-'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
};

// static Font default_font = {
//     .glyphs = &default_glyphs[0][0][0],
//     .width = FONT_WIDTH,
//     .height = FONT_HEIGHT,
// };

void xlabel(const char text[]){
    // function to be called adding an x-axis label
    if (text[0] != '\0')
        strcpy(plot_xlabel,text);
}

void ylabel(const char text[]){
    // function to be called adding a y-axis label
    if (text[0] != '\0')
        strcpy(plot_ylabel,text);
}

void title(const char text[]){
    // function to be called adding a title
    if (text[0] != '\0')
        strcpy(plot_title,text);
}

void path(char new_path[]){
    // function to be called to change the path and file name
    strcpy(file_path, new_path);
}

void grid(int input_density){
    // function to be called to change the grid density
    if (input_density != 0)
        g_density = input_density;
    grid_on = 1;
}

void draw_grid(Colour32 colour){
    // drawing the grid according to the given density
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
    // converting the 2D array into a 1D array that can be converted to a png/jpg file
    uint8_t *image_write;
    int image_size = width * height * CHANNEL_NUM;
    image_write = malloc(image_size* sizeof(uint8_t));
    int index = 0;

    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x){
            uint32_t pixel = image[y][x]; 
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
    char jpg[] = ".jpg";
    char png[] = ".png";
    int i, j;
    for (i = 0; i < 4; ++i){
        if (path[p_len  + i - 4] != jpg[i])
            break;
    }
    if (i == 4){
        stbi_write_jpg(path, WIDTH, HEIGHT, 3, image_write, 100);
        printf("-- JPEG file successfully created and saved as %s --\n", path);
        free(image_write);
        return;
    }

    for (j = 0; j < 4; ++j){
        if (path[p_len + j - 4] != png[j])
            break;
    }
    if (j == 4){
        stbi_write_png(path, WIDTH, WIDTH, CHANNEL_NUM, image_write, WIDTH * CHANNEL_NUM);
        printf("-- PNG file successfully created and saved as %s --\n", path);
        free(image_write);
        return;
    }
    printf("ERROR: invalid path - ensure the path string ends in .png or .jpg\n");
    exit(1);
}

float max_value(float input_array[]){
    // finding the maximum value of the given array
    float max = input_array[0];
    for (int i = 0; i < array_length; ++i){
        if (input_array[i] > max)
            max = input_array[i];
    }
    return max;
}

float min_value(float input_array[]){
    // finding the minimum value of the given array
    float min = input_array[0];
    for (int i = 0; i < array_length; ++i){
        if (input_array[i] < min)
            min = input_array[i];
    }
    return min;
}

void length_check(int len, char spec[]){
    // check the length of a string and through an error/warning if needed
    int max_len = 33;
    int shit_length = 25;
    if (len > max_len){
        printf("ERROR: %s is too long - shorten the string to less than %d\n",spec, max_len);
        exit(1);
    }
    else if (len > shit_length){
        printf("WARNING: %s is kinda long - shorten it so it doesn't look like shit\n", spec);
    }
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
                image[HEIGHT - PLOT_BORDER - BORDER - yval + j][PLOT_BORDER + BORDER + xval + k] = colour;
            }
        }
    }
}

void add_text(const char *xlabel, const char *ylabel, const char *title){
    // function draws all the required text
    // adding an x-axis label
    int ypos, xpos;
    int label_len = (int)strlen(xlabel);
    length_check(label_len, "x-axis label");

    ypos = 920;
    xpos = 500 - (label_len*label_font_size*6)/2;
    for (int i = 0; i < label_len; ++i, xpos += 6*label_font_size){
        for (int x = 0; x < 5*label_font_size; ++x){
            for (int y = 0; y < 6*label_font_size; ++y){
                if (default_glyphs[(unsigned)*(xlabel + i)][y/label_font_size][x/label_font_size])
                    image[y + ypos][x + xpos] = COLOR_BLACK;
            }   
        }
    }
    // adding a y-axis label
    label_len = (int)strlen(ylabel);
    length_check(label_len, "y-axis label");

    ypos = 500 + (label_len*label_font_size*5)/2;
    xpos = 50;
    for (int i = 0; i < label_len; ++i, ypos -= 6*label_font_size){
        for (int x = 0; x < 6*label_font_size;++x){
            for (int y = 0; y < 5*label_font_size;++y){
                if (default_glyphs[(unsigned)*(ylabel + i)][x/label_font_size][4 - y/label_font_size])
                    image[ypos + y][xpos + x] = COLOR_BLACK;
            }   
        }
    }
    // adding a title 
    label_len = (int)strlen(title);
    length_check(label_len, "title");

    ypos = 50;
    xpos = 500 - (label_len*title_font_size*6)/2;
    for (int i = 0; i < label_len; ++i, xpos += 6*title_font_size){
        for (int x = 0; x < 5*title_font_size; ++x){
            for (int y = 0; y < 6*title_font_size; ++y){
                if (default_glyphs[(unsigned)*(title + i)][y/title_font_size][x/title_font_size])
                    image[y + ypos][x + xpos] = COLOR_BLACK;
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
    add_text(plot_xlabel, plot_ylabel, plot_title);
    plot_scatter(xarr,yarr,COLOR_PURPLE); // plot individual points
    save_image_as_png(file_path); // convert image to a ppm output
}
