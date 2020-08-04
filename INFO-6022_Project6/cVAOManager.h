#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

#include <string>
#include "cMesh.h"
#include <map>

struct sVertex
{
	float x, y, z, w;
	float r, g, b, a;
	float nx, ny, nz, nw;
	float u0, v0, u1, v1;
};

struct sModelDrawInfo
{
	sModelDrawInfo();

	std::string meshName;
	unsigned int VAO_ID;
	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;
	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;	
	sVertex* pVertices;		// temporary array(CPU side)	
	unsigned int* pIndices;		// The index buffer (CPU side)

	float maxX, maxY, maxZ;
	float minX, minY, minZ;
	float maxExtent;
	float extentX, extentY, extentZ;

	void CalcExtents(void);
};

class cVAOManager
{
public:
	cVAOManager();
	~cVAOManager();

	bool LoadModelIntoVAO(std::string fileName,
		cMesh& theMesh,
		sModelDrawInfo& drawInfo,
		unsigned int shaderProgramID);
	
	bool FindDrawInfoByModelName(std::string filename, sModelDrawInfo& drawInfo);	

private:
	std::map< std::string /*model name*/, sModelDrawInfo > m_map_ModelName_to_VAOID;
};

#endif