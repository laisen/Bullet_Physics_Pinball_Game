#include "cParticleEmitter.h"
#include <algorithm>		// for std::copy()
extern glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

cParticle::cParticle()
{
	// Note: we aren't really using these default values...
	this->location = glm::vec3(0.0f,0.0f,0.0f);
	this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->lifeTime = 0.0f;
	this->colourRGBA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	// 
	this->qOrientation = glm::quat( glm::vec3(0.0f,0.0f,0.0f));
	this->scale = 1.0f;
	this->scaleChangePercentPerSecond = 0.0f;

	this->qRotationalChangePerSecond = glm::quat(glm::vec3(0.0f,0.0f,0.0f));
	
	this->bImposterFacesCamera = false;
	this->textureName = "";

	// Default is no change over time
	this->alphaChangePerSecond = 0.0f;

	return;
}

cParticleEmitter::sParticleCreationSettings::sParticleCreationSettings()
{
	this->minLifeSeconds = this->maxLifeInSeconds = 0.0f;
	this->numParticlesPerSecond = 0;
	this->minVelocity = this->maxVelocity = glm::vec3(0.0f);
	this->particleCreationVolume = AABB;
	this->minDeltaAABBPosition = this->maxDeltaAABBPosition = glm::vec3(0.0f);
	this->minDeltaRadiusPosition = this->maxDeltaRadiusPosition = 0.0f;
	this->minStartingScale = this->maxStartingScale = 1.0f;
	this->minScaleChangePercentPerSecond = this->maxScaleChangePercentPerSecond = 0.0f;
	// Rotational change is 0 degrees on all axes
	this->minRotationalChangePerSecond = glm::quat(glm::vec3(0.0f,0.0f,0.0f));
	this->maxRotationalChangePerSecond = glm::quat(glm::vec3(0.0f,0.0f,0.0f));
	this->isImposterFaceCamera = false;
	this->bFadeOutOverLifetime = false;
	return;
}



cParticleEmitter::cParticleEmitter()
{
	// NOTE: I'm NOT setting up the particles here.
	// That will happen in initialize()
	this->location = glm::vec3(0.0f,0.0f,0.0f);
	this->acceleration = glm::vec3(0.0f,0.0f,0.0f);

	this->m_generateNewParticles = false;
	this->m_timeSinceLastParticle = 0.0f;
	this->m_particleCreationPeriod = FLT_MAX;	// Infinite time until new particles (so won't make any)
	return;
}


void cParticleEmitter::Step(double deltaTime)
{
	// Loop through the particles:
	// - Decrease the life by "deltaTime"
	// - If "still alive" (lifeTime > 0.0):
	//    - Move the particle



	for ( std::vector<cParticle*>::iterator itPart = this->m_vec_pParticles.begin();
		  itPart != this->m_vec_pParticles.end();  itPart++ )
	{
		cParticle* pCurParticle = *itPart;	// Dereference iterator, giving us the original type

		pCurParticle->lifeTime -= (float)deltaTime;

		// Is it dead, Jim? (Star Trek reference)
		if ( pCurParticle->lifeTime > 0.0f )
		{ 
			// Update velocity from acceler
			pCurParticle->velocity += ( this->acceleration * (float)deltaTime );

			// Update position from velocity
			pCurParticle->location += ( pCurParticle->velocity * (float)deltaTime );

			// Change scale. 
			pCurParticle->scale *= (1.0f + ((pCurParticle->scaleChangePercentPerSecond * (float)deltaTime)));

			// Rotate particle
			// We are using SLERP to make a rotational change from 0 degrees (all axes) 
			//  to the maximum rotation angle per second.
			glm::quat rotChange = glm::slerp( glm::quat(glm::vec3(0.0f,0.0f,0.0f)), 
											  pCurParticle->qRotationalChangePerSecond, 
											  (float)deltaTime );

			pCurParticle->qOrientation *= rotChange;

			// Does this particle fade to black over time
			pCurParticle->colourRGBA.a -= pCurParticle->alphaChangePerSecond * (float)deltaTime;

		}// if ( pCurParticle->lifeTime > 0.0f )

	}//for ( std::vector<cParticle*>::iterator itPart


	// How many new particles should we create this frame? 
	// Create new particles?
	if (this->m_generateNewParticles)
	{
		this->m_timeSinceLastParticle += deltaTime;

		if (this->m_timeSinceLastParticle >= this->m_particleCreationPeriod)
		{
			// Create a new particle
			this->m_CreateNewParticle();
			// Reset the timer
			this->m_timeSinceLastParticle = 0.0f;
		}
//		int numParticleToCreate = randInRange<int>(this->m_NewParticleSettings.min, this->m_maxParticlesPerFrame);
//
//		for (int count = 0; count != numParticleToCreate; count++)
//		{
//			this->m_CreateNewParticle( this->m_minVelocity, this->m_maxVelocity, 
//									   this->m_minDeltaPosition, this->m_maxDeltaPosition,
//									   this->m_minLifeSeconds, this->m_maxLifeSeconds );
//		}//for (int count = 0;
	}//if (this->m_generateNewParticles)



	return;
}

