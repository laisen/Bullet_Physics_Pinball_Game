#include "cVAOManager.h"
#include "GLCommon.h"
#include <iostream>

cVAOManager::cVAOManager()
{
	std::cout << "A cVAOManager() is created." << std::endl;
	return;
}

cVAOManager::~cVAOManager()
{
	std::cout << "A cVAOManager() is destroyed." << std::endl;
	return;
}

sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;
	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;
	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;	
	this->pVertices = 0;
	this->pIndices = 0;
	return;
}

bool cVAOManager::LoadModelIntoVAO(
	std::string fileName,
	cMesh& theMesh,
	sModelDrawInfo& drawInfo,
	unsigned int shaderProgramID)

{
	// cMesh& theMesh to sModelDrawInfo& drawInfo
	drawInfo.numberOfVertices = (unsigned int)theMesh.vecVertices.size();
	drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];

	for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
	{

		drawInfo.pVertices[index].x = theMesh.vecVertices[index].x;
		drawInfo.pVertices[index].y = theMesh.vecVertices[index].y;
		drawInfo.pVertices[index].z = theMesh.vecVertices[index].z;
		drawInfo.pVertices[index].w = 1.0f;

		drawInfo.pVertices[index].r = 1.0f;
		drawInfo.pVertices[index].g = 1.0f;
		drawInfo.pVertices[index].b = 1.0f;
		drawInfo.pVertices[index].a = 1.0f;

		drawInfo.pVertices[index].nx = theMesh.vecVertices[index].nx;
		drawInfo.pVertices[index].ny = theMesh.vecVertices[index].ny;
		drawInfo.pVertices[index].nz = theMesh.vecVertices[index].nz;
		drawInfo.pVertices[index].nw = 1.0f;
		
		drawInfo.pVertices[index].u0 = theMesh.vecVertices[index].u;
		drawInfo.pVertices[index].v0 = theMesh.vecVertices[index].v;
		drawInfo.pVertices[index].u1 = 1.0f;
		drawInfo.pVertices[index].v1 = 1.0f;
	}

	// copy the index information
	drawInfo.numberOfTriangles = (unsigned int)theMesh.vecTriangles.size();
	drawInfo.numberOfIndices = (unsigned int)theMesh.vecTriangles.size() * 3;

	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

	unsigned int indexTri = 0;
	unsigned int indexIndex = 0;
	for (; indexTri != drawInfo.numberOfTriangles; indexTri++, indexIndex += 3)
	{
		drawInfo.pIndices[indexIndex + 0] = (unsigned int)theMesh.vecTriangles[indexTri].vert_index_1;
		drawInfo.pIndices[indexIndex + 1] = (unsigned int)theMesh.vecTriangles[indexTri].vert_index_2;
		drawInfo.pIndices[indexIndex + 2] = (unsigned int)theMesh.vecTriangles[indexTri].vert_index_3;
	}

	drawInfo.meshName = fileName;
	
	glGenVertexArrays(1, &(drawInfo.VAO_ID));		// new buffer ID	
	glBindVertexArray(drawInfo.VAO_ID);		// use this VAO buffer

	glGenBuffers(1, &(drawInfo.VertexBufferID));

	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);

	glBufferData(GL_ARRAY_BUFFER,
		sizeof(sVertex) * drawInfo.numberOfVertices,
		(GLvoid*)drawInfo.pVertices,
		GL_STATIC_DRAW);

	// Copy the index buffer
	glGenBuffers(1, &(drawInfo.IndexBufferID));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * drawInfo.numberOfIndices,
		(GLvoid*)drawInfo.pIndices,
		GL_STATIC_DRAW);

	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPosition");
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vColour");
	GLint vnorm_location = glGetAttribLocation(shaderProgramID, "vNormal");
	GLint vUV_location = glGetAttribLocation(shaderProgramID, "vUVx2");

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 4,
		GL_FLOAT, GL_FALSE,
		sizeof(sVertex),
		(void*)(offsetof(sVertex, x)));

	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 4,
		GL_FLOAT, GL_FALSE,
		sizeof(sVertex),
		(void*)(offsetof(sVertex, r)));

	//	float nx, ny, nz, nw;
	glEnableVertexAttribArray(vnorm_location);
	glVertexAttribPointer(vnorm_location, 4,
		GL_FLOAT, GL_FALSE,
		sizeof(sVertex),
		(void*)(offsetof(sVertex, nx)));

	//	float u0, v0, u1, v1;
	glEnableVertexAttribArray(vUV_location);
	glVertexAttribPointer(vUV_location, 4,
		GL_FLOAT, GL_FALSE,
		sizeof(sVertex),
		(void*)(offsetof(sVertex, u0)));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vcol_location);

	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;

	return true;
}

bool cVAOManager::FindDrawInfoByModelName(std::string filename,	sModelDrawInfo& drawInfo)
{
	std::map< std::string /*model name*/, sModelDrawInfo >::iterator
		itDrawInfo = this->m_map_ModelName_to_VAOID.find(filename);

	if (itDrawInfo == this->m_map_ModelName_to_VAOID.end())
	{		
		return false;
	}

	drawInfo = itDrawInfo->second;
	return true;
}

void sModelDrawInfo::CalcExtents(void)
{
	// See if we have any vertices loaded...
	if ((this->numberOfVertices == 0) || (this->pVertices == 0))
	{
		return;
	}

	// We're good

	// Assume the 1st vertex is the max and min (so we can compare)
	this->minX = this->maxX = this->pVertices[0].x;
	this->minY = this->maxY = this->pVertices[0].y;
	this->minZ = this->maxZ = this->pVertices[0].z;

	for (unsigned int index = 0; index != this->numberOfVertices; index++)
	{
		if (this->pVertices[index].x < this->minX) { this->minX = this->pVertices[index].x; }
		if (this->pVertices[index].y < this->minY) { this->minY = this->pVertices[index].y; }
		if (this->pVertices[index].z < this->minZ) { this->minZ = this->pVertices[index].z; }

		if (this->pVertices[index].x > this->maxX) { this->maxX = this->pVertices[index].x; }
		if (this->pVertices[index].y > this->maxY) { this->maxY = this->pVertices[index].y; }
		if (this->pVertices[index].z > this->maxZ) { this->maxZ = this->pVertices[index].z; }
	}

	this->extentX = this->maxX - this->minX;
	this->extentY = this->maxY - this->minY;
	this->extentZ = this->maxZ - this->minZ;

	this->maxExtent = this->extentX;
	if (this->extentY > this->maxExtent) { this->maxExtent = this->extentY; }
	if (this->extentZ > this->maxExtent) { this->maxExtent = this->extentZ; }

	return;
}
