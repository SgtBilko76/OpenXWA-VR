#include "xwa_vr/vr_command_menu.h"

#include "xwa_vr/vr_draw.h"

#include <SDL3/SDL_scancode.h>

#include <string.h>

/* Four columns of five commands; the keys are the game's own shortcuts
 * (JOYSTICK.TXT / the XWA manual). */
enum {
	COLUMNS     = 4,
	ROWS        = 5,
	MARGIN_PX   = 16,
	HEADER_PX   = 48,
	GAP_PX      = 8,
	RADIUS_PX   = 12,
	ITEM_COUNT  = COLUMNS * ROWS,
};

static const char* const k_headers[COLUMNS] = { "ENERGY", "TARGETING", "WINGMEN", "SHIP" };

/* Column-major: k_items[column * ROWS + row]. */
static const VrCommand k_items[ITEM_COUNT] = {
	/* Energy */
	{ "Laser rate", SDL_SCANCODE_F9, 0 },
	{ "Shield rate", SDL_SCANCODE_F10, 0 },
	{ "Shield balance", SDL_SCANCODE_S, 0 },
	{ "Shield > laser", SDL_SCANCODE_F9, 1 },
	{ "Laser > shield", SDL_SCANCODE_F10, 1 },
	/* Targeting */
	{ "Next objective", SDL_SCANCODE_O, 0 },
	{ "Components", SDL_SCANCODE_COMMA, 0 },
	{ "Its attacker", SDL_SCANCODE_A, 0 },
	{ "Map", SDL_SCANCODE_M, 0 },
	{ "Hyperspace", SDL_SCANCODE_H, 0 },
	/* Wingmen */
	{ "Attack target", SDL_SCANCODE_A, 1 },
	{ "Cover me", SDL_SCANCODE_C, 1 },
	{ "Ignore target", SDL_SCANCODE_I, 1 },
	{ "Reinforce", SDL_SCANCODE_S, 1 },
	{ "Call resupply", SDL_SCANCODE_B, 1 },
	/* Ship */
	{ "Pick up cargo", SDL_SCANCODE_P, 1 },
	{ "Release cargo", SDL_SCANCODE_R, 1 },
	{ "Dock / board", SDL_SCANCODE_D, 1 },
	{ "S-foils", SDL_SCANCODE_V, 0 },
	{ "Confirm", SDL_SCANCODE_SPACE, 0 },
};

static int column_width(void) { return (VR_CMD_WIDTH_PX - 2 * MARGIN_PX) / COLUMNS; }

static int row_height(void) { return (VR_CMD_HEIGHT_PX - 2 * MARGIN_PX - HEADER_PX) / ROWS; }

static void item_rect(int item, int* x0, int* y0, int* x1, int* y1) {
	const int column = item / ROWS;
	const int row    = item % ROWS;

	*x0 = MARGIN_PX + column * column_width() + GAP_PX / 2;
	*x1 = MARGIN_PX + (column + 1) * column_width() - GAP_PX / 2;
	*y0 = MARGIN_PX + HEADER_PX + row * row_height() + GAP_PX / 2;
	*y1 = MARGIN_PX + HEADER_PX + (row + 1) * row_height() - GAP_PX / 2;
}

void VrCommandMenu_Reset(VrCommandMenu* menu) {
	memset(menu, 0, sizeof(*menu));
	menu->hover[0]   = VR_CMD_NO_ITEM;
	menu->hover[1]   = VR_CMD_NO_ITEM;
	menu->flash_item = VR_CMD_NO_ITEM;
}

int VrCommandMenu_Count(void) { return ITEM_COUNT; }

const VrCommand* VrCommandMenu_Item(int item) { return item >= 0 && item < ITEM_COUNT ? &k_items[item] : NULL; }

int VrCommandMenu_ItemAt(float u, float v) {
	const int px = (int)(u * VR_CMD_WIDTH_PX) - MARGIN_PX;
	const int py = (int)(v * VR_CMD_HEIGHT_PX) - MARGIN_PX - HEADER_PX;
	int       column, row;

	/* Gaps belong to the nearest entry so the ray never falls between them. */
	if (px < 0 || py < 0) {
		return VR_CMD_NO_ITEM;
	}
	column = px / column_width();
	row    = py / row_height();
	if (column >= COLUMNS || row >= ROWS) {
		return VR_CMD_NO_ITEM;
	}
	return column * ROWS + row;
}

void VrCommandMenu_Press(VrCommandMenu* menu, int item, uint64_t now_ms) {
	menu->flash_item     = item;
	menu->flash_until_ms = now_ms + 180;
}

void VrCommandMenu_Render(const VrCommandMenu* menu, uint8_t* rgba, int bgra, uint64_t now_ms) {
	static const VrRgba panel      = { 12, 16, 24, 225 };
	static const VrRgba entry      = { 52, 58, 70, 245 };
	static const VrRgba entry_hov  = { 70, 120, 200, 255 };
	static const VrRgba entry_hit  = { 230, 230, 240, 255 };
	static const VrRgba label      = { 240, 240, 245, 255 };
	static const VrRgba label_dark = { 20, 20, 25, 255 };
	static const VrRgba header     = { 200, 140, 40, 255 };
	VrImage             img        = { rgba, VR_CMD_WIDTH_PX, VR_CMD_HEIGHT_PX };

	VrDraw_Clear(&img);
	VrDraw_RoundRect(&img, 0, 0, VR_CMD_WIDTH_PX, VR_CMD_HEIGHT_PX, 24, panel);
	for (int column = 0; column < COLUMNS; ++column) {
		VrDraw_TextCentered(&img, k_headers[column], MARGIN_PX + column * column_width() + column_width() / 2,
							MARGIN_PX + HEADER_PX / 2, header);
	}
	for (int item = 0; item < ITEM_COUNT; ++item) {
		const int flashed = item == menu->flash_item && now_ms < menu->flash_until_ms;
		const int hovered = item == menu->hover[0] || item == menu->hover[1];
		int       x0, y0, x1, y1;

		item_rect(item, &x0, &y0, &x1, &y1);
		VrDraw_RoundRect(&img, x0, y0, x1, y1, RADIUS_PX, flashed ? entry_hit : hovered ? entry_hov : entry);
		VrDraw_TextCentered(&img, k_items[item].label, (x0 + x1) / 2, (y0 + y1) / 2, flashed ? label_dark : label);
	}
	if (bgra) {
		VrDraw_SwapRedBlue(&img);
	}
}
