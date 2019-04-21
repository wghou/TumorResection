// Example2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include <string>
#include <map>
#include <vector>

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

#include <utils/Path.h>
#include "Config.h"
#include "FilamentApp.h"
#include "RenderableObject.h"
#include "ObjLoader/OBJLoader.h"
#include "ObjLoader/ElementLoader.h"

#include "Test.h"

using namespace filament::math;
using namespace filament;
using namespace utils;

static SandboxParameters g_params;
static Config g_config;

static RenderableObject *obj = nullptr;
static ObjLoader *loader;

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

	delete obj;
	delete loader;
}

static void setup(Engine* engine, View* view, Scene* scene) {

	//view->setClearColor({ 0.1, 0.125, 0.25, 1.0 });

	obj = new RenderableObject(*engine);
	loader = new ObjLoader();

	loader->loadObj("../assets/models/monkey/monkey.obj");

	obj->genMaterial("../assets/models/monkey");
	obj->genRenderable(scene, loader->getNumVertices(), loader->getVertices(),
		loader->getTBNs(), loader->getUVs(), loader->getNumFaceVertices(),
		loader->getFaces());

	obj->genLight(scene);
	return;
}

static void preRender(filament::Engine*, filament::View* view, filament::Scene*, filament::Renderer*) {
	view->setAntiAliasing(g_params.fxaa ? View::AntiAliasing::FXAA : View::AntiAliasing::NONE);
	view->setDithering(g_params.dithering ? View::Dithering::TEMPORAL : View::Dithering::NONE);
	view->setSampleCount((uint8_t)(g_params.msaa ? 4 : 1));
}
 
static void animate(Engine* engine, View* view, double now)
{
	return;

	auto& rcm = engine->getRenderableManager();
	auto& tcm = engine->getTransformManager();

	Entity *renderable = obj->renderable;
	if (rcm.hasComponent(*renderable)) {
		auto ti = tcm.getInstance(*renderable);
		tcm.setTransform(ti, filament::math::mat4f::rotation(now, filament::math::float3{ 0, 1, 0 }) *
			mat4f {
			mat3f(g_config.scale), float3(0.0f, 0.0f, 0.0f)
		});
	}
}

int main(int argc, char *argv[]) {

	g_config.scale = 1.0f;
	g_config.title = "Material Sandbox";
	g_config.iblDirectory = "../envs/pillars";

	FilamentApp& filamentApp = FilamentApp::get();
	filamentApp.animate(animate);
	filamentApp.run(g_config, setup, cleanup, nullptr, preRender, nullptr);

	return 0;
}