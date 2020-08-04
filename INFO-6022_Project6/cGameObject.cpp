#include "cGameObject.h"
#include <iostream>

#include <map>

extern glm::mat4 calculateWorldMatrix(cGameObject* pCurrentObject);
extern std::map<std::string, cMesh> mapNameToMesh;

cGameObject::cGameObject()
{	
	this->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	this->m_qRotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	this->scaleXYZ = glm::vec3(1.0f);
	this->diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	
	this->isWireframe = false;
	this->isVisible = true;
	this->useTransparentMap = false;
	this->noLighting = false;
	this->wireframeColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->textureBlendMode = NO_TEXTURE;
	this->physicsShapeType = UNKNOWN;
	this->SPHERE_radius = 1.0f;
	this->m_uniqueID = cGameObject::next_uniqueID;
	cGameObject::next_uniqueID++;
	this->disableDepthBufferTest = false;
	this->disableDepthBufferWrite = false;
	this->textures[0] = "Half_grey.bmp";
	this->textures[1] = "Half_grey.bmp";
	this->textures[2] = "White.bmp";		// Cutout alpha map, discard black pixel
	this->textures[3] = "White.bmp";		// Transparent map, white is opaque
	this->textureTile[0] = 1.0f;
	this->textureTile[1] = 1.0f;
	this->textureTile[2] = 1.0f;
	this->textureTile[3] = 1.0f;
	this->front = glm::vec3(0.0f, 0.0f, 1.0f);
	this->right = glm::vec3(-1.0f, 0.0f, 0.0f);
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->triggerActive = false;
	
	//std::cout << "A cGameObject() is created." << std::endl;
	return;
}

cGameObject::~cGameObject()
{
	std::cout << "A cGameObject() is destroyed." << std::endl;
	return;
}

unsigned int cGameObject::next_uniqueID = 1000;
unsigned int cGameObject::getUniqueID(void)
{
	return this->m_uniqueID;
}

glm::quat cGameObject::getQOrientation(void)
{
	return this->m_qRotation;
}

// Overwrite the orientation
void cGameObject::setOrientation(glm::vec3 EulerAngleDegreesXYZ)
{
	// c'tor of the glm quaternion converts Euler 
	//	to quaternion representation. 
	glm::vec3 EulerAngleRadians;
	EulerAngleRadians.x = glm::radians(EulerAngleDegreesXYZ.x);
	EulerAngleRadians.y = glm::radians(EulerAngleDegreesXYZ.y);
	EulerAngleRadians.z = glm::radians(EulerAngleDegreesXYZ.z);

	this->m_qRotation = glm::quat(EulerAngleRadians);
}

void cGameObject::setOrientation(glm::quat qAngle)
{
	this->m_qRotation = qAngle;
}

// Updates the existing angle
void cGameObject::updateOrientation(glm::vec3 EulerAngleDegreesXYZ)
{
	// Create a quaternion of this angle...
	glm::vec3 EulerAngleRadians;
	EulerAngleRadians.x = glm::radians(EulerAngleDegreesXYZ.x);
	EulerAngleRadians.y = glm::radians(EulerAngleDegreesXYZ.y);
	EulerAngleRadians.z = glm::radians(EulerAngleDegreesXYZ.z);

	glm::quat angleChange = glm::quat(EulerAngleRadians);

	// ...apply it to the exiting rotation
	this->m_qRotation *= angleChange;		// This multiply order will rotate according to local axis, not world axis
	//this->m_qRotation = angleChange * m_qRotation;
}

void cGameObject::updateOrientation(glm::quat qAngleChange)
{
	//this->m_qRotation *= qAngleChange;
	this->m_qRotation = qAngleChange * m_qRotation;
}

glm::vec3 cGameObject::getEulerAngle(void)
{
	// In glm::gtx (a bunch of helpful things there)
	glm::vec3 EulerAngle = glm::eulerAngles(this->m_qRotation);

	return EulerAngle;
}

void cGameObject::UpdateWorldMatrix()
{
	this->worldMatrix = calculateWorldMatrix(this);
	return;
}

void cGameObject::UpdateCollidePoints()
{
	UpdateWorldMatrix();

	this->vecUpdatedCollidePoints.clear();

	for (std::vector<glm::vec3>::iterator itCP = this->vecCollidePoints.begin();
		itCP != this->vecCollidePoints.end(); itCP++)
	{		
		this->vecUpdatedCollidePoints.push_back(glm::vec3(this->worldMatrix * glm::vec4(*itCP, 1.0f)));
	}

	return;
}

