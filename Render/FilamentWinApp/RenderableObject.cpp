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

#include <math/mat3.h>
#include <math/mat4.h>
#include <math/vec4.h>
#include <math/norm.h>

#include <utils/Path.h>
#include <stb/stb_image.h>
#include "resources/resources.h"
#include "FilamentApp.h"

using namespace filament::math;
using namespace filament;
using namespace filamat;
using namespace utils;

static Entity rootEntity;

RenderableObject& RenderableObject::get(Engine* engine, Scene* scene) {
	static RenderableObject objs(engine, scene);
	return objs;
}

RenderableObject::RenderableObject(Engine *engine, Scene* scene) : mEngine(engine), mScene(scene)
{
	mDefaultMap = createOneByOneTexture(0xffffffff);
	mDefaultNormalMap = createOneByOneTexture(0xffff8080);

	mDefaultColorMaterial = Material::Builder()
		.package(RESOURCES_AIDEFAULTMAT_DATA, RESOURCES_AIDEFAULTMAT_SIZE)
		.build(*mEngine);

	mDefaultColorMaterial->setDefaultParameter("baseColor", RgbType::LINEAR, float3{ 0.8 });
	mDefaultColorMaterial->setDefaultParameter("metallic", 0.0f);
	mDefaultColorMaterial->setDefaultParameter("roughness", 0.4f);
	mDefaultColorMaterial->setDefaultParameter("reflectance", 0.5f);

	mDefaultTransparentColorMaterial = Material::Builder()
		.package(RESOURCES_AIDEFAULTTRANS_DATA, RESOURCES_AIDEFAULTTRANS_SIZE)
		.build(*mEngine);

	mDefaultTransparentColorMaterial->setDefaultParameter("baseColor", RgbType::LINEAR, float3{ 0.8 });
	mDefaultTransparentColorMaterial->setDefaultParameter("metallic", 0.0f);
	mDefaultTransparentColorMaterial->setDefaultParameter("roughness", 0.4f);
}


void RenderableObject::cleanUp()
{
	for (auto& obj : mRenderables) {
		mEngine->destroy(obj.second.mtlInstance);
		mEngine->destroy(obj.second.mVertexBuffer);
		mEngine->destroy(obj.second.mIndexBuffer);
	}

	for (auto mtl : mMtls) {
		mEngine->destroy(mtl.second.g_material);
		mEngine->destroy(mtl.second.g_metallicMap);
		mEngine->destroy(mtl.second.g_roughnessMap);
		mEngine->destroy(mtl.second.g_aoMap);
		mEngine->destroy(mtl.second.g_normalMap);
		mEngine->destroy(mtl.second.g_baseColorMap);
	}

	EntityManager& em = EntityManager::get();
	for (auto obj : mRenderables) {
		mEngine->destroy(obj.second.renderable);
		em.destroy(obj.second.renderable);
	}

	for (auto lt : mLights) {
		mEngine->destroy(lt.second);
		em.destroy(lt.second);
	}

	mEngine->destroy(mDefaultColorMaterial);
	mEngine->destroy(mDefaultTransparentColorMaterial);
	mEngine->destroy(mDefaultNormalMap);
	mEngine->destroy(mDefaultMap);
}

