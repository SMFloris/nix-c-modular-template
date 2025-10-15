#pragma once

#include <raylib.h>

typedef struct {
  int width;
  int height;
  bool *data;
} BinaryImage;

typedef struct {
  double A;
  double x_bar;
  double y_bar;
  double a;
  double b;
  double c;
  double gama1;
  double gama2;
} BinaryImageData;

/** misc **/
BinaryImage *new_binary_image(int width, int height);
BinaryImage *color_to_binary(int width, int height, float threshold, Color pix[static width * height]);
Color *binary_to_color(BinaryImage *binary);
Color *color_to_grayscale(int count, Color pix[static count]);

/** algorithms **/
BinaryImageData get_binary_image_data(BinaryImage *img);
double E_gamma(const BinaryImageData *d, double gamma);

