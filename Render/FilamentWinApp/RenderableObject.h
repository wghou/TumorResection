#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H
#include<string>
#include<map>

// Name for default materials (2nd is used if meshes have UV coords)
#define AI_DEFAULT_MATERIAL_NAME          "DefaultMaterial"

namespace utils {
	class Entity;
}

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

//static Entity *rootEntity;
//static Entity *g_light;

class RenderableObject
{
public:
	RenderableObject(Engine &engine);
	~RenderableObject();

	void genMaterial(std::string mtFile);
	void genRenderable(Scene* scene, int numVert, float *mVert, float *mTBNs, float *mUVs, int numFaces, uint16_t* mFaces);
	void genLight(Scene *scene);

private:
	void loadTexture(const std::string& filePath, Texture** map, bool sRGB = true);
	Texture* createOneByOneTexture(uint32_t pixel);

public:
	std::string objDir;
	bool clearCoat = false;
	bool anisotropy = false;
	Entity *renderable;
	VertexBuffer* mVertexBuffer = nullptr;
	IndexBuffer* mIndexBuffer = nullptr;
	Material* g_material = nullptr;
	std::map<std::string, MaterialInstance*> g_materialInstances;
	Texture* g_metallicMap = nullptr;
	Texture* g_roughnessMap = nullptr;
	Texture* g_aoMap = nullptr;
	Texture* g_normalMap = nullptr;
	Texture* g_baseColorMap = nullptr;

private:
	Engine& mEngine;

	filament::Material* mDefaultColorMaterial = nullptr;
	filament::Material* mDefaultTransparentColorMaterial = nullptr;

	filament::Texture* mDefaultMap = nullptr;
	filament::Texture* mDefaultNormalMap = nullptr;
};
#endif // !RENDERABLE_OBJECT_H



