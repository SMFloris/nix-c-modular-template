#include "rgridlayout.h"
#include <rlgl.h> // raylib's lower-level transform API

// Internal state (single global layout). If you need multiple layouts,
// wrap these in a struct and expose a handle; this keeps the minimal API requested.
static struct {
  int cw, ch;  // cell width/height
  int pad;     // padding between cells
  int originX; // grid origin top-left
  int originY;
  int row;     // current row index
  int col;     // current column index
  bool inCell; // guard to ensure balanced begin/end
} G = {.cw = 0, .ch = 0, .pad = 0, .originX = 0, .originY = 0, .row = 0, .col = 0, .inCell = false};

static inline int cell_x(void) { return G.originX + G.col * (G.cw + G.pad); }
static inline int cell_y(void) { return G.originY + G.row * (G.ch + G.pad); }

void rgrid_init(int cellWidth, int cellHeight, int cellPadding) {
  G.cw = (cellWidth > 0) ? cellWidth : 1;
  G.ch = (cellHeight > 0) ? cellHeight : 1;
  G.pad = (cellPadding >= 0) ? cellPadding : 0;
  G.row = 0;
  G.col = 0;
  G.inCell = false;
}

void rgrid_setOrigin(int x, int y) {
  G.originX = x;
  G.originY = y;
}

void rgrid_reset(void) {
  G.row = 0;
  G.col = 0;
}

void rgrid_beginCell(void) {
  if (G.inCell)
    return; // prevent nested begin; user error guard
  rlPushMatrix();
  rlTranslatef((float)cell_x(), (float)cell_y(), 0.0f);
  G.inCell = true;
}

void rgrid_endCell(void) {
  if (!G.inCell)
    return; // guard double-end
  rlPopMatrix();
  G.inCell = false;
  // Advance to next column for the next begin/end pair
  G.col += 1;
}

void rgrid_newRow(void) {
  // If we're inside a cell, finish it without advancing column twice
  if (G.inCell) {
    rlPopMatrix();
    G.inCell = false;
  }
  G.row += 1;
  G.col = 0;
}

Vector2 rgrid_cellOrigin(void) {
  return (Vector2){(float)cell_x(), (float)cell_y()};
}

Rectangle rgrid_cellRect(void) {
  return (Rectangle){(float)cell_x(), (float)cell_y(), (float)G.cw, (float)G.ch};
}

