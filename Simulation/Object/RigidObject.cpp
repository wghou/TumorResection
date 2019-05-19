// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Instrument is 

#include"RigidObject.h"

#include"ObjLoader\ObjLoader.h"
#include"Logger\include\Logger.h"
#include"json/json_helper.h"
#include <iostream>
#include <fstream>

#include"Collision/MyCollision.h"
#include"Object/SoftObjectGPU.h"
#include"Object/SurfaceMesh.h"

#include"FilamentWinApp/RenderableObject.h"
#include"utils/Path.h"



RigidObject::RigidObject(std::string filePath)
{
	m_loader = new ObjLoader();

	Path _path(filePath);
	if (!_path.exists()) {
		Logger::getMainLogger().log(Logger::Level::Error, "The file " + _path.getName() + " doesnt exist.", "RigidObject::RigidObject");
		initialized = false;
		return;
	}

	m_objName = _path.getNameWithoutExtension();

	std::string objFileName = _path.concat(_path.getName() + ".obj");
	std::string jsonFileName = _path.concat(_path.getName() + ".json");

	// load .obj file
	m_loader->loadObj(objFileName);

	// material path
	std::string m_mtlPath;

	// open json
	nlohmann::json _json;
	std::ifstream input_file(jsonFileName);
	if (input_file.is_open()) {
		
		float sl = 1.0f;
		float trans[3] = { 0.0f,0.0f,0.0f };

		try {
			_json << input_file;
			json_helper::readVector3f(_json, "trans", trans);
			json_helper::readValue(_json, "scale", sl);
			json_helper::readValue(_json, "mtlPath", m_mtlPath);

			// translate, then scale
			m_loader->translate(trans[0], trans[1], trans[2]);
			m_loader->scale(sl);
		}
		catch (std::exception &ex) {
			Logger::getMainLogger().log(Logger::Level::Error, "Error when initial json: " + std::string(ex.what()), "RigidObject::RigidObject");
		}
	}
	else {
		Logger::getMainLogger().log(Logger::Level::Warning, "Cannot open config file: " + jsonFileName, "RigidObject::RigidObject");
	}

	if (m_loader->getNumVertices() == 0 || m_loader->getNumVertices() == 0) {
		Logger::getMainLogger().log(Logger::Level::Error, "There is no mesh in the eleFile.", "RigidObject::RigidObject");
		initialized = false;
		return;
	}

	m_mesh = new SurfaceMesh(m_loader->getNumVertices(), m_loader->getNumFaces(), m_objName);
	m_mesh->initSurfaceMesh(m_loader->getVertices(), m_loader->getFaces(), m_loader->getUVs(), m_mtlPath);
	
	// 初始化成功
	initialized = true;
	return;
}

RigidObject::~RigidObject()
{
	if (m_mesh) delete m_mesh;
	if (m_loader) delete m_loader;
}

bool RigidObject::createRenderableObject(RenderableObject* rdFactory, std::string objName)
{
	m_rdFactory = rdFactory;

	if (m_rdFactory == nullptr) {
		Logger::getMainLogger().log(Logger::Level::Error, "The *rdFactory is NULL.", "RigidObject::createRenderableObject");
		return false;
	}

	bool rlt = m_mesh->createRenderableObject(rdFactory, objName);
	return rlt;
}

void RigidObject::timeStep(float time)
{
	
}

void RigidObject::post2Render()
{
	m_mesh->rendering(m_rdFactory);
}

void RigidObject::collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder)
{
	// 未初始化
	if (initialized == false) return;


	// 不参与碰撞检测
	if (!isPerformCollisionDetection) return;


	// 执行碰撞检测
	this->m_collision->computeCollision(obj_other->getCollisionObject(), recorder);
}