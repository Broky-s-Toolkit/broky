#pragma once
#include "main.h"


// > SUBMODULE: GRID
//   1. Grid has a workspace: GUI_GridReset(Rectangle workspace) or  GUI_GridResetForScreen()
//   2. The workspace is divided: GUI_GridFor(...)
//   3. GUI_GridAt(x,y) to get shapes OR GUI_GridNextX() to directly consume the workspace
// > INDEX
// > BASICS
GUI_GridTemp    GUI_MakeGrid(void);
float           GUI_GridHeightOrDefault(void);
float           GUI_GridWidthOrDefault(void);
Rectangle       GUI_GridRelative(Rectangle shape);
Rectangle       GUI_GridRelativePositionOnly(Rectangle shape);
// > GRID STARTERS
void            GUI_GridResetForScreen(void);
void            GUI_GridReset(Rectangle workspace);
void            GUI_GridForX(float w);
void            GUI_GridForY(float h);
void            GUI_GridForXY(float w, float h, float h_keep);
void            GUI_GridForCols(int columns, float default_height);
void            GUI_GridRepeat(void);
void            GUI_GridClearWorkspace(void);
// > IN PLACE QUERIES
Rectangle       GUI_GridAt(int x, int y);
Rectangle       GUI_GridBetween(int x, int y, int x_end, int y_end);
// > CONSUMABLES
Rectangle       GUI_GridNextX(void);
Rectangle       GUI_GridNextY(void);
Rectangle       GUI_GridNextXn(int n);
Rectangle       GUI_GridNextYn(int n);
void            GUI_GridAutoJump(void);
// > INFO
Rectangle       GUI_GridAvailable(Rectangle workspace);
Rectangle       GUI_GridApplyScroll(Rectangle shape);

