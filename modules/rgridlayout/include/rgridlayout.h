#ifndef RGRIDLAYOUT_H
#define RGRIDLAYOUT_H

// rgridlayout: simple grid layout transforms for raylib
// Requires raylib + rlgl (bundled with raylib). Safe no-op if Begin/End not paired.

#include <raylib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize grid sizing.
// cellWidth/cellHeight: size of a cell in pixels
// cellPadding: spacing between cells (both axes)
void rgrid_init(int cellWidth, int cellHeight, int cellPadding);

// Set the screen-space origin (top-left) where the grid starts. Default is (0,0).
void rgrid_setOrigin(int x, int y);

// Reset the layout cursor to the first cell at origin (row=0, col=0).
void rgrid_reset(void);

// Begin drawing into the current cell. All draw calls after this will be
// translated so that (0,0) maps to the top-left of the cell.
void rgrid_beginCell(void);

// End drawing into the current cell. Pops the transform and advances to the
// next cell in the current row (col += 1).
void rgrid_endCell(void);

// Move to the start of the next row: (row += 1; col = 0).
void rgrid_newRow(void);

// Helpers to query the current cell placement.
Vector2 rgrid_cellOrigin(void); // screen-space top-left of current cell
Rectangle rgrid_cellRect(void); // screen-space rect of current cell

#ifdef __cplusplus
}
#endif

#endif // RGRIDLAYOUT_H

