#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include "cMesh.h"
#include <iPhysicsComponent.h>
#include <time.h>

enum eShapeTypes
{
	AABB,
	SPHERE,
	CAPSULE,
	PLANE,
	MESH,
	BOX,
	CYLINDER,
	SPINNER,
	TRIGGER,
	FLIPPER,
	PLUNGER,
	UNKNOWN
};

enum eTextureBlend
{
	NO_TEXTURE,		// Plain diffuse colour only, no blend
	BASE/*Albedo*/,		// Show the 1st textures[0] only, no blend
	DIFFUSE_COLOUR,		// Use diffuse colour to colour blend base texture textures[0]
	COLOUR,		// Use 2nd textures[1] to colour blend base texture textures[0]
	MULTIPLY,		// Use 2nd textures[1] to multiply blend base texture textures[0]
	AVERAGE,		// Use half 2nd textures[1] to add blend half base texture textures[0]
	SOFT_LIGHT,		// Use 2nd textures[1] to soft light blend base texture textures[0]
	IMPOSTER,		// Base texture with black colour discarded
	CUBE_MAP		// Skybox
};

class cGameObject
{
public:
	cGameObject();
	~cGameObject();
	
	std::string meshName;
	std::string friendlyName;
	glm::vec3 positionXYZ;
	glm::vec3 scaleXYZ;
	glm::vec4 diffuseColour;
	glm::mat4 worldMatrix;		
	bool isWireframe;
	bool isVisible;
	bool useTransparentMap;
	bool noLighting;
	glm::vec4 wireframeColour;
	eTextureBlend textureBlendMode;

	nPhysics::iPhysicsComponent* physicsComponent;
	bool triggerActive;
	eShapeTypes physicsShapeType;
	glm::vec3 AABB_min, AABB_max;
	float SPHERE_radius;
	std::vector<glm::vec3> vecCollidePoints;
	std::vector<glm::vec3> vecUpdatedCollidePoints;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
	
	bool disableDepthBufferTest;
	bool disableDepthBufferWrite;

	static const int NUMBEROFTEXTURES = 4;
	std::string textures[NUMBEROFTEXTURES];
	float textureTile[NUMBEROFTEXTURES];

	clock_t timeStamp;

	glm::quat getQOrientation();
	// Overwrite the orientation
	void setOrientation(glm::vec3 EulerAngleDegreesXYZ);
	void setOrientation(glm::quat qAngle);
	// Updates the existing angle
	void updateOrientation(glm::vec3 EulerAngleDegreesXYZ);
	void updateOrientation(glm::quat qAngle);
	glm::vec3 getEulerAngle();

	void CalculateAABBMinMax();
	void CalculateTransformedMesh(cMesh& originalMesh, glm::mat4 worldMatrix, cMesh& transformedMesh);

	void UpdateWorldMatrix();

	void UpdateCollidePoints();

	glm::vec3 getUpdatedFront();
	glm::vec3 getUpdatedRight();
	glm::vec3 getUpdatedUp();

	unsigned int getUniqueID();

private:
	glm::quat m_qRotation;
	
	static unsigned int next_uniqueID;
	unsigned int m_uniqueID;
};

#endif