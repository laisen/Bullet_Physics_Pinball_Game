#include "cModelLoader.h"
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

cModelLoader::cModelLoader()
{
	std::cout << "A cModelLoader() is created." << std::endl;
	return;
}

cModelLoader::~cModelLoader()
{
	std::cout << "A cModelLoader() is destroyed." << std::endl;
	return;
}

bool cModelLoader::LoadPlyModel(std::string filename, cMesh &theMesh)
{
	std::ifstream meshFile(filename.c_str());
	if (!meshFile.is_open())
	{
		std::cout << "cModelLoader::LoadPlyModel() can't find the model file." << std::endl;
		return false;
	}
	
	std::string currentString;
	while (meshFile >> currentString)
	{		
		if (currentString == "vertex")
		{
			break;
		}
	}

	unsigned int numberOfVertices;
	meshFile >> numberOfVertices;

	while (meshFile >> currentString)
	{		
		if (currentString == "face")
		{
			break;
		}
	}

	unsigned int numberOfTriangles;
	meshFile >> numberOfTriangles;

	while (meshFile >> currentString)
	{		
		if (currentString == "end_header")
		{
			break;
		}
	}
	
	for (unsigned int index = 0; index != numberOfVertices; index++)
	{		
		sPlyVertexXYZ_N_UV currentStringVertex;		
		meshFile >> currentStringVertex.x >> currentStringVertex.y >> currentStringVertex.z;
		
		glm::vec3 normalized;
		meshFile >> normalized.x >> normalized.y >> normalized.z;
		normalized = glm::normalize(normalized);
		currentStringVertex.nx = normalized.x;
		currentStringVertex.ny = normalized.y;
		currentStringVertex.nz = normalized.z;

		meshFile >> currentStringVertex.u >> currentStringVertex.v;
		
		theMesh.vecVertices.push_back(currentStringVertex);
	}

	for (unsigned int index = 0; index != numberOfTriangles; index++)
	{		
		int discardNumber;
		sPlyTriangle currentStringTriangle;
		meshFile >> discardNumber >> currentStringTriangle.vert_index_1 >> currentStringTriangle.vert_index_2 >> currentStringTriangle.vert_index_3;
		
		theMesh.vecTriangles.push_back(currentStringTriangle);
	}

	return true;
}

