#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <vector>


struct sPlyVertexXYZ_N_UV
{
	float x, y, z, nx, ny, nz, u, v;
};

struct sPlyTriangle
{
	unsigned int vert_index_1, vert_index_2, vert_index_3;
};

class cMesh
{
public:
	cMesh() {};
	~cMesh() {};	
	std::vector<sPlyVertexXYZ_N_UV> vecVertices;	
	std::vector<sPlyTriangle> vecTriangles;

	enum enumTEXCOORDBIAS
	{
		POSITIVE_X, POSITIVE_Y, POSITIVE_Z
	};

	void GenTextureCoordsSpherical(void);
	void GenTextureCoordsSpherical(enumTEXCOORDBIAS uBias, enumTEXCOORDBIAS vBias, bool basedOnNormals, float scale, bool fast);

};

#endif 