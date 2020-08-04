#ifndef _cModelLoader_HG_
#define _cModelLoader_HG_

#include <string>
#include "cMesh.h"

class cModelLoader
{
public:
	cModelLoader();
	~cModelLoader();
	bool LoadPlyModel(std::string filename,	cMesh &theMesh);
	bool LoadModel_Assimp(std::string filename,	cMesh& theMesh,	std::string& errors);

};

#endif