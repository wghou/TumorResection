#include "RenderableObj.h"

#include <stb/stb_image.h>
#include"resources/resources.h"

RenderableObj::RenderableObj(Engine &engine):mEngine(engine)
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

RenderableObj::~RenderableObj()
{
	mEngine.destroy(vb);
	mEngine.destroy(ib);
	mEngine.destroy(mDefaultColorMaterial);
	mEngine.destroy(mDefaultTransparentColorMaterial);
	mEngine.destroy(mDefaultNormalMap);
	mEngine.destroy(mDefaultMap);

	mEngine.destroy(renderable);

	// destroy the Entities itself
	EntityManager::get().destroy(renderable);
}

void RenderableObj::cleanup()
{
	for (auto& item : g_materialInstances) {
		auto materialInstance = item.second;
		mEngine.destroy(materialInstance);
	}

	mEngine.destroy(g_material);
	mEngine.destroy(g_baseColorMap);
	mEngine.destroy(g_metallicMap);
	mEngine.destroy(g_roughnessMap);
	mEngine.destroy(g_aoMap);
	mEngine.destroy(g_normalMap);
}

void RenderableObj::setup(View* view, Scene* scene, PbrConfig confg)
{
	Path path(g_pbrConfig.materialDir);
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

	if (g_pbrConfig.clearCoat) {
		shader += R"SHADER(
            material.clearCoat = 1.0;
        )SHADER";
	}
	if (g_pbrConfig.anisotropy) {
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

	// vertex buffer
	vb = VertexBuffer::Builder()
		.vertexCount(confg.numVertices)
		.bufferCount(4)
		.attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
		.attribute(VertexAttribute::TANGENTS, 1, VertexBuffer::AttributeType::FLOAT4)
		.normalized(VertexAttribute::TANGENTS)
		.attribute(VertexAttribute::UV0, 2, VertexBuffer::AttributeType::SHORT2)
		.normalized(VertexAttribute::UV0)
		.attribute(VertexAttribute::UV1, 3, VertexBuffer::AttributeType::SHORT2)
		.normalized(VertexAttribute::UV1)
		.build(mEngine);
	vb->setBufferAt(mEngine, 0,
		VertexBuffer::BufferDescriptor(confg.mVertices, confg.numVertices * 3 * sizeof(float), nullptr));
	vb->setBufferAt(mEngine, 1,
		VertexBuffer::BufferDescriptor(confg.mTBNs, confg.numVertices * 4 * sizeof(float), nullptr));
	vb->setBufferAt(mEngine, 2,
		VertexBuffer::BufferDescriptor(confg.mUV0s, confg.numVertices * 2 * sizeof(float), nullptr));
	vb->setBufferAt(mEngine, 3,
		VertexBuffer::BufferDescriptor(confg.mUV1s, confg.numVertices * 2 * sizeof(float), nullptr));


	// index buffer
	ib = IndexBuffer::Builder()
		.indexCount(confg.numFaces * 3)
		.bufferType(IndexBuffer::IndexType::USHORT)
		.build(mEngine);
	ib->setBuffer(mEngine,
		IndexBuffer::BufferDescriptor(confg.mFaces, confg.numFaces * 3 * sizeof(uint16_t), nullptr));

	// always add the DefaultMaterial (with its default parameters), so we don't pick-up
	// whatever defaults is used in mesh
	if (g_materialInstances.find(AI_DEFAULT_MATERIAL_NAME) == g_materialInstances.end()) {
		g_materialInstances[AI_DEFAULT_MATERIAL_NAME] = mDefaultColorMaterial->createInstance();
	}

	// create renderable entity
	EntityManager::get().create(1, &renderable);
	if (rootEntity.isNull()) {
		EntityManager::get().create(1, &rootEntity);

		TransformManager& tcm = mEngine.getTransformManager();
		//Add root instance
		tcm.create(rootEntity, TransformManager::Instance{}, mat4f());
	}

	RenderableManager::Builder(1)
		.boundingBox(confg.aabb)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, vb, ib, 0, confg.numFaces * 3)
		.material(0, g_materialInstances[AI_DEFAULT_MATERIAL_NAME])
		.build(mEngine, renderable);

	// 其实还不太懂，这里设置 parent 是什么意思
	TransformManager& tcm2 = mEngine.getTransformManager();
	TransformManager::Instance parent(tcm2.getInstance(rootEntity));
	tcm2.create(renderable, parent, mat4f());
}

void RenderableObj::loadTexture(const std::string& filePath, Texture** map, bool sRGB) {
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

Texture* RenderableObj::createOneByOneTexture(uint32_t pixel) {
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