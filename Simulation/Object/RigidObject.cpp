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

#include"FilamentWinApp/RenderableObject.h"
#include"utils/Path.h"



RigidObject::RigidObject(std::string filePath)
{
	ObjLoader m_loader;

	Path _path(filePath);
	if (!_path.exists()) {
		Logger::getMainLogger().log(Logger::Level::Error, "The file " + _path.getName() + " doesnt exist.", "RigidObject::RigidObject");
		initialized = false;
		return;
	}

	std::string objFileName = _path.concat(_path.getName() + ".obj");
	std::string jsonFileName = _path.concat(_path.getName() + ".json");

	// load .obj file
	m_loader.loadObj(objFileName);

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
			m_loader.translate(trans[0], trans[1], trans[2]);
			m_loader.scale(sl);
		}
		catch (std::exception &ex) {
			Logger::getMainLogger().log(Logger::Level::Error, "Error when initial json: " + std::string(ex.what()), "RigidObject::RigidObject");
		}
	}
	else {
		Logger::getMainLogger().log(Logger::Level::Error, "Cannot open config file: " + jsonFileName, "RigidObject::RigidObject");
	}

	
	m_mesh.numVertices = m_loader.getNumVertices();
	if (m_loader.getNumVertices() != 0)
	{
		m_mesh.mVertices = new float[m_mesh.numVertices * 3];
		memcpy(m_mesh.mVertices, m_loader.getVertices(), sizeof(float)*m_mesh.numVertices * 3);

		m_mesh.mTBNs = new float[m_mesh.numVertices * 4];
		memcpy(m_mesh.mTBNs, m_loader.getTBNs(), sizeof(float)*m_mesh.numVertices * 4);

		if (m_loader.getUVs() != nullptr) {
			m_mesh.mUV0 = new float[m_mesh.numVertices * 2];
			memcpy(m_mesh.mUV0, m_loader.getUVs(), sizeof(float)*m_mesh.numVertices * 2);
		}
	}
	else {
		Logger::getMainLogger().log(Logger::Level::Error, "There is no vertex in the obj: " + string(_path.getName()), "");
		initialized = false;
		return;
	}
	
	m_mesh.numFaces = m_loader.getNumFaces();
	if (m_mesh.numFaces != 0)
	{
		m_mesh.mFaces = new uint16_t[m_mesh.numFaces * 3];
		memcpy(m_mesh.mFaces, m_loader.getFaces(), sizeof(uint16_t)*m_mesh.numFaces * 3);
	}
	else {
		Logger::getMainLogger().log(Logger::Level::Error, "There is no surface mesh in the obj: " + string(_path.getName()), "");
		initialized = false;
		return;
	}


	m_objName = _path.getNameWithoutExtension();
	// 初始化成功
	initialized = true;
	return;
}

RigidObject::~RigidObject()
{
	if (m_mesh.mVertices) delete[] m_mesh.mVertices;
	if (m_mesh.mNormals) delete[] m_mesh.mNormals;
	if (m_mesh.mTBNs) delete[] m_mesh.mTBNs;
	if (m_mesh.mUV0) delete[] m_mesh.mUV0;
	if (m_mesh.mUV1) delete[] m_mesh.mUV1;
}

bool RigidObject::createRenderableObject(RenderableObject* rdFactory, std::string objName)
{
	m_rdFactory = rdFactory;

	// 导入纹理
	if (!m_mtlPath.empty()) {
		rdFactory->genMaterial(m_mtlPath);
	}
	else {
		Logger::getMainLogger().log(Logger::Level::Warning, "There is no material for object " + objName, "RigidObject::createRenderableObject");
	}
	
	// 生成渲染对象
	bool rlt = rdFactory->genRenderable(objName, m_mesh.numVertices, m_mesh.mVertices,
		m_mesh.mTBNs, m_mesh.mUV0, m_mesh.numFaces, m_mesh.mFaces);
	return rlt;
}

void RigidObject::timeStep(float time)
{

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