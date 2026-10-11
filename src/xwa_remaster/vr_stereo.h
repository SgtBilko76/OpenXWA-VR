#ifndef XWA_REMASTER_VR_STEREO_H
#define XWA_REMASTER_VR_STEREO_H

/* Stereo flight rendering for VR presentation.
 *
 * The VR host publishes the two eye views of the current frame before
 * XwaRemaster_Frame(). While the HD flight scene is active the remaster then
 * renders the 3D world once per eye (head-tracked, real parallax) into the
 * host's eye textures, and renders the HUD alone on a transparent background
 * into the normal presentation frame, sized to the game camera's field of
 * view. Map, hyperspace and every non-flight scene stay on the flat path. */

#include <SDL3/SDL_gpu.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XwaVrStereoEye {
	/* Eye pose relative to the seated reference (OpenXR LOCAL space at
	 * recenter): x right, y up, -z forward. Metres; quaternion x,y,z,w. */
	float           position_m[3];
	float           orientation[4];
	/* Positive tangents of the four frustum half-angles. */
	float           tan_left, tan_right, tan_up, tan_down;
	SDL_GPUTexture* target;
	int             target_width;
	int             target_height;
} XwaVrStereoEye;

typedef struct XwaVrStereoFrame {
	XwaVrStereoEye eyes[2];
} XwaVrStereoFrame;

/* Publishes this frame's eye views (NULL disables stereo). The struct is
 * copied; targets must stay valid until the frame's GPU work is submitted. */
void XwaRemasterVr_SetStereoFrame(const XwaVrStereoFrame* frame);

/* 1 when the last XwaRemaster_Frame() wrote both eye targets; the flat
 * presentation frame then holds only the transparent HUD overlay. */
int XwaRemasterVr_StereoRendered(void);

/* Positive tangents of the HUD frame's left/right/up/down edges as seen from
 * the seat (the game camera's frustum, including its off-centre horizon), so
 * a host can place the HUD overlay to line up with the 3D world. Returns 0
 * while unknown. */
int XwaRemasterVr_HudTangents(float* tan_left, float* tan_right, float* tan_up, float* tan_down);

/* Number of world units per metre of head movement. */
float XwaRemasterVr_WorldUnitsPerMetre(void);

#ifdef __cplusplus
}
#endif

#endif
