#include "c3rdPersonCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "cGameObject.h"

extern bool g_MouseLeftButtonIsDown;
extern double g_mouseX, g_mouseY, g_scrollY;
extern float g_FOV;
extern glm::vec3 cameraVelocity;
extern double averageDeltaTime;
extern std::vector<cGameObject*> controlTargets;
extern unsigned int controlIndex;
glm::vec3 SmoothFollow(glm::vec3 current, glm::vec3 target, glm::vec3& currentVelocity, float smoothTime, float deltaTime);
void CamShake(glm::vec3& eyeOut);

s3rdPersonCameraDef::s3rdPersonCameraDef()
{
	ElasticYaw = false;
	ClampYaw = false;
	ClampPitch = true;
	InvertPitch = false;
	ZoomSpeed = 50.0f;
	MinDistance = 2.0f;
	Distance = 55.0f;
	MaxDistance = 100.0f;
	Yaw = 0.0f;
	MinYaw = -0.7f;
	MaxYaw = 0.7f;
	YawSpeed = 350.0f;
	Pitch = -1.0f;
	MinPitch = -1.5f;
	MaxPitch = 0.3f;
	PitchSpeed = 350.0f;
	//Offset = glm::normalize(glm::vec3(0.0f, 2.5f, 25.0f));
	Offset = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
}

c3rdPersonCamera::c3rdPersonCamera() : mMouseDown(false), mCurrentZoomSpeed(0.0f)
{
	SetUp(s3rdPersonCameraDef());
}

c3rdPersonCamera::~c3rdPersonCamera()
{
}

void c3rdPersonCamera::OnWindowResize(int newWidth, int newHeight)
{
	mWidth = newWidth;
	mHeight = newHeight;
}

void c3rdPersonCamera::SetUp(const s3rdPersonCameraDef& def)
{
	mElasticYaw = def.ElasticYaw;
	mClampYaw = def.ClampYaw;
	mClampPitch = def.ClampPitch;
	mInvertPitch = def.InvertPitch;
	mZoomSpeed = def.ZoomSpeed;
	mMinDistance = def.MinDistance;
	mDistance = def.Distance;
	mMaxDistance = def.MaxDistance;
	mYaw = def.Yaw;
	mMinYaw = def.MinYaw;
	mMaxYaw = def.MaxYaw;
	mYawSpeed = def.YawSpeed;
	mPitch = def.Pitch;
	mMinPitch = def.MinPitch;
	mMaxPitch = def.MaxPitch;
	mPitchSpeed = def.PitchSpeed;
	mOffset = glm::normalize(def.Offset);
	mCurrentTargetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	mTargetTransform = glm::mat4(1.0f);
}

glm::vec3 c3rdPersonCamera::GetForwardAxis()
{
	return mForwardAxis;
}

glm::vec3 c3rdPersonCamera::GetRightAxis()
{
	return mRightAxis;
}

glm::vec3 c3rdPersonCamera::GetUpAxis()
{
	return mUpAxis;
}

void c3rdPersonCamera::GetProjectionMatrix(glm::mat4& projOut)
{
	//projOut = glm::perspective(0.8f, (float)mWidth/(float)mHeight, 0.1f, 10000.0f);
	projOut = glm::perspective(glm::radians(g_FOV), (float)mWidth/(float)mHeight, 0.1f, 2000.0f);
}

void c3rdPersonCamera::GetViewMatrix(glm::mat4& viewOut)
{
	viewOut = mViewMatrix;
}

void c3rdPersonCamera::GetEyePosition(glm::vec3& eyeOut)
{
	eyeOut = mEyePosition;
}

//void c3rdPersonCamera::GetLineSegmentFromMousePos(double mouseX, double mouseY, glm::vec3& nearOutput, glm::vec3& farOutput)
//{
//	glm::vec4 rayStartNDC;
//	// TODO fix
//	rayStartNDC.x = ((float)mouseX / (float)mWidth * 0.5f) * 2.0f;
//	rayStartNDC.y = -((float)mouseY / (float)mHeight * 0.5f) * 2.0f;
//	rayStartNDC.z = -1.0f;
//	rayStartNDC.w = 1.0f;
//
//	glm::vec4 rayEndNDC;
//	rayEndNDC.x = ((float)mouseX / (float)mWidth * 0.5f) * 2.0f;
//	rayEndNDC.y = -((float)mouseY / (float)mHeight * 0.5f) * 2.0f;
//	rayEndNDC.z = 1.0f;
//	rayEndNDC.w = 1.0f;
//
//	glm::mat4 projectionMatrix(1.0f);
//	GetProjectionMatrix(projectionMatrix);
//	glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
//
//	glm::mat4 inverseView(glm::inverse(mViewMatrix));
//
//	glm::vec4 rayStartCam = inverseProjection * rayStartNDC;
//	rayStartCam /= rayStartCam.w;
//
//	glm::vec4 rayStartWorld = inverseView * rayStartCam;
//	rayStartWorld /= rayStartWorld.w;
//
//	glm::vec4 rayEndCam = inverseProjection * rayEndCam;
//	rayEndCam /= rayEndCam.w;
//
//	glm::vec4 rayEndWorld = inverseView * rayEndCam;
//	rayEndWorld /= rayEndWorld.w;
//
//	nearOutput.x = rayStartWorld.x;
//	nearOutput.y = rayStartWorld.y;
//	nearOutput.z = rayStartWorld.z;
//	farOutput.x = rayEndWorld.x;
//	farOutput.y = rayEndWorld.y;
//	farOutput.z = rayEndWorld.z;
//}

