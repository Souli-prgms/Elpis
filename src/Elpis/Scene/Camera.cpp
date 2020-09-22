#include "Camera.h"

#define ORTHO_THRESOLD 1.e2f

Mat4 orthographicProjection(
	float l, float r, float b, float t, float n, float f) {
	float rpl = r + l;
	float tpb = t + b;
	float fpn = f + n;

	float rml = r - l;
	float tmb = t - b;
	float fmn = f - n;

	Mat4 m;
	m << 2.f / rml, 0.0f, 0.0f, -rpl / rml,
		0.0f, 2.f / tmb, 0.0f, -tpb / tmb,
		0.0f, 0.0f, -2.f / fmn, -fpn / fmn,
		0.0f, 0.0f, 0.0f, 1;
	return m;
}



Mat4 perspectiveProjection(
	float l, float r, float b, float t, float n, float f) {
	float rpl = r + l;
	float tpb = t + b;
	float fpn = f + n;

	float rml = r - l;
	float tmb = t - b;
	float fmn = f - n;

	Mat4 m;
	m << 2 / rml, 0, rpl / rml, 0,
		0, 2 / tmb, tpb / tmb, 0,
		0, 0, -fpn / fmn, -2 * f*n / fmn,
		0, 0, -1, 0;
	return m;
}


Camera::Camera() :
	m_scnCenter(Vec3::Zero()),
	m_scnDistance(3.),
	m_scnRadius(1.),
	m_scnOrientation(1., 0., 0., 0.),
	m_scrViewport(),
	m_minNear(.001),
	m_nearOffset(-1),
	m_farOffset(1),
	m_state(Idle) {
}


const Vec3& Camera::sceneCenter() const {
	return m_scnCenter;
}


float Camera::sceneDistance() const {
	return m_scnDistance;
}


float Camera::sceneRadius() const {
	return m_scnRadius;
}


const Quat& Camera::sceneOrientation() const {
	return m_scnOrientation;
}


const Box2& Camera::screenViewport() const {
	return m_scrViewport;
}


float Camera::minNear() const {
	return m_minNear;
}


float Camera::nearOffset() const {
	return m_nearOffset;
}


float Camera::farOffset() const {
	return m_farOffset;
}


float Camera::minScreenViewportSize() const {
	return m_scrViewport.sizes().minCoeff();
}


bool Camera::isOrthographic() const {
	return (m_scnDistance / m_scnRadius) > ORTHO_THRESOLD;
}


bool Camera::isPerspective() const {
	return !isOrthographic();
}


Mat4 Camera::computeViewMatrix() const {
	return Eigen::Affine3f(m_scnOrientation.inverse() * Eigen::Translation3f(-getPosition())).matrix();
}


Mat4 Camera::computeProjectionMatrix() const {
	float scrMinSize = minScreenViewportSize();
	float r = m_scnRadius * m_scrViewport.sizes().x() / (scrMinSize);
	float t = m_scnRadius * m_scrViewport.sizes().y() / (scrMinSize);

	if (isPerspective()) {
		r /= m_scnDistance;
		t /= m_scnDistance;
		return perspectiveProjection(
			-r, r, -t, t,
			max(m_minNear, m_scnDistance + m_nearOffset),
			m_scnDistance + m_farOffset);
	}
	else {
		return orthographicProjection(-r, r, -t, t, m_nearOffset, m_farOffset);
	}
}


Mat4 Camera::computeOrthoType() const {
	float scrMinSize = minScreenViewportSize();
	float r = m_scnRadius * m_scrViewport.sizes().x() / (scrMinSize);
	float t = m_scnRadius * m_scrViewport.sizes().y() / (scrMinSize);

	return orthographicProjection(-r, r, -t, t, m_nearOffset, m_farOffset);
}

void Camera::setSceneCenter(const Vec3& scnCenter) {
	m_scnCenter = scnCenter;
}


void Camera::setSceneDistance(float scnDistance) {
	m_scnDistance = scnDistance;
}


void Camera::setSceneRadius(float scnRadius) {
	m_scnRadius = scnRadius;
}


void Camera::setSceneOrientation(const Quat& scnOrientation) {
	m_scnOrientation = scnOrientation;
}


void Camera::setScreenViewport(const Box2& scrViewport) {
	m_scrViewport = scrViewport;
}


void Camera::setMinNear(float minNear) {
	m_minNear = minNear;
}


