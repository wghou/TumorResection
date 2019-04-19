#include "RenderableObject.h"

#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/Texture.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filamat/MaterialBuilder.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>
#include <filament/LightManager.h>
#include <utils/EntityManager.h>

#include <utils/Path.h>
#include <stb/stb_image.h>
#include "resources/resources.h"

using namespace filament::math;
using namespace filament;
using namespace filamat;
using namespace utils;

static Entity *rootEntity = new Entity();
static Entity *g_light = new Entity();

RenderableObject::RenderableObject(Engine &engine) : mEngine(engine)
{
	mDefaultMap = createOneByOneTexture(0xffffffff);
	mDefaultNormalMap = createOneByOneTexture(0xffff8080);

	mDefaultColorMaterial = Material::Builder()
		.package(RESOURCES_AIDEFAULTMAT_DATA, RESOURCES_AIDEFAULTMAT_SIZE)
		.build(mEngine);

	mDefaultColorMaterial->setDefaultParameter("baseColor", RgbType::LINEAR, float3{ 0.8 });
	mDefaultColorMaterial->setDefaultParameter("metallic", 0.0f);
	mDefaultColorMaterial->setDefaultParameter("roughness", 0.4f);
	mDefaultColorMaterial->setDefaultParameter("reflectance", 0.5f);

	mDefaultTransparentColorMaterial = Material::Builder()
		.package(RESOURCES_AIDEFAULTTRANS_DATA, RESOURCES_AIDEFAULTTRANS_SIZE)
		.build(mEngine);

	mDefaultTransparentColorMaterial->setDefaultParameter("baseColor", RgbType::LINEAR, float3{ 0.8 });
	mDefaultTransparentColorMaterial->setDefaultParameter("metallic", 0.0f);
	mDefaultTransparentColorMaterial->setDefaultParameter("roughness", 0.4f);
}


RenderableObject::~RenderableObject()
{
	mEngine.destroy(mVertexBuffer);
	mEngine.destroy(mIndexBuffer);
	mEngine.destroy(mDefaultColorMaterial);
	mEngine.destroy(mDefaultTransparentColorMaterial);
	mEngine.destroy(mDefaultNormalMap);
	mEngine.destroy(mDefaultMap);

	mEngine.destroy(*renderable);

	/*for (Texture* texture : mTextures) {
		mEngine.destroy(texture);
	}*/

	// destroy the Entities itself
	EntityManager::get().destroy(*renderable);

}

Texture* RenderableObject::createOneByOneTexture(uint32_t pixel) {
	uint32_t *textureData = (uint32_t *)malloc(sizeof(uint32_t));
	*textureData = pixel;

	Texture *texturePtr = Texture::Builder()
		.width(uint32_t(1))
		.height(uint32_t(1))
		.levels(0xff)
		.format(driver::TextureFormat::RGBA8)
		.build(mEngine);

	Texture::PixelBufferDescriptor defaultNormalBuffer(textureData,
		size_t(1 * 1 * 4),
		Texture::Format::RGBA,
		Texture::Type::UBYTE,
		(driver::BufferDescriptor::Callback) &free);

	texturePtr->setImage(mEngine, 0, std::move(defaultNormalBuffer));
	texturePtr->generateMipmaps(mEngine);

	return texturePtr;
}

void RenderableObject::loadTexture(const std::string& filePath, Texture** map, bool sRGB)
{
	if (!filePath.empty()) {
		Path path(filePath);
		if (path.exists()) {
			int w, h, n;
			unsigned char* data = stbi_load(path.getAbsolutePath().c_str(), &w, &h, &n, 3);
			if (data != nullptr) {
				*map = Texture::Builder()
					.width(uint32_t(w))
					.height(uint32_t(h))
					.levels(0xff)
					.format(sRGB ? driver::TextureFormat::SRGB8 : driver::TextureFormat::RGB8)
					.build(mEngine);
				Texture::PixelBufferDescriptor buffer(data, size_t(w * h * 3),
					Texture::Format::RGB, Texture::Type::UBYTE,
					(driver::BufferDescriptor::Callback) &stbi_image_free);
				(*map)->setImage(mEngine, 0, std::move(buffer));
				(*map)->generateMipmaps(mEngine);
			}
			else {
				std::cout << "The texture " << path << " could not be loaded" << std::endl;
			}
		}
		else {
			std::cout << "The texture " << path << " does not exist" << std::endl;
		}
	}
}

