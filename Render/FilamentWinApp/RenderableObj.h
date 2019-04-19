#ifndef RENDERABLE_OBJ_H
#define RENDERABLE_OBJ_H

#include <string>
#include <map>
#include <vector>

#include <filament/Engine.h>
#include <filament/LightManager.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>
#include <filament/Scene.h>
#include <filament/Texture.h>

#include <filamat/MaterialBuilder.h>

#include <utils/Path.h>
#include <utils/EntityManager.h>

#include <math/mat3.h>
#include <math/mat4.h>
#include <math/vec4.h>
#include <math/norm.h>

// Name for default materials (2nd is used if meshes have UV coords)
#define AI_DEFAULT_MATERIAL_NAME          "DefaultMaterial"

using namespace filament::math;
using namespace filament;
using namespace filamat;
using namespace utils;

class RenderableObj
{
	struct PbrConfig;
public:
	RenderableObj(Engine &engine);
	~RenderableObj();

	void cleanup();
	void setup(View* view, Scene* scene, PbrConfig confg);


protected:
	void loadTexture(const std::string& filePath, Texture** map, bool sRGB = true);
	Texture* createOneByOneTexture(uint32_t pixel);

private:
	std::map<std::string, MaterialInstance*> g_materialInstances;
	const Material* g_material;
	Texture* g_metallicMap = nullptr;
	Texture* g_roughnessMap = nullptr;
	Texture* g_aoMap = nullptr;
	Texture* g_normalMap = nullptr;
	Texture* g_baseColorMap = nullptr;

	filament::Material* mDefaultColorMaterial = nullptr;
	filament::Material* mDefaultTransparentColorMaterial = nullptr;
	filament::Texture* mDefaultMap = nullptr;
	filament::Texture* mDefaultNormalMap = nullptr;
	float mDefaultMetallic = 0.0;
	float mDefaultRoughness = 0.4;

	struct PbrConfig {
		std::string materialDir;
		bool clearCoat = false;
		bool anisotropy = false;

		int numVertices = 0;
		float* mVertices = nullptr;
		float* mTBNs = nullptr;
		float* mUV0s = nullptr;
		float* mUV1s = nullptr;
		int numFaces = 0;
		uint16_t* mFaces = nullptr;
		filament::Box aabb;
	} g_pbrConfig;

	Engine &mEngine;
	Entity renderable;
	static Entity rootEntity;
	VertexBuffer* vb = nullptr;
	IndexBuffer* ib = nullptr;
};

#endif // !RENDERABLE_OBJ_H