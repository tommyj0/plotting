// #include <assert.h>
// #include <errno.h>
// #include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "font.h"
#include "plotting.h"

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

// USER FUNCTIONS
void xlabel(const char text[]) 
{
  if (text[0] != '\0')
    strcpy(plot_xlabel, text);
}

void ylabel(const char text[]) 
{
  if (text[0] != '\0')
    strcpy(plot_ylabel, text);
}

void title(const char text[]) 
{
  if (text[0] != '\0')
    strcpy(plot_title, text);
}

void path(char * new_path) 
{
  strcpy(file_path, new_path); 
}

void grid(int input_density) 
{
  if (input_density != 0)
    g_density = input_density;
  grid_on = 1;
}

// NON-USER FUNCTIONS
void draw_grid(Colour32 colour) 
{
  // drawing the grid according to the given density
  if (grid_on == 0) // check if grid() has been called
    return;
  for (int i = 1; i < g_density; ++i) 
  {
    for (size_t coord = BORDER; coord < WIDTH - BORDER; ++coord) 
    {
      if (coord % 2 == 0)
      {
        continue;
      }
      image[BORDER + i * (border_area / g_density)][coord] = colour;
      image[coord][BORDER + i * (border_area / g_density)] = colour;
    }
  }
}

void draw_background(Colour32 color) 
{
  for (size_t y = 0; y < HEIGHT; ++y) 
  {
    for (size_t x = 0; x < WIDTH; ++x) 
    {
      image[y][x] = color;
    }
  }
}

void draw_border(Colour32 colour) 
{
  for (size_t x = BORDER; x < WIDTH - BORDER; ++x) 
  {
    image[BORDER][x] = colour;
    image[HEIGHT - BORDER][x] = colour;
  }
  for (size_t y = BORDER; y < HEIGHT - BORDER; ++y) 
  {
    image[y][BORDER] = colour;
    image[y][WIDTH - BORDER] = colour;
  }
}

