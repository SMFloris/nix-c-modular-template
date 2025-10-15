#include "binary_images.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

/** creation */
BinaryImage *new_binary_image(int width, int height) {
  BinaryImage *binary = malloc(sizeof *binary);
  binary->height = height;
  binary->width = width;
  binary->data = malloc(height * width * sizeof(bool));
  return binary;
}

/** conversions */
BinaryImage *color_to_binary(int width, int height, float threshold, Color pix[static width * height]) {
  BinaryImage *binary = new_binary_image(width, height);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      int idx = i + j * binary->width;
      binary->data[idx] = (pix[i + j * width].g / 255.0) > threshold;
    }
  }
  return binary;
}

Color *binary_to_color(BinaryImage *binary) {
  Color *px = calloc(binary->height * binary->width, sizeof(Color));
  for (int i = 0; i < binary->width; i++) {
    for (int j = 0; j < binary->height; j++) {
      int idx = i + j * binary->width;
      bool p = binary->data[idx];
      px[idx].r = px[idx].g = px[idx].b = (unsigned char)((int)p) * 255;
      px[idx].a = 255;
    }
  }
  return px;
}

Color *color_to_grayscale(int count, Color pix[static count]) {
  Color *px = calloc(count, sizeof(Color));
  for (int i = 0; i < count; i++) {
    unsigned gray = (pix[i].r + pix[i].g + pix[i].b) / 3;
    px[i].r = px[i].g = px[i].b = (unsigned char)gray;
    px[i].a = 255;
  }
  return px;
}

/** algorithms */
BinaryImageData get_binary_image_data(BinaryImage *img) {
  BinaryImageData out = {0};

  // centroid
  double sx = 0, sy = 0, A = 0;
  for (int x = 0; x < img->width; x++)
    for (int y = 0; y < img->height; y++) {
      int idx = x + y * img->width;
      int p = (int)img->data[idx];
      A += p;
      sx += p * x;
      sy += p * y;
    }
  out.x_bar = (sx / A);
  out.y_bar = (sy / A);

  // central second moments
  double a = 0, b = 0, c = 0;
  for (int x = 0; x < img->width; x++)
    for (int y = 0; y < img->height; y++) {
      int idx = x + y * img->width;
      int p = (int)img->data[idx];
      double x0 = x - out.x_bar, y0 = y - out.y_bar;
      a += p * x0 * x0;
      b += p * 2.0 * x0 * y0;
      c += p * y0 * y0;
    }
  out.a = a;
  out.b = b;
  out.c = c;
  out.gama1 = atan2f(out.b, out.a - out.c) / 2.0f;
  out.gama2 = out.gama1 + PI / 2.0f;
  return out;
}

double E_gamma(const BinaryImageData *d, double gamma) {
  float s = sin(gamma), c = cos(gamma);
  return d->a * s * s - d->b * s * c + d->c * c * c;
}

