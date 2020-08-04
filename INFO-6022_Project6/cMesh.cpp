#include "cMesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

void cMesh::GenTextureCoordsSpherical(void)
{
	// Not based on normals.
	// Scale is 1.0f (0.0f to 1.0f U and V values)
	this->GenTextureCoordsSpherical(cMesh::POSITIVE_X, cMesh::POSITIVE_Y, false, 1.0f, false);

	return;
}

void cMesh::GenTextureCoordsSpherical(enumTEXCOORDBIAS uBias, enumTEXCOORDBIAS vBias, bool basedOnNormals, float scale, bool fast)
{
	// determine the 'remaining' coordinate...
	bool xUsed = false;
	bool yUsed = false;
	bool zUsed = false;
	if (uBias == POSITIVE_X || vBias == POSITIVE_X)	xUsed = true;
	if (uBias == POSITIVE_Y || vBias == POSITIVE_Y)	yUsed = true;
	if (uBias == POSITIVE_Z || vBias == POSITIVE_Z)	yUsed = true;

	std::vector<sPlyVertexXYZ_N_UV>::iterator itVert;
	for (itVert = this->vecVertices.begin(); itVert != this->vecVertices.end(); itVert++)
	{
		glm::vec3 xyz;
		if (basedOnNormals)
		{
			if (uBias == POSITIVE_X)		xyz.x = itVert->nx;
			else if (uBias == POSITIVE_Y)	xyz.x = itVert->ny;
			else if (uBias == POSITIVE_Z)	xyz.x = itVert->nz;

			if (vBias == POSITIVE_X)		xyz.y = itVert->nx;
			else if (vBias == POSITIVE_Y)	xyz.y = itVert->ny;
			else if (vBias == POSITIVE_Z)	xyz.y = itVert->nz;

			// Fill in the remaining coordinate...
			if (!xUsed)	xyz.z = itVert->nx;
			if (!yUsed)	xyz.z = itVert->ny;
			if (!zUsed)	xyz.z = itVert->nz;
		}
		else
		{
			if (uBias == POSITIVE_X)		xyz.x = itVert->x;
			else if (uBias == POSITIVE_Y)	xyz.x = itVert->y;
			else if (uBias == POSITIVE_Z)	xyz.x = itVert->z;

			if (vBias == POSITIVE_X)		xyz.y = itVert->x;
			else if (vBias == POSITIVE_Y)	xyz.y = itVert->y;
			else if (vBias == POSITIVE_Z)	xyz.y = itVert->z;

			// Fill in the remaining coordinate...
			if (!xUsed)	xyz.z = itVert->x;
			if (!yUsed)	xyz.z = itVert->y;
			if (!zUsed)	xyz.z = itVert->z;
		}

		xyz = glm::normalize(xyz);

		if (fast)
		{
			itVert->u = ((xyz.x / 2.0f) + 0.5f) * scale;
			itVert->v = ((xyz.y / 2.0f) + 0.5f) * scale;
		}
		else
		{
			itVert->u = ((asin(xyz.x) / glm::pi<float>()) + 0.5f) * scale;
			itVert->v = ((asin(xyz.y) / glm::pi<float>()) + 0.5f) * scale;
		}
	}

	return;
}
