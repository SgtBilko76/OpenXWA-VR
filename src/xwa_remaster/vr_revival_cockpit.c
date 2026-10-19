/* Quest VR: X-Wing Revival 3D cockpits (see vr_revival_cockpit.h). */
#include "xwa_remaster/vr_revival_cockpit.h"

#include <aeron/aeron.h>
#include <aeron/asset/flight_model.h>
#include <aeron/asset/opt_model.h>
#include <aeron/log.h>
#include <aeron/vfs.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef struct Cockpit {
	const char* opt;  /* XWA cockpit OPT basename */
	const char* path; /* Revival model resource */
	AeronFlightModel* model;
	AeronSceneMesh* mesh;
	bool failed;
} Cockpit;

/* XWA flies the TIE Fighter and TIE Interceptor with TieCockpit. */
static Cockpit cockpits[] = {
	{ "XwingCockpit", "vr_cockpits/xwing.glb" },
	{ "YwingCockpit", "vr_cockpits/ywing.glb" },
	{ "AwingCockpit", "vr_cockpits/awing.glb" },
	{ "BwingCockpit", "vr_cockpits/bwing.glb" },
	{ "TieCockpit", "vr_cockpits/tiefighter.glb" },
	{ "TieBomberCockpit", "vr_cockpits/tiebomber.glb" },
	{ "MilleniumFalcon2Cockpit", "vr_cockpits/falcon.glb" },
};
enum { COCKPIT_COUNT = sizeof cockpits / sizeof cockpits[0] };

static Cockpit* Find(const char* name) {
	if (!name || !name[0])
		return NULL;
	size_t len = strcspn(name, ".");
	for (int i = 0; i < COCKPIT_COUNT; ++i)
		if (strlen(cockpits[i].opt) == len && strncasecmp(cockpits[i].opt, name, len) == 0)
			return &cockpits[i];
	return NULL;
}

AeronSceneMesh* XwaVrRevivalCockpit_Mesh(AeronCommandBuffer* cmd, const char* cockpit_model_name) {
	Cockpit* c = Find(cockpit_model_name);
	if (!c || c->failed)
		return NULL;
	if (c->mesh)
		return c->mesh;
	c->failed = true;
	uint8_t* data = NULL;
	size_t size = 0;
	if (!AeronVfs_ReadAll(Aeron_GetVfs(), AERON_VFS_ROOT_RESOURCE, c->path, 0, &data, &size)) {
		Aeron_LogInfo("xwa.vr", "3D cockpit %s not found; keeping %s", c->path, c->opt);
		return NULL;
	}
	c->model = calloc(1, sizeof *c->model);
	bool built = c->model && Aeron_FlightModelBuildMemory(data, size, c->path, c->model);
	free(data);
	if (!built) {
		Aeron_LogWarn("xwa.vr", "3D cockpit %s failed to load", c->path);
		free(c->model);
		c->model = NULL;
		return NULL;
	}
	AeronSceneMeshCreateStatus status = AERON_SCENE_MESH_CREATE_SUCCESS;
	c->mesh = AeronScene_MeshCreate(cmd, c->model, c->path, &status);
	if (!c->mesh) {
		Aeron_LogWarn("xwa.vr", "3D cockpit %s mesh creation failed (%d)", c->path, (int)status);
		return NULL;
	}
	c->failed = false;
	Aeron_LogInfo("xwa.vr", "3D cockpit: X-Wing Revival %s for %s (%u triangles)", c->path, c->opt,
				  c->mesh->index_count / 3u);
	return c->mesh;
}

void XwaVrRevivalCockpit_Matrix(const float camera_rows[9], const float camera_local[3], float out[16]) {
	/* Model axis c = eye axis c = row c of the world->eye matrix. */
	const float s = AERON_OPT_UNITS_PER_METER;
	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c)
			out[r * 4 + c] = camera_rows[c * 3 + r] * s;
		out[r * 4 + 3] = camera_local[r];
	}
	out[12] = out[13] = out[14] = 0.0f;
	out[15] = 1.0f;
}

void XwaVrRevivalCockpit_Shutdown(void) {
	for (int i = 0; i < COCKPIT_COUNT; ++i) {
		Cockpit* c = &cockpits[i];
		if (c->mesh)
			AeronScene_MeshDestroy(c->mesh);
		if (c->model) {
			Aeron_FlightModelFree(c->model);
			free(c->model);
		}
		c->mesh = NULL;
		c->model = NULL;
		c->failed = false;
	}
}