glm::vec3 cGameObject::getUpdatedFront()
{
	glm::mat4 matRotation = glm::toMat4(this->getQOrientation());

	glm::vec3 updatedFront = glm::normalize(glm::vec3(matRotation * glm::vec4(this->front, 1.0f)));

	return updatedFront;
}

glm::vec3 cGameObject::getUpdatedRight()
{
	glm::mat4 matRotation = glm::toMat4(this->getQOrientation());

	glm::vec3 updatedRight = glm::normalize(glm::vec3(matRotation * glm::vec4(this->right, 1.0f)));

	return updatedRight;
}

glm::vec3 cGameObject::getUpdatedUp()
{
	glm::mat4 matRotation = glm::toMat4(this->getQOrientation());

	glm::vec3 updatedUp = glm::normalize(glm::vec3(matRotation * glm::vec4(this->up, 1.0f)));

	return updatedUp;
}

void cGameObject::CalculateAABBMinMax()
{
	cMesh transformedGameobjectMesh;
	CalculateTransformedMesh(mapNameToMesh[this->meshName], this->worldMatrix, transformedGameobjectMesh);

	// See if we have any vertices loaded...
	if (transformedGameobjectMesh.vecVertices.size() == 0)
	{
		return;
	}

	float minX, minY, minZ, maxX, maxY, maxZ;

	// Assume the 1st vertex is the max and min (so we can compare)
	minX = maxX = transformedGameobjectMesh.vecVertices[0].x;
	minY = maxY = transformedGameobjectMesh.vecVertices[0].y;
	minZ = maxZ = transformedGameobjectMesh.vecVertices[0].z;

	for (unsigned int index = 0; index != transformedGameobjectMesh.vecVertices.size(); index++)
	{
		if (transformedGameobjectMesh.vecVertices[index].x < minX) { minX = transformedGameobjectMesh.vecVertices[index].x; }
		if (transformedGameobjectMesh.vecVertices[index].y < minY) { minY = transformedGameobjectMesh.vecVertices[index].y; }
		if (transformedGameobjectMesh.vecVertices[index].z < minZ) { minZ = transformedGameobjectMesh.vecVertices[index].z; }

		if (transformedGameobjectMesh.vecVertices[index].x > maxX) { maxX = transformedGameobjectMesh.vecVertices[index].x; }
		if (transformedGameobjectMesh.vecVertices[index].y > maxY) { maxY = transformedGameobjectMesh.vecVertices[index].y; }
		if (transformedGameobjectMesh.vecVertices[index].z > maxZ) { maxZ = transformedGameobjectMesh.vecVertices[index].z; }
	}

	//this->extentX = this->maxX - this->minX;
	//this->extentY = this->maxY - this->minY;
	//this->extentZ = this->maxZ - this->minZ;

	//this->maxExtent = this->extentX;
	//if (this->extentY > this->maxExtent) { this->maxExtent = this->extentY; }
	//if (this->extentZ > this->maxExtent) { this->maxExtent = this->extentZ; }

	this->AABB_min = glm::vec3(minX, minY, minZ);
	this->AABB_max = glm::vec3(maxX, maxY, maxZ);

	return;
}

void cGameObject::CalculateTransformedMesh(cMesh& originalMesh, glm::mat4 worldMatrix, cMesh& transformedMesh)
{
	transformedMesh = originalMesh;

	for (std::vector<sPlyVertexXYZ_N_UV>::iterator itVert = transformedMesh.vecVertices.begin();
		itVert != transformedMesh.vecVertices.end(); itVert++)
	{
		glm::vec4 vertex = glm::vec4(itVert->x, itVert->y, itVert->z, 1.0f);
		glm::vec4 transformedVertex = worldMatrix * vertex;

		itVert->x = transformedVertex.x;
		itVert->y = transformedVertex.y;
		itVert->z = transformedVertex.z;

		// normal need inverse transpose world matrix
		glm::vec4 normal = glm::vec4(itVert->nx, itVert->ny, itVert->nz, 1.0f);
		glm::vec4 transformedNormal = glm::inverse(glm::transpose(worldMatrix)) * normal;

		itVert->nx = transformedNormal.x;
		itVert->ny = transformedNormal.y;
		itVert->nz = transformedNormal.z;
	}
	return;
}