void RenderableObject::genMaterial(std::string mtFile)
{
	Path path(mtFile);
	std::string name(path.getName());

	loadTexture(path.concat(name + "_Color.png"), &g_baseColorMap);
	loadTexture(path.concat(name + "_Metallic.png"), &g_metallicMap, false);
	loadTexture(path.concat(name + "_Roughness.png"), &g_roughnessMap, false);
	loadTexture(path.concat(name + "_AO.png"), &g_aoMap, false);
	loadTexture(path.concat(name + "_Normal.png"), &g_normalMap, false);

	bool hasBaseColorMap = g_baseColorMap != nullptr;
	bool hasMetallicMap = g_metallicMap != nullptr;
	bool hasRoughnessMap = g_roughnessMap != nullptr;
	bool hasAOMap = g_aoMap != nullptr;
	bool hasNormalMap = g_normalMap != nullptr;

	std::string shader = R"SHADER(
        void material(inout MaterialInputs material) {
    )SHADER";

	if (hasNormalMap) {
		shader += R"SHADER(
            material.normal = texture(materialParams_normalMap, getUV0()).xyz * 2.0 - 1.0;
            material.normal.y *= -1.0;
        )SHADER";
	}

	shader += R"SHADER(
        prepareMaterial(material);
    )SHADER";

	if (hasBaseColorMap) {
		shader += R"SHADER(
            material.baseColor.rgb = texture(materialParams_baseColorMap, getUV0()).rgb;
        )SHADER";
	}
	else {
		shader += R"SHADER(
            material.baseColor.rgb = float3(1.0, 0.75, 0.94);
        )SHADER";
	}
	if (hasMetallicMap) {
		shader += R"SHADER(
            material.metallic = texture(materialParams_metallicMap, getUV0()).r;
        )SHADER";
	}
	else {
		shader += R"SHADER(
            material.metallic = 0.0;
        )SHADER";
	}
	if (hasRoughnessMap) {
		shader += R"SHADER(
            material.roughness = texture(materialParams_roughnessMap, getUV0()).r;
        )SHADER";
	}
	else {
		shader += R"SHADER(
            material.roughness = 1.0;
        )SHADER";
	}
	if (hasAOMap) {
		shader += R"SHADER(
            material.ambientOcclusion = texture(materialParams_aoMap, getUV0()).r;
        )SHADER";
	}
	else {
		shader += R"SHADER(
            material.ambientOcclusion = 1.0;
        )SHADER";
	}

	if (clearCoat) {
		shader += R"SHADER(
            material.clearCoat = 1.0;
        )SHADER";
	}
	if (anisotropy) {
		shader += R"SHADER(
            material.anisotropy = 0.7;
        )SHADER";
	}
	shader += "}\n";


	MaterialBuilder::init();
	MaterialBuilder builder = MaterialBuilder()
		.name("DefaultMaterial")
		.material(shader.c_str())
		.shading(Shading::LIT);

	if (hasBaseColorMap) {
		builder
			.require(VertexAttribute::UV0)
			.parameter(MaterialBuilder::SamplerType::SAMPLER_2D, "baseColorMap");
	}
	if (hasMetallicMap) {
		builder
			.require(VertexAttribute::UV0)
			.parameter(MaterialBuilder::SamplerType::SAMPLER_2D, "metallicMap");
	}
	if (hasRoughnessMap) {
		builder
			.require(VertexAttribute::UV0)
			.parameter(MaterialBuilder::SamplerType::SAMPLER_2D, "roughnessMap");
	}
	if (hasAOMap) {
		builder
			.require(VertexAttribute::UV0)
			.parameter(MaterialBuilder::SamplerType::SAMPLER_2D, "aoMap");
	}
	if (hasNormalMap) {
		builder
			.require(VertexAttribute::UV0)
			.parameter(MaterialBuilder::SamplerType::SAMPLER_2D, "normalMap");
	}

	Package pkg = builder.build();

	g_material = Material::Builder().package(pkg.getData(), pkg.getSize()).build(mEngine);
	g_materialInstances["DefaultMaterial"] = g_material->createInstance();

	TextureSampler sampler(TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR,
		TextureSampler::MagFilter::LINEAR, TextureSampler::WrapMode::REPEAT);
	sampler.setAnisotropy(8.0f);

	if (hasBaseColorMap) {
		g_materialInstances["DefaultMaterial"]->setParameter(
			"baseColorMap", g_baseColorMap, sampler);
	}
	if (hasMetallicMap) {
		g_materialInstances["DefaultMaterial"]->setParameter(
			"metallicMap", g_metallicMap, sampler);
	}
	if (hasRoughnessMap) {
		g_materialInstances["DefaultMaterial"]->setParameter(
			"roughnessMap", g_roughnessMap, sampler);
	}
	if (hasAOMap) {
		g_materialInstances["DefaultMaterial"]->setParameter(
			"aoMap", g_aoMap, sampler);
	}
	if (hasNormalMap) {
		g_materialInstances["DefaultMaterial"]->setParameter(
			"normalMap", g_normalMap, sampler);
	}
}


