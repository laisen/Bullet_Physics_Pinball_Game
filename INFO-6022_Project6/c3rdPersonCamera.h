#pragma once
#include <glm/glm.hpp>


struct s3rdPersonCameraDef
{
	s3rdPersonCameraDef();

	glm::vec3 Offset;		// Initial offset of eye from target, normalized during set using Distance for distance
	float ZoomSpeed;
	float Distance;		// Initial distance of eye from target
	float MaxDistance;		// Zoom boundary
	float MinDistance;		// Zoom boundary
	float Yaw;		// Initial left/right pan position in radians
	float MinYaw;
	float MaxYaw;
	float YawSpeed;
	float Pitch;
	float MinPitch;
	float MaxPitch;
	float PitchSpeed;
	bool ElasticYaw;
	bool ClampYaw;
	bool ClampPitch;
	bool InvertPitch;

};

class c3rdPersonCamera
{
public:
	c3rdPersonCamera();
	~c3rdPersonCamera();

	void OnWindowResize(int newWidth, int newHeight);		// call once after starting window
	void SetUp(const s3rdPersonCameraDef& def);		// // call once to initialize

	glm::vec3 GetForwardAxis();
	glm::vec3 GetRightAxis();
	glm::vec3 GetUpAxis();

	void GetProjectionMatrix(glm::mat4& projOut);
	void GetViewMatrix(glm::mat4& viewOut);
	void GetEyePosition(glm::vec3& eyeOut);
	//void GetLineSegmentFromMousePos(double mouseX, double mouseY, glm::vec3& nearOutput, glm::vec3& farOutput);

	void Update(float dt);		// Call this in main loop
	void SetTargetTransform(glm::mat4& targetTransform);		// Call this when look at target moves, with rotating/zooming
	void SetTargetTransform(glm::vec3& targetTransform);		// Call this when look at target moves, with rotating/zooming
	//void InitTargetPosition(glm::mat4& targetTransform);

private:
	void UpdateView();		// Call this at the end of Update

	int mWidth;
	int mHeight;

	glm::mat4 mTargetTransform;		// The transform of the object being looked at
	glm::vec3 mCurrentTargetPosition;

	glm::vec3 mForwardAxis;
	glm::vec3 mRightAxis;
	glm::vec3 mUpAxis;

	glm::vec3 mOffset;
	glm::vec3 mEyePosition;
	double mPrevMouseX;
	double mPrevMouseY;
	bool mMouseDown;
	float mCurrentZoomSpeed;
	float mZoomSpeed;
	float mDistance;
	float mMaxDistance;
	float mMinDistance;
	float mYaw;
	float mMinYaw;
	float mMaxYaw;
	float mYawSpeed;
	float mPitch;
	float mMinPitch;
	float mMaxPitch;
	float mPitchSpeed;
	bool mElasticYaw;
	bool mClampYaw;
	bool mClampPitch;
	bool mInvertPitch;
	glm::mat4 mViewMatrix;
};

