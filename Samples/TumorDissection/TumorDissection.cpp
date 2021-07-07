﻿// TumorDissection.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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

#include "BrainDissection.h"

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

	ObjectBase* obj_drill = new Drill("../assets/models/gasper/");
	obj_drill->createRenderableObject(&objRef);
	simulator.addRigidObject(obj_drill);

	ObjectBase* obj_knife = new Knife("../assets/models/drill/");
	obj_knife->createRenderableObject(&objRef);
	simulator.addRigidObject(obj_knife);

	ObjectBase* obj_brain = new SftBrainTumor("../assets/models/dissection/"); 
	obj_brain->createRenderableObject(&objRef);
	simulator.addSoftObject(obj_brain);


	InteractionBase* itc_bs = new InteractionTraction();
	dynamic_cast<InteractionTraction*>(itc_bs)->init(dynamic_cast<Drill*>(obj_drill), dynamic_cast<SftBrainTumor*>(obj_brain));
	simulator.addInteractions(itc_bs);

	InteractionBase* itc_bs_knife = new InteractionDissect();
	dynamic_cast<InteractionDissect*>(itc_bs_knife)->init(dynamic_cast<Knife*>(obj_knife), dynamic_cast<SftBrainTumor*>(obj_brain));
	simulator.addInteractions(itc_bs_knife);


	objRef.genLight("light1");

	return;
}

static void preRender(filament::Engine*, filament::View* view, filament::Scene*, filament::Renderer*) {

}

static void animate(Engine* engine, View* view, Scene* scene, double deltaTime)
{
	simulator.stepSimulation(deltaTime);
	simulator.post2RenderBuffer();

	return;

	RenderableObject& objRef = RenderableObject::get(engine, scene);

	objRef.updateObjectOriant("brain",mat4f {
		mat3f(g_config.scale), float3(0.0f, 0.0f, 0.0f)
	});
	objRef.updateObjectOriant("membrane", mat4f{
		mat3f(g_config.scale), float3(0.0f, 0.0f, 0.0f)
		});
	objRef.updateObjectOriant("tumor", mat4f{
		mat3f(g_config.scale), float3(0.0f, 0.0f, 0.0f)
		});
}

int main(int argc, char *argv[]) {

	g_config.scale = 2.0f;
	g_config.title = "Brain Tumor Dissection";
	g_config.iblDirectory = "../envs/pillars";

	FilamentApp& filamentApp = FilamentApp::get();
	filamentApp.animate(animate);
	//filamentApp.animateStep(animate);
	filamentApp.run(g_config, setup, cleanup, nullptr, preRender, nullptr);

	return 0;
}