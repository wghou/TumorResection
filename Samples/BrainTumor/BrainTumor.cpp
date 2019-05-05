﻿// BrainTumor.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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

#include"Object/Object.h"
#include"SimulationScene/SimulationSceneC.h"

#include "BrainTumor.h"

using namespace filament::math;
using namespace filament;
using namespace utils;

static Config g_config;
static SimulationSceneC simulator;


static void cleanup(Engine* engine, View*, Scene* secne) {

	simulator.cleanUp();
	RenderableObject::get(engine, secne).cleanUp();
}

static void setup(Engine* engine, View* view, Scene* scene) {
	
	RenderableObject& objRef = RenderableObject::get(engine, scene);

	ObjectBase* obj_drill = new Drill();
	obj_drill->createRenderableObject(&objRef, "Drill");
	simulator.addRigidObject(obj_drill);

	ObjectBase* obj_liver = new SoftObjectGPU("../assets/models/liver/myLiver");
	obj_liver->createRenderableObject(&objRef, "Liver");
	simulator.addSoftObject(obj_liver);

	return;
}

static void preRender(filament::Engine*, filament::View* view, filament::Scene*, filament::Renderer*) {

}

static void animate(Engine* engine, View* view, double now)
{
	simulator.stepSimulation(now);
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