Texture* RenderableObject::createOneByOneTexture(uint32_t pixel) {
	uint32_t *textureData = (uint32_t *)malloc(sizeof(uint32_t));
	*textureData = pixel;

	Texture *texturePtr = Texture::Builder()
		.width(uint32_t(1))
		.height(uint32_t(1))
		.levels(0xff)
		.format(driver::TextureFormat::RGBA8)
		.build(*mEngine);

	Texture::PixelBufferDescriptor defaultNormalBuffer(textureData,
		size_t(1 * 1 * 4),
		Texture::Format::RGBA,
		Texture::Type::UBYTE,
		(driver::BufferDescriptor::Callback) &free);

	texturePtr->setImage(*mEngine, 0, std::move(defaultNormalBuffer));
	texturePtr->generateMipmaps(*mEngine);

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
					.build(*mEngine);
				Texture::PixelBufferDescriptor buffer(data, size_t(w * h * 3),
					Texture::Format::RGB, Texture::Type::UBYTE,
					(driver::BufferDescriptor::Callback) &stbi_image_free);
				(*map)->setImage(*mEngine, 0, std::move(buffer));
				(*map)->generateMipmaps(*mEngine);
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

bool RenderableObject::genMaterial(std::string mtFile)
{
	Path path(mtFile);
	std::string name(path.getName());

	if (mMtls.find(name) == mMtls.end()) {
		std::cerr << "material " << name << " already exist." << std::endl;
		return false;
	}

	Mtl mtl;

	loadTexture(path.concat(name + "_Color.png"), &mtl.g_baseColorMap);
	loadTexture(path.concat(name + "_Metallic.png"), &mtl.g_metallicMap, false);
	loadTexture(path.concat(name + "_Roughness.png"), &mtl.g_roughnessMap, false);
	loadTexture(path.concat(name + "_AO.png"), &mtl.g_aoMap, false);
	loadTexture(path.concat(name + "_Normal.png"), &mtl.g_normalMap, false);

	bool hasBaseColorMap = mtl.g_baseColorMap != nullptr;
	bool hasMetallicMap = mtl.g_metallicMap != nullptr;
	bool hasRoughnessMap = mtl.g_roughnessMap != nullptr;
	bool hasAOMap = mtl.g_aoMap != nullptr;
	bool hasNormalMap = mtl.g_normalMap != nullptr;

	if (!hasBaseColorMap && !hasMetallicMap && !hasRoughnessMap && !hasAOMap && !hasNormalMap) {
		std::cerr << "No material" << std::endl;
		return false;
	}

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

	if (mtl.clearCoat) {
		shader += R"SHADER(
            material.clearCoat = 1.0;
        )SHADER";
	}
	if (mtl.anisotropy) {
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

	mtl.g_material = Material::Builder().package(pkg.getData(), pkg.getSize()).build(*mEngine);
	
	mMtls[name] = mtl;
	return true;
}


bool RenderableObject::genRenderable(std::string objName, int numVert, float *mVert, float *mTBNs, float *mUVs, int numFaces, uint16_t* mFaces)
{
	if (mRenderables.find(objName) != mRenderables.end()) {
		std::cerr << "renderable object " << objName << "  already exist." << std::endl;
		return false;
	}
	
	if (numVert == 0 || mVert == nullptr || mTBNs == nullptr ||
		numFaces == 0 || mFaces == nullptr) {
		std::cerr << "errror" << std::endl;
		return false;
	}

	bool hasUVs = mUVs != nullptr ? true : false;

	Object obj;
	if (hasUVs) {
		obj.mVertexBuffer = VertexBuffer::Builder()
			.vertexCount(numVert)
			.bufferCount(3)
			.attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
			.attribute(VertexAttribute::TANGENTS, 1, VertexBuffer::AttributeType::FLOAT4)
			.normalized(VertexAttribute::TANGENTS)
			.attribute(VertexAttribute::UV0, 2, VertexBuffer::AttributeType::FLOAT2)
			.normalized(VertexAttribute::UV0)
			.build(*mEngine);

		obj.mVertexBuffer->setBufferAt(*mEngine, 0,
			VertexBuffer::BufferDescriptor(mVert, numVert * 3 * sizeof(float), nullptr));
		obj.mVertexBuffer->setBufferAt(*mEngine, 1,
			VertexBuffer::BufferDescriptor(mTBNs, numVert * 4 * sizeof(float), nullptr));
		obj.mVertexBuffer->setBufferAt(*mEngine, 2,
			VertexBuffer::BufferDescriptor(mUVs, numVert * 2 * sizeof(float), nullptr));
	}
	else {
		obj.mVertexBuffer = VertexBuffer::Builder()
			.vertexCount(numVert)
			.bufferCount(2)
			.attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
			.attribute(VertexAttribute::TANGENTS, 1, VertexBuffer::AttributeType::FLOAT4)
			.normalized(VertexAttribute::TANGENTS)
			.build(*mEngine);

		obj.mVertexBuffer->setBufferAt(*mEngine, 0,
			VertexBuffer::BufferDescriptor(mVert, numVert * 3 * sizeof(float), nullptr));
		obj.mVertexBuffer->setBufferAt(*mEngine, 1,
			VertexBuffer::BufferDescriptor(mTBNs, numVert * 4 * sizeof(float), nullptr));
	}

	obj.mIndexBuffer = IndexBuffer::Builder()
		.indexCount(numFaces * 3)
		.bufferType(IndexBuffer::IndexType::USHORT)
		.build(*mEngine);

	obj.mIndexBuffer->setBuffer(*mEngine,
		IndexBuffer::BufferDescriptor(mFaces, numFaces * 3 * sizeof(uint16_t), nullptr));


	// always add the DefaultMaterial (with its default parameters), so we don't pick-up
	// whatever defaults is used in mesh
	if (hasUVs && mMtls.find(objName) != mMtls.end()) {

		obj.mtlInstance = mMtls[objName].g_material->createInstance();

		TextureSampler sampler(TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR,
			TextureSampler::MagFilter::LINEAR, TextureSampler::WrapMode::REPEAT);
		sampler.setAnisotropy(8.0f);

		if (mMtls[objName].g_baseColorMap != nullptr) {
			obj.mtlInstance->setParameter(
				"baseColorMap", mMtls[objName].g_baseColorMap, sampler);
		}
		if (mMtls[objName].g_metallicMap != nullptr) {
			obj.mtlInstance->setParameter(
				"metallicMap", mMtls[objName].g_metallicMap, sampler);
		}
		if (mMtls[objName].g_roughnessMap != nullptr) {
			obj.mtlInstance->setParameter(
				"roughnessMap", mMtls[objName].g_roughnessMap, sampler);
		}
		if (mMtls[objName].g_aoMap != nullptr) {
			obj.mtlInstance->setParameter(
				"aoMap", mMtls[objName].g_aoMap, sampler);
		}
		if (mMtls[objName].g_normalMap != nullptr) {
			obj.mtlInstance->setParameter(
				"normalMap", mMtls[objName].g_normalMap, sampler);
		}
	}
	else {
		obj.mtlInstance = mDefaultColorMaterial->createInstance();
	}
	
	// create renderable entity
	EntityManager::get().create(1, &obj.renderable);

	RenderableManager::Builder(1)
		.boundingBox({ {-1.f, -1.f, -1.f}, {1.f, 1.f, 1.f} })
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, obj.mVertexBuffer, obj.mIndexBuffer, 0, numFaces * 3)
		.material(0, obj.mtlInstance)
		.build(*mEngine, obj.renderable);


	// 其实还不太懂，这里设置 parent 是什么意思
	TransformManager& tcm2 = mEngine->getTransformManager();
	TransformManager::Instance parent(tcm2.getInstance(rootEntity));
	tcm2.create(obj.renderable, parent, mat4f());
	mScene->addEntity(obj.renderable);

	mRenderables[objName] = obj;
	return true;
}

