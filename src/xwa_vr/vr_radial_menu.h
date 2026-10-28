#ifndef XWA_VR_RADIAL_MENU_H
#define XWA_VR_RADIAL_MENU_H

/* In-flight radial ("star") menus for the Quest build. Holding a grip shows
 * that hand's menu above its controller; tilting the same hand's stick picks
 * one of eight commands and releasing the grip types its keyboard shortcut.
 * Left hand: energy and ship. Right hand: targeting and wingmen. This file
 * holds the commands, the direction mapping and the drawing; xwa_vr.c shows
 * the panels and types the shortcuts. */

#include <stdint.h>

enum {
	VR_RADIAL_SIZE_PX = 1024, /* square image */
	VR_RADIAL_ITEMS   = 8,
	VR_RADIAL_NONE    = -1,
};

typedef enum VrRadialSide { VR_RADIAL_LEFT = 0, VR_RADIAL_RIGHT = 1 } VrRadialSide;

typedef struct VrCommand {
	const char* label;
	int         scancode; /* SDL_Scancode */
	int         shift;    /* hold Shift while typing it */
} VrCommand;

/* Command `item` (0 = up, then clockwise) of a side's menu. */
const VrCommand* VrRadialMenu_Item(VrRadialSide side, int item);

/* Item for a stick direction (x right, y up, magnitude ignored). */
int VrRadialMenu_ItemForDirection(float x, float y);

/* Draws a side's menu with `selected` highlighted (or VR_RADIAL_NONE) into
 * `rgba` (VR_RADIAL_SIZE_PX squared, 4 bytes per pixel, premultiplied alpha).
 * `bgra` swaps red and blue. */
void VrRadialMenu_Render(VrRadialSide side, int selected, uint8_t* rgba, int bgra);

#endif