void RenderableObject::genRenderable(Scene* scene, int numVert, float *mVert, float *mTBNs, float *mUVs, int numFaces, uint16_t* mFaces)
{
	mVertexBuffer = VertexBuffer::Builder()
		.vertexCount(numVert)
		.bufferCount(3)
		.attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
		.attribute(VertexAttribute::TANGENTS, 1, VertexBuffer::AttributeType::FLOAT4)
		.normalized(VertexAttribute::TANGENTS)
		.attribute(VertexAttribute::UV0, 2, VertexBuffer::AttributeType::FLOAT2)
		.normalized(VertexAttribute::UV0)
		.build(mEngine);

	mVertexBuffer->setBufferAt(mEngine, 0,
		VertexBuffer::BufferDescriptor(mVert, numVert * 3 * sizeof(float), nullptr));
	mVertexBuffer->setBufferAt(mEngine, 1,
		VertexBuffer::BufferDescriptor(mTBNs, numVert * 4 * sizeof(float), nullptr));
	mVertexBuffer->setBufferAt(mEngine, 2,
		VertexBuffer::BufferDescriptor(mUVs, numVert * 2 * sizeof(float), nullptr));

	mIndexBuffer = IndexBuffer::Builder()
		.indexCount(numFaces * 3)
		.bufferType(IndexBuffer::IndexType::USHORT)
		.build(mEngine);

	mIndexBuffer->setBuffer(mEngine,
		IndexBuffer::BufferDescriptor(mFaces, numFaces * 3 * sizeof(uint16_t), nullptr));


	// always add the DefaultMaterial (with its default parameters), so we don't pick-up
	// whatever defaults is used in mesh
	if (g_materialInstances.find(AI_DEFAULT_MATERIAL_NAME) == g_materialInstances.end()) {
		g_materialInstances[AI_DEFAULT_MATERIAL_NAME] = mDefaultColorMaterial->createInstance();
	}

	// create renderable entity
	renderable = new Entity();
	EntityManager::get().create(1, renderable);

	if (rootEntity->isNull()) {
		EntityManager::get().create(1, rootEntity);

		TransformManager& tcm = mEngine.getTransformManager();
		//Add root instance
		tcm.create(*rootEntity, TransformManager::Instance{}, mat4f());
	}

	RenderableManager::Builder(1)
		.boundingBox({ {-1.f, -1.f, -1.f}, {1.f, 1.f, 1.f} })
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, mVertexBuffer, mIndexBuffer, 0, numFaces * 3)
		.material(0, g_materialInstances[AI_DEFAULT_MATERIAL_NAME])
		.build(mEngine, *renderable);


	// 其实还不太懂，这里设置 parent 是什么意思
	TransformManager& tcm2 = mEngine.getTransformManager();
	TransformManager::Instance parent(tcm2.getInstance(*rootEntity));
	tcm2.create(*renderable, parent, mat4f());
	scene->addEntity(*renderable);
}

void RenderableObject::genLight(Scene *scene)
{
	if (!g_light->isNull()) return;

	*g_light = EntityManager::get().create();
	LightManager::Builder(LightManager::Type::DIRECTIONAL)
		.color(Color::toLinear<ACCURATE>({ 0.98f, 0.92f, 0.89f }))
		.intensity(110000)
		.direction({ 0.6, -1, -0.8 })
		.build(mEngine, *g_light);
	scene->addEntity(*g_light);
}