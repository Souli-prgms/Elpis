#pragma once

#include "Core.h"

Mat4 orthographicProjection(float l, float r, float b, float t, float n, float f);
Mat4 perspectiveProjection(float l, float r, float b, float t, float n, float f);

class Camera
{
public:
	enum State {
		Idle, Rotating, Translating
	};

	Camera();
	~Camera() {};

	const Vec3& sceneCenter() const;
	const Quat& sceneOrientation() const;
	const Box2& screenViewport() const;
	Mat4 computeViewMatrix() const;
	Mat4 computeProjectionMatrix() const;
	Mat4 computeOrthoType() const;

	void setSceneCenter(const Vec3& scnCenter);
	void setSceneDistance(float scnDistance);
	void setSceneRadius(float scnRadius);
	void setSceneOrientation(const Quat& scnOrientation);
	void setScreenViewport(const Box2& scnViewport);
	void setMinNear(float minNear);
	void setNearFarOffsets(float nearOffset, float farOffset);

	State state() const;
	bool isIdle() const;

	void rotate(const Quat& rot);
	bool isRotating() const;
	void startRotation(const Vec2& scrPos);
	void dragRotate(const Vec2& scrPos);
	void cancelRotation();
	void endRotation();

	void translate(const Vec2& scnVec);
	void translate(const Vec3& scnVec);
	bool isTranslating() const;
	void startTranslation(const Vec2& scrPos);
	void dragTranslate(const Vec2& scrPos);
	void cancelTranslation();
	void endTranslation();

	void zoom(float factor);
	void grow(float factor);
	void dollyZoom(float factor);

	float sceneDistance() const;
	float sceneRadius() const;

	float minNear() const;

	float nearOffset() const;
	float farOffset() const;

	float minScreenViewportSize() const;
	bool isOrthographic() const;
	bool isPerspective() const;

	Vec3 getPosition() const;

	static Mat4 perspective(float fovy, float aspect, float zNear, float zFar);
	static Mat4 lookAt(const Vec3& position, const Vec3& target, const Vec3& up);

private:
	Vec2 normFromScr(const Vec2& scrPos) const;

	Quat computeRotation(const Vec2& scrPos) const;
	Vec3 computeTranslation(const Vec2& scrPos) const;

	Vec3 m_scnCenter;
	float m_scnDistance;
	float m_scnRadius;
	Quat m_scnOrientation;
	Box2 m_scrViewport;
	float m_minNear;
	float m_nearOffset;
	float m_farOffset;

	State m_state;
	Vec2 m_scrMouseInit;
	Quat m_scnOrientInit;
	Vec3 m_scnCenterInit;
};

