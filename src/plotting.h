#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

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
#define PI 3.14159

// defaults
#define DEFAULT_FILE_PATH "plot.png"
#define DEFAULT_TITLE "plot title"
#define DEFAULT_X_LABEL "x-axis"
#define DEFAULT_Y_LABEL "y-axis"
#define DOT_SIZE 2
#define GRID_DENSITY 10

// colours in hex 
// 0xAABBGGRR
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0xFF000000
#define COLOR_RED   0xFF0000FF
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE  0xFFFF0000
#define COLOR_PURPLE 0xFFFF00AF
#define COLOR_GREY  0xFFA0A0A0
#define COLOR_DARKGREY 0xFF505050

// useless typedef
typedef uint32_t Colour32;

// user functions
void xlabel(const char text[]);
void ylabel(const char text[]);
void title(const char text[]);
void grid(int input_density);
void path(char * new_path);
void plot(float * xarr, float * yarr, int size_array);

