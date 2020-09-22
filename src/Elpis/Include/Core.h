#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <eigen3/Eigen/Eigen>

// Pointers
template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> createScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> createRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}


// Vectors
typedef Eigen::Vector2f Vec2;
typedef Eigen::Vector3f Vec3;
typedef Eigen::Vector4f Vec4;


// Matrices
typedef Eigen::Matrix3f Mat3;
typedef Eigen::Matrix4f Mat4;


// Quaternion
typedef Eigen::Quaternionf Quat;


// Bounding boxes
typedef Eigen::AlignedBox2f Box2;
typedef Eigen::AlignedBox3f Box3;