void Camera::setNearFarOffsets(float nearOffset, float farOffset) {
	m_nearOffset = nearOffset;
	m_farOffset = farOffset;
}


bool Camera::isIdle() const {
	return m_state == Idle;
}


void Camera::rotate(const Quat& rot) {
	m_scnOrientation *= rot;
}


bool Camera::isRotating() const {
	return m_state == Rotating;
}


void Camera::startRotation(const Vec2& scrPos) {
	assert(m_state == Idle);
	m_state = Rotating;
	m_scrMouseInit = scrPos;
	m_scnOrientInit = m_scnOrientation;
}


void Camera::dragRotate(const Vec2& scrPos) {
	assert(m_state == Rotating);
	m_scnOrientation = computeRotation(scrPos);
}


void Camera::cancelRotation() {
	assert(m_state == Rotating);
	m_state = Idle;
	m_scnOrientation = m_scnOrientInit;
}


void Camera::endRotation() {
	assert(m_state == Rotating);
	m_state = Idle;
}


bool Camera::isTranslating() const {
	return m_state == Translating;
}


void Camera::startTranslation(const Vec2& scrPos) {
	assert(m_state == Idle);
	m_state = Translating;
	m_scrMouseInit = scrPos;
	m_scnCenterInit = m_scnCenter;
}


void Camera::dragTranslate(const Vec2& scrPos) {
	assert(m_state == Translating);
	m_scnCenter = computeTranslation(scrPos);
}


void Camera::cancelTranslation() {
	assert(m_state == Translating);
	m_state = Idle;
	m_scnCenter = m_scnCenterInit;
}


void Camera::endTranslation() {
	assert(m_state == Translating);
	m_state = Idle;
}


void Camera::zoom(float factor) {
	if (isPerspective())
		m_scnDistance /= factor;
	m_scnRadius /= factor;
}


void Camera::grow(float factor) {
	m_scnRadius *= factor;
}


void Camera::dollyZoom(float factor) {
	if (m_scnDistance / m_scnRadius > ORTHO_THRESOLD)
		m_scnDistance = ORTHO_THRESOLD * m_scnRadius;
	m_scnDistance /= factor;
}


Vec2 Camera::normFromScr(const Vec2& scrPos) const {
	return (scrPos - m_scrViewport.center()) / minScreenViewportSize();
}


Quat Camera::computeRotation(const Vec2& scrPos) const {
	Vec2 v = (m_scrMouseInit - scrPos) * (float(2. * M_PI) / m_scrViewport.sizes().x());
	Vec3 x = Vec3::UnitX();
	Vec3 y = Vec3::UnitY();
	return m_scnOrientInit * Quat(Eigen::AngleAxisf(v.x(), y)) * Quat(Eigen::AngleAxisf(v.y(), x));
}


Vec3 Camera::computeTranslation(const Vec2& scrPos) const {
	Eigen::Matrix<float, 3, 2> m;
	Vec2 normOffset = 2.f * (scrPos - m_scrMouseInit) / minScreenViewportSize();
	m << m_scnOrientation * Vec3::UnitX(), m_scnOrientation * -Vec3::UnitY();
	return m_scnCenterInit - m * normOffset * m_scnRadius;
}

Vec3 Camera::getPosition() const {
	Vec3 scnCamera = m_scnCenter;
	if (isPerspective())
		scnCamera += m_scnOrientation * Vec3::UnitZ() * m_scnDistance;
	return scnCamera;
}

Mat4 Camera::perspective(float fovy, float aspect, float zNear, float zFar)
{
	float tanHalfFovy = tan(fovy / 2.f);

	Mat4 m;
	m << 1.f / (aspect * tanHalfFovy), 0, 0, 0,
		0, 1.f / (tanHalfFovy), 0, 0,
		0, 0, -(zFar + zNear) / (zFar - zNear), -(2.f * zFar * zNear) / (zFar - zNear),
		0, 0, -1, 0;
	return m;
}

Mat4 Camera::lookAt(const Vec3& position, const Vec3& target, const Vec3& up)
{
	Mat3 R;
	R.col(2) = (position - target).normalized();
	R.col(0) = up.cross(R.col(2)).normalized();
	R.col(1) = R.col(2).cross(R.col(0));

	Mat4 m(Mat4::Identity());
	m.topLeftCorner<3, 3>() = R.transpose();
	m.topRightCorner<3, 1>() = -R.transpose() * position;

	return m;
}