void cParticleEmitter::Initialize(cParticleEmitter::sParticleCreationSettings collisionParticleSettings)
{
	// In case we are calling Initialize() again... 
	// Clear all the existing particles
	// TODO: 
	// - If the new size is LARGER than the current vector, 
	//   allocate the extra space and call new
	// - If the new size is SMALLER than the current vector, 
	//   go though and call delete on all the "extra" particles. 

//	this->m_vec_pParticles.clear();

	// Load the vector with the maximum number of particles
	// Reserve doesn't create the objects, it just sets asside space in 1 step
	this->m_vec_pParticles.reserve(cParticleEmitter::DEFAULT_NUM_PARTICLES);

	for (int count = 0; count != cParticleEmitter::DEFAULT_NUM_PARTICLES; count++)
	{
		// TODO: If we are calling Initialize() AGAIN, 
		//       then we DON'T want to call new
		cParticle* pNewParticle = new cParticle();

		// Set the lifetime to 0.0 for all of them
		pNewParticle->lifeTime = 0.0f;

		this->m_vec_pParticles.push_back(pNewParticle);

	}

	this->m_NewParticleSettings = collisionParticleSettings;

	// Calculate now often we should create particles...
	this->m_particleCreationPeriod = 1.0f / (float)this->m_NewParticleSettings.numParticlesPerSecond;

	return;
}



