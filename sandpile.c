#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static void die(const char *s) {
  fprintf(stderr, "%s\n", s);
  exit(1);
}

int N = 801;
int16_t **grid;

// you can also use this function to debug print your sandpile
static void render(char *filename) {
  int chan = 3;
  unsigned char *pixels = (unsigned char *)malloc(N * N * chan * sizeof(char));
  int stride = N * chan;
  if (!pixels)
    die("malloc failed for PNG pixel buffer");
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      unsigned char r = 0, g = 0, b = 0;
      switch (grid[i][j]) {
      case 0: // obsidian
        r = 15;
        g = 15;
        b = 25;
        break;
      case 1: // electric cyan
        r = 0;
        g = 210;
        b = 255;
        break;
      case 2: // vibrant magenta
        r = 230;
        g = 30;
        b = 130;
        break;
      case 3: // bright gold
        r = 255;
        g = 200;
        b = 0;
        break;
      default: // white
        r = 0;
        g = 0;
        b = 0;
      }

      int pid = (i * N + j) * chan;
      pixels[pid] = r;
      pixels[pid + 1] = g;
      pixels[pid + 2] = b;
    }
  }
  if (stbi_write_png(filename, N, N, chan, pixels, stride)) {
    printf("Successfully saved sandpile identity to: %s\n", filename);
  } else {
    die("Error: stb_image_write failed to save png file\n");
  }
  free(pixels);
}

static int update_elements(int i_index) {
  int changed = 0;
  #pragma omp parallel for
  for (int i = i_index; i < N; i += 3) {
    int16_t* row = grid[i];
    int max = MAX(i, N - 1 - i);
    int c = 0;
    for (int j = 0; j <= max; ++j) {
      if (row[j] < 4) continue;
      if ((i - N / 2) * (i - N / 2) + (j - N / 2) * (j - N / 2) <=
            (N / 2) * (N / 2)) {
        c = 1; // topple it
        row[j] -= 4;
        if (i > 0)
          grid[i-1][j]++;
        if (j > 0)
          row[j - 1]++;
        if (i < N - 1)
          grid[i+1][j]++;
        if (j < N - 1)
          row[j + 1]++;
      }
    }
    if (c == 1) changed = 1;
  }
  return changed;
}

static void stabilize() {
  int changed = 0;
  do {
    changed = 0;
    int changed0 = update_elements(0); // row_index % 3 == 0
    int changed1 = update_elements(1); // row_index % 3 == 1
    int changed2 = update_elements(2); // row_index % 3 == 2

    changed = (changed0 == 1 || changed1 == 1 || changed2 == 1);
  } while (changed);
}

void find_identity() {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      grid[i][j] = 6;
    }
  }
  stabilize();
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      grid[i][j] = 6 - grid[i][j];
    }
  }
  stabilize();
}

int main(int argc, char **argv) {
  struct timeval start, stop;
  double elapsed_time;

  gettimeofday(&start, NULL);

  if (argc < 2 || argc > 2) {
    fprintf(stderr, "Usage: %s N\n", argv[0]);
    return EXIT_FAILURE;
  }
  char *end;
  uintmax_t parsed = strtoumax(argv[1], &end, 10);
  if (errno || argv[1][0] < '0' || argv[1][0] > '9' || *end || !parsed ||
      parsed > INT32_MAX)
    die("grid size must be a positive int");
  N = (int)parsed;

  grid = (int16_t **)malloc(N * sizeof(int16_t *));
  for (int i = 0; i < N; ++i) {
    grid[i] = (int16_t *)malloc(N * sizeof(int16_t));
  }

  find_identity();

  render("identity.png");
  for (int i = 0; i < N; ++i) {
    free(grid[i]);
  }
  free(grid);

  gettimeofday(&stop, NULL);
  elapsed_time = (stop.tv_sec - start.tv_sec) + (stop.tv_usec - start.tv_usec) / 1000000.0;
  printf("Execution time: %.6f seconds\n", elapsed_time);
  return EXIT_SUCCESS;
}
