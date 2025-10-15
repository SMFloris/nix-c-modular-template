#include "binary_images.h"
#include "rgridlayout.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  const char *path;
  int w, h;
  Color *grayData;
  Color *binData;
  BinaryImageData stats;
  Texture2D texGray;
  Texture2D texBin;
} Row;

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s img1.png [img2.png ...]\n", argv[0]);
    return 1;
  }

  const int pad = 10, marginX = 10, marginY = 10, captionH = 18;
  int rowsN = argc - 1;

  Row *rows = (Row *)calloc(rowsN, sizeof(Row));
  if (!rows) {
    fprintf(stderr, "OOM\n");
    return 1;
  }

  // Preload & process on CPU (no window needed)
  int maxRowWidth = 0;
  int totalHeight = 0;
  for (int i = 0; i < rowsN; ++i) {
    rows[i].path = argv[i + 1];

    Image img = LoadImage(rows[i].path);
    if (img.data == NULL) {
      fprintf(stderr, "Failed to load: %s\n", rows[i].path);
      return 1;
    }

    rows[i].w = img.width;
    rows[i].h = img.height;

    Color *px = LoadImageColors(img);
    int count = img.width * img.height;

    rows[i].grayData = color_to_grayscale(count, px);
    BinaryImage *binary = color_to_binary(img.width, img.height, 0.1, rows[i].grayData);
    rows[i].stats = get_binary_image_data(binary);
    rows[i].binData = binary_to_color(binary);

    UnloadImageColors(px);
    UnloadImage(img);

    int rowWidth = 3 * rows[i].w + 2 * pad;
    if (rowWidth > maxRowWidth)
      maxRowWidth = rowWidth;
    totalHeight += rows[i].h + captionH;
    if (i < rowsN - 1)
      totalHeight += pad;
  }

  int W = marginX * 2 + maxRowWidth;
  int H = marginY * 2 + totalHeight;
  if (W < 800)
    W = 800;
  if (H < 600)
    H = 600;

  InitWindow(W, H, "Raylib rgridlayout (multi-image)");

  // Create GPU textures
  for (int i = 0; i < rowsN; ++i) {
    rows[i].texGray = LoadTextureFromImage((Image){
        .data = rows[i].grayData, .width = rows[i].w, .height = rows[i].h, .mipmaps = 1, .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8});
    rows[i].texBin = LoadTextureFromImage((Image){
        .data = rows[i].binData, .width = rows[i].w, .height = rows[i].h, .mipmaps = 1, .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8});
  }

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    int originY = marginY;

    for (int i = 0; i < rowsN; ++i) {
      // Each row uses its own grid sized to that image (height includes caption).
      rgrid_init(rows[i].w, rows[i].h + captionH, pad);
      rgrid_setOrigin(marginX, originY);
      rgrid_reset();

      // Cell 0: Grayscale
      rgrid_beginCell();
      DrawTexture(rows[i].texGray, 0, 0, WHITE);
      DrawText("Grayscale", 0, rows[i].h + 4, 14, BLACK);
      rgrid_endCell();

      // Cell 1: Binary + features
      rgrid_beginCell();
      DrawTexture(rows[i].texBin, 0, 0, WHITE);
      DrawText("Binary", 0, rows[i].h + 4, 14, BLACK);

      Vector2 p = (Vector2){rows[i].stats.x_bar, rows[i].stats.y_bar};
      float a = rows[i].stats.gama1;
      float L = (float)(rows[i].w < rows[i].h ? rows[i].w : rows[i].h) * 0.4f;
      Vector2 d = (Vector2){cos(a) * L, sin(a) * L};
      Vector2 a0 = (Vector2){p.x - d.x, p.y - d.y};
      Vector2 a1 = (Vector2){p.x + d.x, p.y + d.y};
      DrawCircle((int)p.x, (int)p.y, 5, RED);
      DrawLineEx(a0, a1, 2.0f, RED);
      rgrid_endCell();

      // Cell 2: Stats
      rgrid_beginCell();
      int y = 0;
      auto Emin = E_gamma(&rows[i].stats, rows[i].stats.gama1);
      auto Emax = E_gamma(&rows[i].stats, rows[i].stats.gama2);
      DrawText(TextFormat("Roundedness: %.2f", Emin / Emax), 0, y, 22, BLACK);
      rgrid_endCell();

      originY += rows[i].h + captionH + pad;
    }

    EndDrawing();
  }

  // Cleanup
  for (int i = 0; i < rowsN; ++i) {
    UnloadTexture(rows[i].texGray);
    UnloadTexture(rows[i].texBin);
    // If you won't reuse the CPU buffers, free them now.
    MemFree(rows[i].grayData);
    MemFree(rows[i].binData);
  }
  free(rows);

  CloseWindow();
  return 0;
}
