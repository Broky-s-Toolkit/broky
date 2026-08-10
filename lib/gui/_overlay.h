#pragma once
#include "main.h"


// > SUBMODULE: OVERLAY
// > INDEX
GUI_Overlay     GUI_MakeOverlay(void);
void            GUI_DrawOverlay(void);
bool            GUI_OverlayOpenedBy(const void* owner_id);
bool            GUI_OverlayWasJustEnabled(void);
EGUI_ThemeColor GUI_OverlayColors(void);
void            GUI_OverlayClose(void);
void            GUI_OverlayOpenFor(const void* id);
void            GUI_OverlaySetDrawCall(bool just_enabled, EGUI_ThemeColor colors, void (*draw_function)(void));
void            GUI_OverlaySetFinalShape(Rectangle shape);
bool            GUI_OverlayIsDrawing(void);
void            GUI_BeginOverlay(bool force_overflow, bool restore_grid);
void            GUI_EndOverlay(Rectangle final_shape);