void cParticleEmitter::m_CreateNewParticle(void)
{
	// Find the 1st particle that has life <= 0.0f
	for (std::vector<cParticle*>::iterator itPart = this->m_vec_pParticles.begin();
		 itPart != this->m_vec_pParticles.end();  itPart++)
	{
		// Is this particle "dead"? 
		cParticle* pCurParticle = *itPart;

		if (pCurParticle->lifeTime <= 0.0f)
		{
			// Make it "alive" and set to some values
			pCurParticle->lifeTime = randInRange<float>(this->m_NewParticleSettings.minLifeSeconds,
														this->m_NewParticleSettings.maxLifeInSeconds);

			// Set the initial velocity
			pCurParticle->velocity.x = randInRange<float>(this->m_NewParticleSettings.minVelocity.x,
														  this->m_NewParticleSettings.maxVelocity.x);
			pCurParticle->velocity.y = randInRange<float>(this->m_NewParticleSettings.minVelocity.y,
														  this->m_NewParticleSettings.maxVelocity.y);
			pCurParticle->velocity.z = randInRange<float>(this->m_NewParticleSettings.minVelocity.z,
														  this->m_NewParticleSettings.maxVelocity.z);
			//pCurParticle->velocity = randInRange<glm::vec3>(minVelocity, maxVelocity);

			pCurParticle->scale = randInRange<float>(this->m_NewParticleSettings.minStartingScale,
													 this->m_NewParticleSettings.maxStartingScale);

			pCurParticle->scaleChangePercentPerSecond = randInRange<float>(this->m_NewParticleSettings.minScaleChangePercentPerSecond,
																		   this->m_NewParticleSettings.maxScaleChangePercentPerSecond);

			// Use SLERP to pick a rotational change per second
			pCurParticle->qRotationalChangePerSecond = glm::slerp(this->m_NewParticleSettings.minRotationalChangePerSecond,
																  this->m_NewParticleSettings.maxRotationalChangePerSecond,
																  randInRange<float>(0.0f, 1.0f));

			// Pick a location based on the creation volume
			switch (this->m_NewParticleSettings.particleCreationVolume)
			{
			case cParticleEmitter::sParticleCreationSettings::AABB:
				pCurParticle->location.x = randInRange<float>(this->m_NewParticleSettings.minDeltaAABBPosition.x,
															  this->m_NewParticleSettings.maxDeltaAABBPosition.x);
				pCurParticle->location.y = randInRange<float>(this->m_NewParticleSettings.minDeltaAABBPosition.y,
															  this->m_NewParticleSettings.maxDeltaAABBPosition.y);
				pCurParticle->location.z = randInRange<float>(this->m_NewParticleSettings.minDeltaAABBPosition.z,
															  this->m_NewParticleSettings.maxDeltaAABBPosition.z);

				pCurParticle->location += this->location;

				break;
			case cParticleEmitter::sParticleCreationSettings::SPHERE:
				// Pick a random point and check if it's inside a sphere
				bool bKeepPickingPoints = true;
				while (bKeepPickingPoints)
				{
					pCurParticle->location.x = randInRange<float>(this->m_NewParticleSettings.minDeltaAABBPosition.x,
																  this->m_NewParticleSettings.maxDeltaAABBPosition.x);
					pCurParticle->location.y = randInRange<float>(this->m_NewParticleSettings.minDeltaAABBPosition.y,
																  this->m_NewParticleSettings.maxDeltaAABBPosition.y);
					pCurParticle->location.z = randInRange<float>(this->m_NewParticleSettings.minDeltaAABBPosition.z,
																  this->m_NewParticleSettings.maxDeltaAABBPosition.z);

					if (glm::distance(glm::vec3(0.0f,0.0f,0.0f), pCurParticle->location) >= this->m_NewParticleSettings.minDeltaRadiusPosition)
					{
						if (glm::distance(glm::vec3(0.0f, 0.0f, 0.0f), pCurParticle->location) <= this->m_NewParticleSettings.maxDeltaRadiusPosition)
						{
							// We're good
							pCurParticle->location += this->location;
							bKeepPickingPoints = false;
						}
					}//if (glm::distance...
				
				}//while (bKeepPickingPoints)

				break;
			}
			// TODO: Adjust position based on Max and Min delta postiion
			// (For now, just make the particle the same as the emitter)
			pCurParticle->location = this->location;


			pCurParticle->bImposterFacesCamera = this->m_NewParticleSettings.isImposterFaceCamera;

			pCurParticle->alphaChangePerSecond = 0.0f;
			if ( this->m_NewParticleSettings.bFadeOutOverLifetime )
			{
				// Calculate how fast the alpha value should change
				pCurParticle->alphaChangePerSecond = 1.0f / pCurParticle->lifeTime;
			}

			// Reset transparency
			pCurParticle->colourRGBA.a = 1.0f;

			// All done, so return
			return;

		}//if (pCurParticle->lifeTime <= 0.0f)

	}//for (std::vector<cParticle*>

	// This is if we didn't find any particles to recycle
	return;
}


// If the particle is an imposter
glm::quat cParticleEmitter::m_calcImposterRotationFromEye(glm::vec3 particlePos, glm::vec3 eye)
{
	// Angle returned is in radian already...
	//float angleY = atan2(particlePos.x - eye.x,	particlePos.z - eye.z);

	glm::quat magicRotation = RotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f),
		glm::normalize(eye - particlePos));

	//* ( 180.0 / PI );
// Since we aren't paying any attention to the x and z rotations.
	//return glm::quat( glm::vec3(0.0f, angleY, 0.0f) );
	return glm::quat(magicRotation);
}


void cParticleEmitter::getParticles(std::vector<cParticle*>& vec_pParticles, 
									glm::vec3 eyePosition /*=glm::vec3(0.0f, 0.0f, 0.0f)*/)
{
	// Copy the internal vector of particles to the one that is passed.

	vec_pParticles.clear();
	vec_pParticles.reserve( this->m_vec_pParticles.size() );
//	// Sets the pointer values to zero (NULL)
//	vec_pParticles.resize( this->m_vec_pParticles.size(), 0 );

	// NOTE: There has to be space there, 
	// AND the objects have to be initialized already
//	std::copy( this->m_vec_pParticles.begin(), 
//			   this->m_vec_pParticles.end(), 
//			   vec_pParticles.begin() );

	for (std::vector<cParticle*>::iterator itPart = this->m_vec_pParticles.begin();
		 itPart != this->m_vec_pParticles.end(); itPart++)
	{
		// If alive, copy over
		cParticle* pCurPart = *itPart;

		// Adjust the orientation if it's an imposter. 
		// Have it face the camera
		if (pCurPart->bImposterFacesCamera)
		{
			pCurPart->qOrientation = this->m_calcImposterRotationFromEye(pCurPart->location, eyePosition);
		}


		if (pCurPart->lifeTime > 0.0f)
		{
			vec_pParticles.push_back( pCurPart );
		}
	}

	return;
}
