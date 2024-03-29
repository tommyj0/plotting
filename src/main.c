#include <math.h>
#include <stdio.h>

#include "plotting.h"

#define N 5000

int main(void)
{
  // test plot
  int reps = 2;
  float * xarr = malloc(N*sizeof(float));
  float * yarr = malloc(N*sizeof(float));
  assert(xarr != NULL && yarr != NULL);

  float y, x;

  for (float i = -N / 2; i < N / 2; ++i)
  {
    x = reps * i / N;
    y = reps * (5 * x * x * x + 4 * x * x);
    xarr[(int)i + N / 2] = i;
    yarr[(int)i + N / 2] = y;
  }

  xlabel("testing new");
  // ylabel("");
  title("shit graph");
  path("out/myplot.png");
  grid(15);
  plot(xarr, yarr, N);  // plot called last
  return 0;
}
