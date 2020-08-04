#pragma once

#include "GLCommon.h"
#include <glm/mat4x4.hpp>
#include "cGameObject.h"
#include "cVAOManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern cBasicTextureManager* g_pTextureManager;
extern bool wireframeMode;
extern bool normalPassMode;
extern bool albedoPassMode;

glm::mat4 calculateWorldMatrix(cGameObject* pCurrentObject)
{
	glm::mat4 m = glm::mat4(1.0f);

	// TRANSLATION
	glm::mat4 matTrans = glm::translate(glm::mat4(1.0f),
			glm::vec3(pCurrentObject->positionXYZ.x, pCurrentObject->positionXYZ.y,	pCurrentObject->positionXYZ.z));
	m = m * matTrans;

	// ROTATION
	//glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
	//	glm::radians(pCurrentObject->rotationXYZ.z),
	//	glm::vec3(0.0f, 0.0f, 1.0f));
	//m = m * rotateZ;

	//glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
	//	glm::radians(pCurrentObject->rotationXYZ.y),
	//	glm::vec3(0.0f, 1.0f, 0.0f));
	//m = m * rotateY;

	//glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
	//	glm::radians(pCurrentObject->rotationXYZ.x),
	//	glm::vec3(1.0f, 0.0f, 0.0f));
	//m = m * rotateX;
	glm::mat4 matRotation = glm::mat4(pCurrentObject->getQOrientation());
	m = m * matRotation;

	// SCALE	
	glm::mat4 scale = glm::scale(glm::mat4(1.0f),
		glm::vec3(pCurrentObject->scaleXYZ.x, pCurrentObject->scaleXYZ.y, pCurrentObject->scaleXYZ.z));
	m = m * scale;

	return m;
}

