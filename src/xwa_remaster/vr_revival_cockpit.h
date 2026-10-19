/* Quest VR: X-Wing Revival 3D cockpits replacing XWA's cockpit OPTs.
 *
 * resources/vr_cockpits/<craft>.glb are converted from the free X-Wing
 * Revival game by tools/quest/gen_revival_cockpits.py. Model axes are the
 * body eye axes (x right, y down, z forward) in metres with the pilot's eye
 * at the origin. Only used for the stereo VR eyes; the desktop and the flat
 * screen keep the XWA cockpit. */
#ifndef XWA_REMASTER_VR_REVIVAL_COCKPIT_H
#define XWA_REMASTER_VR_REVIVAL_COCKPIT_H

#include <aeron/render.h>
#include <aeron/scene/mesh.h>

/* Revival cockpit mesh for an XWA cockpit OPT basename (e.g.
 * "XwingCockpit"), loaded on first use (uploads into `cmd`); NULL when
 * Revival has no cockpit for it or it failed to load. */
AeronSceneMesh* XwaVrRevivalCockpit_Mesh(AeronCommandBuffer* cmd, const char* cockpit_model_name);

/* Seat-fixed model matrix: `camera_rows` world->eye rows (pre head
 * tracking), `camera_local` the seat eye position in scene units. */
void XwaVrRevivalCockpit_Matrix(const float camera_rows[9], const float camera_local[3], float out[16]);

void XwaVrRevivalCockpit_Shutdown(void);

#endif
