// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Instrument is 

#include"RigidObject.h"

#include"ObjLoader.h"
#include"Logger.h"
#include <iostream>
#include <fstream>

#include"Collision/MyCollision.h"
#include"Object/SoftObjectGPU.h"

#include"FilamentWinApp/RenderableObject.h"
#include"utils/Path.h"



RigidObject::RigidObject(char* fileName)
{
	ObjLoader m_loader;

	Path filePath(fileName);
	if (!filePath.exists()) {
		Logger::getMainLogger().log(Logger::Level::Error, "The file " + filePath.getName() + " doesnt exist.", "RigidObject::RigidObject");
		initialized = false;
		return;
	}

	m_loader.loadObj(fileName);
	m_mesh.numVertices = m_loader.getNumVertices();
	if (m_loader.getNumVertices() != 0)
	{
		m_mesh.mVertices = new float[m_mesh.numVertices * 3];
		memcpy(m_mesh.mVertices, m_loader.getVertices(), sizeof(float)*m_mesh.numVertices * 3);

		m_mesh.mTBNs = new float[m_mesh.numVertices * 4];
		memcpy(m_mesh.mTBNs, m_loader.getTBNs(), sizeof(float)*m_mesh.numVertices * 4);

		if (m_loader.getUVs() != nullptr) {
			m_mesh.mUV0 = new float[m_mesh.numVertices * 2];
			memcpy(m_mesh.mUV0, m_loader.getUVs(), m_mesh.numVertices * 2);
		}
	}
	else {
		Logger::getMainLogger().log(Logger::Level::Error, "There is no vertex in the obj: " + string(fileName), "");
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
		Logger::getMainLogger().log(Logger::Level::Error, "There is no surface mesh in the obj: " + string(fileName), "");
		initialized = false;
		return;
	}

	m_objName = filePath.getName();
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

	if (m_collision) delete m_collision;
}

bool RigidObject::createRenderableObject(RenderableObject* rdFactory, std::string objName)
{
	m_rdFactory = rdFactory;

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