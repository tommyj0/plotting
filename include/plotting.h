#ifndef PLOTTING_H
#define PLOTTING_H

void xlabel(const char text[]);
void ylabel(const char text[]);
void title(const char text[]);
void grid(int input_density);
void path(char new_path[]);
void plot(float xarr[], float yarr[], int size_array);

#endif