#ifndef XWA_VR_COMMAND_MENU_H
#define XWA_VR_COMMAND_MENU_H

/* In-flight command menu for the Quest build: a panel of flight commands
 * (energy, targeting, wingmen, ship) shown while the left grip is held.
 * Each entry is a keyboard shortcut of the original game. This file holds
 * the layout, hit testing and drawing; xwa_vr.c shows the panel, tracks the
 * controller rays and types the shortcuts. The panel shares the virtual
 * keyboard's image size and swapchain (they are never visible together). */

#include <stdint.h>

#include "xwa_vr/vr_keyboard.h"

enum {
	VR_CMD_WIDTH_PX  = VR_KB_WIDTH_PX,
	VR_CMD_HEIGHT_PX = VR_KB_HEIGHT_PX,
	VR_CMD_NO_ITEM   = -1,
};

typedef struct VrCommand {
	const char* label;
	int         scancode; /* SDL_Scancode */
	int         shift;    /* hold Shift while typing it */
} VrCommand;

typedef struct VrCommandMenu {
	int      hover[2];   /* entry under each controller ray, or VR_CMD_NO_ITEM */
	int      flash_item; /* last chosen entry, drawn bright until flash_until_ms */
	uint64_t flash_until_ms;
} VrCommandMenu;

void VrCommandMenu_Reset(VrCommandMenu* menu);

int VrCommandMenu_Count(void);

const VrCommand* VrCommandMenu_Item(int item);

/* Entry at normalised image coordinates (0..1, v down), or VR_CMD_NO_ITEM. */
int VrCommandMenu_ItemAt(float u, float v);

/* Marks an entry as chosen (flash feedback). */
void VrCommandMenu_Press(VrCommandMenu* menu, int item, uint64_t now_ms);

/* Draws the panel into `rgba` (VR_CMD_WIDTH_PX x VR_CMD_HEIGHT_PX, 4 bytes per
 * pixel, premultiplied alpha). `bgra` swaps red and blue. */
void VrCommandMenu_Render(const VrCommandMenu* menu, uint8_t* rgba, int bgra, uint64_t now_ms);

#endif