bool RenderableObject::genLight(std::string lightName)
{
	if (mLights.find(lightName) != mLights.end()) {
		std::cerr << "Light with name: " << lightName << " was already existed." << std::endl;
		return false;
	}

	Entity g_light = EntityManager::get().create();
	LightManager::Builder(LightManager::Type::DIRECTIONAL)
		.color(Color::toLinear<ACCURATE>({ 0.98f, 0.92f, 0.89f }))
		.intensity(110000)
		.direction({ 0.6, -1, -0.8 })
		.build(*mEngine, g_light);
	mScene->addEntity(g_light);

	mLights[lightName] = g_light;
	return true;
}

void RenderableObject::updateObjectOriant(std::string objName, float* pos, float* ori)
{
	if (mRenderables.find(objName) == mRenderables.end()) {
		std::cerr << "there is no object " << objName << " when update the oriant." << std::endl;
		return;
	}

	Entity &renderable = mRenderables[objName].renderable;

	auto& rcm = mEngine->getRenderableManager();
	auto& tcm = mEngine->getTransformManager();

	
	if (rcm.hasComponent(renderable)) {
		auto ti = tcm.getInstance(renderable);
		tcm.setTransform(ti, mat4f::translation(float3{pos[0], pos[1], pos[2]}));
	}
}