#pragma once 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Eigen/Eigen>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb/stb_image.h>

#define BIT(x) (1 << x)
#define EL_OK 1
#define EL_KO 0

#define EL_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define EL_RESOURCE_PATH(path) std::string(RESOURCE_DIR) + "/" + path

namespace Elpis {
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
}

/*extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}*/