#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H
#include<string>
#include<map>
#include<vector>

#include <utils/Entity.h>

// Name for default materials (2nd is used if meshes have UV coords)
#define AI_DEFAULT_MATERIAL_NAME          "DefaultMaterial"

namespace filament {
	class Engine;
	class Scene;
	class VertexBuffer;
	class IndexBuffer;
	class Material;
	class MaterialInstance;
	class Texture;
};

using namespace filament;
using namespace utils;


class RenderableObject
{
public:

	static RenderableObject& get(Engine* engine, Scene* scene);

	RenderableObject() = delete;
	RenderableObject(const RenderableObject& rhs) = delete;
	RenderableObject(RenderableObject&& rhs) = delete;
	RenderableObject& operator=(const RenderableObject& rhs) = delete;
	RenderableObject& operator=(RenderableObject&& rhs) = delete;


	bool genMaterial(std::string mtFile);
	bool genRenderable(std::string objName, int numVert, float *mVert, float *mTBNs, float *mUVs, int numFaces, uint16_t* mFaces);
	bool genLight(std::string lightName);

	void cleanUp();

protected:
	RenderableObject(Engine *engine, Scene* scene);

private:
	void loadTexture(const std::string& filePath, Texture** map, bool sRGB = true);
	Texture* createOneByOneTexture(uint32_t pixel);

	struct Object {
		std::string objName;
		Entity renderable;
		VertexBuffer* mVertexBuffer = nullptr;
		IndexBuffer* mIndexBuffer = nullptr;
		MaterialInstance* mtlInstance = nullptr;
	};

	struct Mtl {
		bool clearCoat = false;
		bool anisotropy = false;
		Material* g_material = nullptr;
		Texture* g_metallicMap = nullptr;
		Texture* g_roughnessMap = nullptr;
		Texture* g_aoMap = nullptr;
		Texture* g_normalMap = nullptr;
		Texture* g_baseColorMap = nullptr;
	};

private:
	Engine *mEngine = nullptr;
	Scene *mScene = nullptr;

	filament::Material* mDefaultColorMaterial = nullptr;
	filament::Material* mDefaultTransparentColorMaterial = nullptr;
	filament::Texture* mDefaultMap = nullptr;
	filament::Texture* mDefaultNormalMap = nullptr;

	std::map<std::string, Mtl> mMtls;

	std::map<std::string, Entity> mLights;
	std::map<std::string, Entity> mRenderables;
	std::vector<Object> mObjects;
};
#endif // !RENDERABLE_OBJECT_H