void c3rdPersonCamera::Update(float dt)
{
	bool mouseButtonDown = g_MouseLeftButtonIsDown;
	if (mouseButtonDown)
	{
		double mouseX = g_mouseX;
		double mouseY = g_mouseY;
		if (mMouseDown)
		{
			double diffX(mouseX - mPrevMouseX);
			double diffY(mouseY - mPrevMouseY);

			float yawSpeed = mYawSpeed * (float)(diffX / (double)mWidth);
			mYaw += yawSpeed * dt;
			if (mClampYaw)
			{
				mYaw = glm::clamp(mYaw, mMinYaw, mMaxYaw);
			}			

			float pitchSpeed = mPitchSpeed * (float)(diffY / (double)mHeight);
			if (mInvertPitch)
			{
				mPitch += pitchSpeed * dt;
			}
			else
			{
				mPitch -= pitchSpeed * dt;
			}
			if (mClampPitch)
			{
				mPitch = glm::clamp(mPitch, mMinPitch, mMaxPitch);
			}			
		}
		mPrevMouseX = mouseX;
		mPrevMouseY = mouseY;
	}
	mMouseDown = mouseButtonDown;

	double scrollY = g_scrollY;
	bool pressUp = scrollY > 0;
	bool pressDown = scrollY < 0;
	if (pressDown != pressUp)
	{
		mCurrentZoomSpeed = pressDown ? mZoomSpeed : -mZoomSpeed;
	}
	else
	{
		mCurrentZoomSpeed *= glm::pow(0.01f, dt);
		if (glm::abs(mCurrentZoomSpeed) < 0.005f)
		{
			mCurrentZoomSpeed = 0.0f;
		}
	}
	if (mCurrentZoomSpeed != 0.0f)
	{
		mDistance += mCurrentZoomSpeed * dt;
		mDistance = glm::clamp(mDistance, mMinDistance, mMaxDistance);
	}

	UpdateView();
}

void c3rdPersonCamera::SetTargetTransform(glm::mat4& targetTransform)
{
	//mTargetTransform = targetTransform;
	mTargetTransform[3].x = targetTransform[3].x;
	mTargetTransform[3].y = targetTransform[3].y;
	mTargetTransform[3].z = targetTransform[3].z;
}

void c3rdPersonCamera::SetTargetTransform(glm::vec3& targetTransform)
{
	mTargetTransform[3].x = targetTransform.x;
	mTargetTransform[3].y = targetTransform.y;
	mTargetTransform[3].z = targetTransform.z;
}

//void c3rdPersonCamera::InitTargetPosition(glm::mat4& targetTransform)
//{
//	mCurrentTargetPosition = glm::vec3(targetTransform[3].x, targetTransform[3].y, targetTransform[3].z);
//}

void c3rdPersonCamera::UpdateView()
{
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 right(1.0f, 0.0f, 0.0f);	

	mCurrentTargetPosition = SmoothFollow(mCurrentTargetPosition,
		glm::vec3(mTargetTransform[3].x, mTargetTransform[3].y, mTargetTransform[3].z),
		cameraVelocity,
		0.3f,
		(float)averageDeltaTime);

	glm::vec4 offset(mOffset.x, mOffset.y, mOffset.z, 0.0f);

	glm::mat4 transform(glm::rotate(mTargetTransform, mYaw, up));
	transform = glm::rotate(transform, mPitch, right);
	offset = transform * offset;

	offset = glm::normalize(offset) * mDistance;

	//glm::vec3 eye(target.x + offset.x, target.y + offset.y, target.z + offset.z);
	glm::vec3 eye(mCurrentTargetPosition.x + offset.x,
		mCurrentTargetPosition.y + offset.y,
		mCurrentTargetPosition.z + offset.z);

	CamShake(eye);
	mEyePosition = eye;
	mViewMatrix = glm::lookAtRH(eye, mCurrentTargetPosition, up);

	mForwardAxis = glm::normalize(mCurrentTargetPosition - eye);
	mRightAxis = glm::normalize(glm::cross(mForwardAxis, up));
	mUpAxis = glm::normalize(glm::cross(mRightAxis, mForwardAxis));
}
