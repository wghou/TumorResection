// Example2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>


#include <string>
#include <map>
#include <vector>

#include <utils/Path.h>

#include <filament/Engine.h>
#include <filament/DebugRegistry.h>
#include <filament/IndirectLight.h>
#include <filament/LightManager.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/TransformManager.h>
#include <filament/View.h>
#include <math/mat3.h>
#include <math/mat4.h>
#include <math/vec4.h>
#include <math/norm.h>

#include "Config.h"
#include "IBL.h"
#include "FilamentApp.h"
#include"resources/resources.h"
#include<libsdl2/SDL.h>

#include "Object/Object.h"



#include "Example1.h"

using namespace filament::math;
using namespace filament;
using namespace utils;

struct RenderableObj
{
	Entity renderable;
	VertexBuffer* vb = nullptr;
	IndexBuffer* ib = nullptr;
};

static SandboxParameters g_params;
static Config g_config;


static RenderableObj rd_mky;
static Drill *obj_dirll;
static RenderableObj rd_knife;
static Knife *obj_knife;


static void cleanup(Engine* engine, View*, Scene*) {

	for (auto& i : g_params.materialInstance) {
		engine->destroy(i);
	}

	for (auto& i : g_params.material) {
		engine->destroy(i);
	}

	engine->destroy(g_params.light);
	EntityManager& em = EntityManager::get();
	em.destroy(g_params.light);
}

static void setup(Engine* engine, View* view, Scene* scene) {
	
	//// create material and light
	createInstances(g_params, *engine);

	//// create drill
	obj_dirll = new Drill();
	// vertex buffer
	rd_mky.vb = VertexBuffer::Builder()
		.vertexCount(obj_dirll->getMesh().numVertices)
		.bufferCount(2)
		.attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
		.attribute(VertexAttribute::TANGENTS, 1, VertexBuffer::AttributeType::FLOAT4)
		.normalized(VertexAttribute::TANGENTS)
		.build(*engine);
	rd_mky.vb->setBufferAt(*engine, 0,
		VertexBuffer::BufferDescriptor(obj_dirll->getMesh().mVertices, obj_dirll->getMesh().numVertices * 3 * sizeof(float), nullptr));
	rd_mky.vb->setBufferAt(*engine, 1,
		VertexBuffer::BufferDescriptor(obj_dirll->getMesh().mTBNs, obj_dirll->getMesh().numVertices * 4 * sizeof(float), nullptr));

	// index buffer
	rd_mky.ib = IndexBuffer::Builder()
		.indexCount(obj_dirll->getMesh().numFaces * 3)
		.bufferType(IndexBuffer::IndexType::USHORT)
		.build(*engine);
	rd_mky.ib->setBuffer(*engine,
		IndexBuffer::BufferDescriptor(obj_dirll->getMesh().mFaces, obj_dirll->getMesh().numFaces * 3 * sizeof(uint16_t), nullptr));

	// create renderable entity
	EntityManager::get().create(1, &rd_mky.renderable);

	RenderableManager::Builder(1)
		.boundingBox({ {-1.f, -1.f, -1.f}, {1.f, 1.f, 1.f} })
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, rd_mky.vb, rd_mky.ib, 0, obj_dirll->getMesh().numFaces * 3)
		.build(*engine, rd_mky.renderable);

	// set transform
	auto& tcm = engine->getTransformManager();
	auto ei = tcm.getInstance(rd_mky.renderable);
	auto lt = tcm.getInstance(g_params.light);
	tcm.setParent(ei, lt);

	// set material
	g_params.currentMaterialModel = MATERIAL_LIT;
	g_params.metallic = 0.5f;
	// set parameters
	MaterialInstance* materialInstance = updateInstances(g_params, *engine);
	auto& rcm = engine->getRenderableManager();
	auto instance = rcm.getInstance(rd_mky.renderable);
	rcm.setCastShadows(instance, g_params.castShadows);
	rcm.setMaterialInstanceAt(instance, 0, materialInstance);
	scene->addEntity(rd_mky.renderable);

	// add light
	scene->addEntity(g_params.light);

	return;
}

static void preRender(filament::Engine*, filament::View* view, filament::Scene*, filament::Renderer*) {
	view->setAntiAliasing(g_params.fxaa ? View::AntiAliasing::FXAA : View::AntiAliasing::NONE);
	view->setDithering(g_params.dithering ? View::Dithering::TEMPORAL : View::Dithering::NONE);
	view->setSampleCount((uint8_t)(g_params.msaa ? 4 : 1));
}

static void animate(Engine* engine, View* view, double now)
{
	obj_dirll->timeStep(now);
	mat4f oriant = mat4f{ obj_dirll->getLocalOriant().m[0][0], obj_dirll->getLocalOriant().m[0][1], obj_dirll->getLocalOriant().m[0][2], obj_dirll->getLocalOriant().m[0][3],
						obj_dirll->getLocalOriant().m[1][0], obj_dirll->getLocalOriant().m[1][1], obj_dirll->getLocalOriant().m[1][2], obj_dirll->getLocalOriant().m[1][3],
						obj_dirll->getLocalOriant().m[2][0], obj_dirll->getLocalOriant().m[2][1], obj_dirll->getLocalOriant().m[2][2], obj_dirll->getLocalOriant().m[2][3],
						obj_dirll->getLocalOriant().m[3][0], obj_dirll->getLocalOriant().m[3][1], obj_dirll->getLocalOriant().m[3][2], obj_dirll->getLocalOriant().m[3][3] };
	Vector3f pos = obj_dirll->getLocalPos();
	
	// set transform
	auto& tcm = engine->getTransformManager();
	auto ei = tcm.getInstance(rd_mky.renderable);
	tcm.setTransform(ei, oriant*mat4f::translation(float3(pos.x, pos.y, pos.z)) * mat4f::scaling(2.0f));
}

int main(int argc, char *argv[]) {

	g_config.iblDirectory = "../envs/pillars";
	utils::Path filename = "../assets/models/sphere2.obj";
	if (!filename.exists()) {
		std::cerr << "file " << filename << " not found!" << std::endl;
		return 1;
	}
	g_config.scale = 1.0f;
	g_config.title = "Material Sandbox";

	g_params.metallic = 0.8f;
	g_params.reflectance = 0.5f;
	g_params.roughness = 0.3f;

	FilamentApp& filamentApp = FilamentApp::get();
	filamentApp.animate(animate);
	filamentApp.run(g_config, setup, cleanup, nullptr, preRender, nullptr);

	return 0;
}