void save_image_as_png(const char *path) 
{
  uint8_t *image_write;
  int image_size = width * height * CHANNEL_NUM;
  image_write = malloc(image_size * sizeof(uint8_t));
  int index = 0;

  for (int y = 0; y < height; ++y) 
  {
    for (int x = 0; x < width; ++x) 
    {
      uint32_t pixel = image[y][x];
      uint8_t bytes[3] = {
          (pixel & 0x0000FF) >> 8 * 0,
          (pixel & 0x00FF00) >> 8 * 1,
          (pixel & 0xFF0000) >> 8 * 2,
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
  for (i = 0; i < 4; ++i) 
  {
    if (path[p_len + i - 4] != jpg[i]){
      break;
    }
  }
  if (i == 4) 
  {
    stbi_write_jpg(path, WIDTH, HEIGHT, 3, image_write, 100);
    printf("-- JPEG file successfully created and saved as %s --\n", path);
    free(image_write);
    return;
  }

  for (j = 0; j < 4; ++j) 
  {
    if (path[p_len + j - 4] != png[j])
      break;
  }
  if (j == 4) 
  {
    stbi_write_png(path, WIDTH, WIDTH, CHANNEL_NUM, image_write,
                   WIDTH * CHANNEL_NUM);
    printf("-- PNG file successfully created and saved as %s --\n", path);
    free(image_write);
    return;
  }
  printf("ERROR: invalid path - ensure the path string ends in .png or .jpg\n");
  exit(1);
}

float max_value(float * input_array) 
{
  // finding the maximum value of the given array
  float max = *input_array;
  for (int i = 0; i < array_length; ++i) 
  {
    if (*(input_array + i) > max)
      max = *(input_array + i);
  }
  return max;
}

float min_value(float * input_array) 
{
  // finding the minimum value of the given array
  float min = * input_array;
  for (int i = 0; i < array_length; ++i) 
  {
    if (*(input_array + i) < min)
      min = *(input_array + i);
  }
  return min;
}

void check_length(int len, const char spec[]) 
{
  // check the length of a string and throw an error/warning if needed
  int max_len = 33;
  int warning_length = 25;
  if (len > max_len) 
  {
    printf("ERROR: %s is too long - shorten the string to less than %d\n", spec,
           warning_length);
    exit(1);
  } 

  else if (len > warning_length) 
  {
    printf("WARNING: %s is kinda long - make it less than %d chars it so it "
           "doesn't run off the edge\n",
           spec, warning_length);
  }
}

void plot_scatter(float * x, float * y, Colour32 colour) 
{
  // plots the given points individually
  int xval, yval;

  for (int i = 0; i < array_length; ++i) 
  {

    float min_x = min_value(x);
    float max_x = max_value(x);
    float min_y = min_value(y);
    float max_y = max_value(y);

    if (min_x == max_x)
      xval = plot_area / 2;
    else
      xval = ((*(x + 1) - min_x) / (max_x - min_x)) * f_plot_size;
    if (min_y == max_y)
      yval = plot_area / 2;
    else
      yval = ((*(y + 1) - min_y) / (max_y - min_y)) * f_plot_size;

    for (int j = -DOT_SIZE; j <= DOT_SIZE; ++j) 
    {
      for (int k = -DOT_SIZE; k <= DOT_SIZE; ++k) 
      {
        image[HEIGHT - PLOT_BORDER - BORDER - yval + j]
             [PLOT_BORDER + BORDER + xval + k] = colour;
      }
    }
  }
}

void draw_text(const char * label, const int font_size, int ypos, int xpos,
               char orientation) {
  int label_len = (int)strlen(label);
  check_length(label_len, label);
  if (orientation == 'h') 
  {
    xpos = xpos - (label_len * font_size * 6) / 2;

    for (int i = 0; i < label_len; ++i, xpos += 6 * font_size) 
    {
      for (int x = 0; x < 5 * font_size; ++x) 
      {
        for (int y = 0; y < 6 * font_size; ++y) 
        {
          if (default_glyphs[(unsigned)*(label + i)][y / font_size]
                            [x / font_size]) 
          {
            image[y + ypos][x + xpos] = COLOR_BLACK;
          }
        }
      }
    }
  } 
  else if (orientation == 'v') 
  {

    ypos = 500 + (label_len * font_size * 5) / 2;
    xpos = 50;
    for (int i = 0; i < label_len; ++i, ypos -= 6 * font_size) 
    {
      for (int x = 0; x < 6 * font_size; ++x) 
      {
        for (int y = 0; y < 5 * font_size; ++y) 
        {
          if (default_glyphs[(unsigned)*(label + i)][x / font_size]
                            [4 - y / font_size])
            image[ypos + y][xpos + x] = COLOR_BLACK;
        }
      }
    }
  }
}

void add_text(const char *xlabel, const char *ylabel, const char *title) 
{
  // function draws all the required text
  draw_text(xlabel, label_font_size, 920, 500, 'h'); // adding an x-axis label
  draw_text(ylabel, label_font_size, 50, 500, 'v');  // adding a y-axis label
  draw_text(title, title_font_size, 50, 500, 'h');   // adding a title
}

/*--------------------------------------------------------------*/
/*--------------------MAIN PLOTTING FUNCTION--------------------*/
/*--------------------------------------------------------------*/

void plot(float * xarr, float * yarr, int size_array) 
{
  // input should be of the form - plot(x array, y array, size of array)
  array_length = size_array;
  draw_background(COLOR_GREY);                    // fill in background
  draw_border(COLOR_BLACK);                       // draw a plot area
  draw_grid(COLOR_DARKGREY);                      // draw a grid if requested
  add_text(plot_xlabel, plot_ylabel, plot_title); // wonder what this one does
  plot_scatter(xarr, yarr, COLOR_PURPLE);         // plot individual points
  save_image_as_png(file_path); // convert image to a png output
}