void SetUpTextureBindingsForObject(cGameObject* pCurrentObject, GLint shaderProgID)
{

	//// Tie the texture to the texture unit
	//GLuint texSamp0_UL = g_pTextureManager->getTextureIDFromName("Pizza.bmp");
	//glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
	//glBindTexture(GL_TEXTURE_2D, texSamp0_UL);	// Texture now assoc with texture unit 0

	// Tie the texture to the texture unit
	GLuint texSamp0_UL = g_pTextureManager->getTextureIDFromName(pCurrentObject->textures[0]);
	glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, texSamp0_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp1_UL = g_pTextureManager->getTextureIDFromName(pCurrentObject->textures[1]);
	glActiveTexture(GL_TEXTURE1);				// Texture Unit 1
	glBindTexture(GL_TEXTURE_2D, texSamp1_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp2_UL = g_pTextureManager->getTextureIDFromName(pCurrentObject->textures[2]);
	glActiveTexture(GL_TEXTURE2);				// Texture Unit 2
	glBindTexture(GL_TEXTURE_2D, texSamp2_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp3_UL = g_pTextureManager->getTextureIDFromName(pCurrentObject->textures[3]);
	glActiveTexture(GL_TEXTURE3);				// Texture Unit 3
	glBindTexture(GL_TEXTURE_2D, texSamp3_UL);	// Texture now assoc with texture unit 0

	// Tie the texture units to the samplers in the shader
	GLint textSamp00_UL = glGetUniformLocation(shaderProgID, "textSamp00");
	glUniform1i(textSamp00_UL, 0);	// Texture unit 0

	GLint textSamp01_UL = glGetUniformLocation(shaderProgID, "textSamp01");
	glUniform1i(textSamp01_UL, 1);	// Texture unit 1

	GLint textSamp02_UL = glGetUniformLocation(shaderProgID, "textSamp02");
	glUniform1i(textSamp02_UL, 2);	// Texture unit 2

	GLint textSamp03_UL = glGetUniformLocation(shaderProgID, "textSamp03");
	glUniform1i(textSamp03_UL, 3);	// Texture unit 3


	//GLint tex0_ratio_UL = glGetUniformLocation(shaderProgID, "tex_0_3_ratio");
	//glUniform4f(tex0_ratio_UL,
	//	pCurrentObject->textureRatio[0],		// 1.0
	//	pCurrentObject->textureRatio[1],
	//	pCurrentObject->textureRatio[2],
	//	pCurrentObject->textureRatio[3]);

	GLint tex0_tile_UL = glGetUniformLocation(shaderProgID, "tex_0_3_tile");
	glUniform4f(tex0_tile_UL,
		pCurrentObject->textureTile[0],		// 1.0
		pCurrentObject->textureTile[1],
		pCurrentObject->textureTile[2],
		pCurrentObject->textureTile[3]);

	return;
}

void DrawObject(glm::mat4 worldMatrix, cGameObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager)
{
	if (pCurrentObject->isVisible == false)
	{
		return;
	}

	// Turns on "alpha transparency"
	//glEnable(GL_BLEND);

	// Reads what's on the buffer already, and 
	// blends it with the incoming colour based on the "alpha" 
	// value, which is the 4th colour output
	// RGB+A
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);
	// ************************************************************************
	// Set the texture bindings and samplers

	// See if this is a skybox object? 
	GLint bIsSkyBox_UL = glGetUniformLocation(shaderProgID, "bIsSkyBox");
	if (pCurrentObject->textureBlendMode != CUBE_MAP)
	{
		// Is a regular 2D textured object
		SetUpTextureBindingsForObject(pCurrentObject, shaderProgID);
		glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);

		// Don't draw back facing triangles (default)
		//glDisable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
	}
	else
	{
		// Draw the back facing triangles. 
		// Because we are inside the object, so it will force a draw on the "back" of the sphere 
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		//glDisable(GL_CULL_FACE);	// Draw everything

		glUniform1f(bIsSkyBox_UL, (float)GL_TRUE);

		GLuint skyBoxTextureID = g_pTextureManager->getTextureIDFromName("skyBoxTextures");
		glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0

		// Tie the texture units to the samplers in the shader
		GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgID, "skyBox");
		glUniform1i(skyBoxSampler_UL, 26);	// Texture unit 26
	}
	// ***************************************************************************************

	if (pCurrentObject->physicsShapeType == SPHERE
		|| pCurrentObject->physicsShapeType == BOX
		|| pCurrentObject->physicsShapeType == CYLINDER
		|| pCurrentObject->physicsShapeType == SPINNER
		|| pCurrentObject->physicsShapeType == FLIPPER
		|| pCurrentObject->physicsShapeType == PLUNGER)
	{
		pCurrentObject->physicsComponent->GetTransform(worldMatrix);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f),
			glm::vec3(pCurrentObject->scaleXYZ.x, pCurrentObject->scaleXYZ.y, pCurrentObject->scaleXYZ.z));
		worldMatrix = worldMatrix * scale;
	}
	else
	{
		worldMatrix = calculateWorldMatrix(pCurrentObject);
	}	

	GLint matModel_UL = glGetUniformLocation(shaderProgID, "matModel");

	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(worldMatrix));

	// OpenGL use transpose inverse matrix to get normal
	GLint matModelIT_UL = glGetUniformLocation(shaderProgID, "matModelInverseTranspose");
	glm::mat4 matModelInverseTranspose = glm::inverse(glm::transpose(worldMatrix));
	glUniformMatrix4fv(matModelIT_UL, 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));	

	GLint diffuseColour_UL = glGetUniformLocation(shaderProgID, "diffuseColour");
	glUniform4f(diffuseColour_UL,
		pCurrentObject->diffuseColour.r,
		pCurrentObject->diffuseColour.g,
		pCurrentObject->diffuseColour.b,
		pCurrentObject->diffuseColour.a);		// Transparency

	GLint specularColour_UL = glGetUniformLocation(shaderProgID, "specularColour");
	glUniform4f(specularColour_UL, 0.1f, 0.1f, 0.1f, 32.0f);
	
	GLint debugColour_UL = glGetUniformLocation(shaderProgID, "wireframeColour");
	GLint bWireframeMode_UL = glGetUniformLocation(shaderProgID, "bWireframeMode");
	GLint bNormalPassMode_UL = glGetUniformLocation(shaderProgID, "bNormalPassMode");
	GLint bAlbedoPassMode_UL = glGetUniformLocation(shaderProgID, "bAlbedoPassMode");
	GLint bUseTransparentMap_UL = glGetUniformLocation(shaderProgID, "bUseTransparentMap");
	GLint bNoTexture_UL = glGetUniformLocation(shaderProgID, "bNoTexture");
	GLint bIsImposter_UL = glGetUniformLocation(shaderProgID, "bIsImposter");
	GLint bBaseTexture_UL = glGetUniformLocation(shaderProgID, "bBaseTexture");
	GLint bDiffuseColourBlend_UL = glGetUniformLocation(shaderProgID, "bDiffuseColourBlend");
	GLint bColourBlend_UL = glGetUniformLocation(shaderProgID, "bColourBlend");
	GLint bMultiplyBlend_UL = glGetUniformLocation(shaderProgID, "bMultiplyBlend");
	GLint bAverageBlend_UL = glGetUniformLocation(shaderProgID, "bAverageBlend");
	GLint bSoftlightBlend_UL = glGetUniformLocation(shaderProgID, "bSoftlightBlend");
	GLint bNoLighting_UL = glGetUniformLocation(shaderProgID, "bNoLighting");

	// Restore switch texture uniform to false
	glUniform1f(bNoTexture_UL, (float)GL_FALSE);
	glUniform1f(bIsImposter_UL, (float)GL_FALSE);
	glUniform1f(bBaseTexture_UL, (float)GL_FALSE);
	glUniform1f(bDiffuseColourBlend_UL, (float)GL_FALSE);
	glUniform1f(bColourBlend_UL, (float)GL_FALSE);
	glUniform1f(bMultiplyBlend_UL, (float)GL_FALSE);
	glUniform1f(bAverageBlend_UL, (float)GL_FALSE);
	glUniform1f(bSoftlightBlend_UL, (float)GL_FALSE);
	
	if (wireframeMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform4f(debugColour_UL,
			pCurrentObject->wireframeColour.r,
			pCurrentObject->wireframeColour.g,
			pCurrentObject->wireframeColour.b,
			pCurrentObject->wireframeColour.a);
		glUniform1f(bWireframeMode_UL, (float)GL_TRUE);
	}
	else if (pCurrentObject->isWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform4f(debugColour_UL,
			pCurrentObject->wireframeColour.r,
			pCurrentObject->wireframeColour.g,
			pCurrentObject->wireframeColour.b,
			pCurrentObject->wireframeColour.a);
		glUniform1f(bWireframeMode_UL, (float)GL_TRUE);
	}
	else
	{
		glUniform1f(bWireframeMode_UL, (float)GL_FALSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (normalPassMode)	{	glUniform1f(bNormalPassMode_UL, (float)GL_TRUE);	}
	else	{	glUniform1f(bNormalPassMode_UL, (float)GL_FALSE);	}

	if (albedoPassMode) {	glUniform1f(bAlbedoPassMode_UL, (float)GL_TRUE);	}
	else	{	glUniform1f(bAlbedoPassMode_UL, (float)GL_FALSE);	}	

	if (pCurrentObject->useTransparentMap)
	{
		glUniform1f(bUseTransparentMap_UL, (float)GL_TRUE);
	}
	else
	{
		glUniform1f(bUseTransparentMap_UL, (float)GL_FALSE);
	}	

	switch (pCurrentObject->textureBlendMode)
	{
	case NO_TEXTURE:		glUniform1f(bNoTexture_UL, (float)GL_TRUE);				break;
	case BASE:				glUniform1f(bBaseTexture_UL, (float)GL_TRUE);			break;
	case IMPOSTER:			glUniform1f(bIsImposter_UL, (float)GL_TRUE);			break;
	case DIFFUSE_COLOUR:	glUniform1f(bDiffuseColourBlend_UL, (float)GL_TRUE);	break;
	case COLOUR:			glUniform1f(bColourBlend_UL, (float)GL_TRUE);			break;
	case MULTIPLY:			glUniform1f(bMultiplyBlend_UL, (float)GL_TRUE);			break;
	case AVERAGE:			glUniform1f(bAverageBlend_UL, (float)GL_TRUE);			break;
	case SOFT_LIGHT:		glUniform1f(bSoftlightBlend_UL, (float)GL_TRUE);		break;
	default:																		break;
	}

	if (pCurrentObject->noLighting)
	{
		glUniform1f(bNoLighting_UL, (float)GL_TRUE);
	}
	else
	{
		glUniform1f(bNoLighting_UL, (float)GL_FALSE);
	}


	if (pCurrentObject->disableDepthBufferTest)
	{
		glDisable(GL_DEPTH_TEST);					// DEPTH Test OFF
	}
	else
	{
		glEnable(GL_DEPTH_TEST);						// Turn ON depth test
	}

	if (pCurrentObject->disableDepthBufferWrite)
	{
		glDisable(GL_DEPTH);						// DON'T Write to depth buffer
	}
	else
	{
		glEnable(GL_DEPTH);								// Write to depth buffer
	}

	sModelDrawInfo drawInfo;
	if (pVAOManager->FindDrawInfoByModelName(pCurrentObject->meshName, drawInfo))
	{
		glBindVertexArray(drawInfo.VAO_ID);
		glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT,	0);
		glBindVertexArray(0);
	}

	return;
}

