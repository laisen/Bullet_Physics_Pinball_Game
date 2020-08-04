#pragma once
// cParticleEmitter
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>		// The quaternion class
#include <vector>
#include <string>

class cParticle
{
public:
	cParticle();
	glm::vec3 location;
	glm::vec3 velocity;
	float lifeTime;
	glm::vec4 colourRGBA;	// Colour and transparency
	// 
	// glm::vec orientation
	glm::quat qOrientation;	// To face the camera ("imposter")
	glm::quat qRotationalChangePerSecond;	// For imposters, this is around the z axis
	float scale;
	float scaleChangePercentPerSecond;
	// This is calculated when the particle is created
	float alphaChangePerSecond;
	std::string textureName;
	bool bImposterFacesCamera;
};

class cParticleEmitter
{
public:
	cParticleEmitter();
	glm::vec3 acceleration;
	glm::vec3 location;

	struct sParticleCreationSettings
	{
		sParticleCreationSettings();
		float numParticlesPerSecond;
		float minLifeSeconds; float maxLifeInSeconds;
		glm::vec3 minVelocity;	glm::vec3 maxVelocity;
		enum eCreationVolume
		{
			AABB,		// Default
			SPHERE
		};
		glm::vec3 minDeltaAABBPosition;	glm::vec3 maxDeltaAABBPosition;		// AABB
		float minDeltaRadiusPosition;	float maxDeltaRadiusPosition;		// SPHERE
		eCreationVolume particleCreationVolume;
		float minStartingScale; float maxStartingScale; 
		float minScaleChangePercentPerSecond; float maxScaleChangePercentPerSecond;
		glm::quat minRotationalChangePerSecond; glm::quat maxRotationalChangePerSecond;

		// This will change the alpha value of the object over time.
		// from 1.0 at the start to 0.0 at the final lifetime
		bool bFadeOutOverLifetime;

		bool isImposterFaceCamera;
	};

	void Initialize(sParticleCreationSettings collisionParticleSettings);
	// Called every frame
	void Step(double deltaTime);
	// This will be called when we want to draw them
	// Note: returns nothing and passed by reference ("&")
	void getParticles( std::vector<cParticle*> &vec_pParticles, 
					   glm::vec3 eyePosition = glm::vec3(0.0f, 0.0f, 0.0f) );

	void enableNewParticles(void) { this->m_generateNewParticles = true; }
	void disableNewParticles(void) { this->m_generateNewParticles = false; }

private:

	double m_timeSinceLastParticle;
	// This is the inverse of number of particles per second	
	double m_particleCreationPeriod;		

	bool m_generateNewParticles;

	// If the particle is an imposter
	glm::quat m_calcImposterRotationFromEye(glm::vec3 particlePos, glm::vec3 eye);

	// Static consts can be set here, static can't
	static const unsigned int DEFAULT_NUM_PARTICLES = 1000;

	std::vector<cParticle*> m_vec_pParticles;

	// Taken from line 229 of the DebugRenderer:
	template <class T>
	T randInRange(T min, T max)
	{
		double value =
			min + static_cast <double> (rand())
			/ (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));
		return static_cast<T>(value);
	};

	// Creates a single particle (called by Initialize and Step)
	void m_CreateNewParticle(void);

	// Saved for when we call Step()
	// These are set when we call Initialize();

	sParticleCreationSettings m_NewParticleSettings;
};