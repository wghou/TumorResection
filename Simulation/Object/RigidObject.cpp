// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Instrument is 

#include"RigidObject.h"

#include"OBJLoader.h"
#include"Logger.h"
#include <iostream>
#include <fstream>

#include"Collision\MyCollision.h"
#include"Object\SoftObjectGPU.h"



RigidObject::RigidObject(char* fileName)
{
	m_loader = new OBJLoader();

	m_loader->loadObj(fileName);
	m_mesh.numVertices = m_loader->getNumVertices();
	if (m_loader->getNumVertices() != 0)
	{
		m_mesh.mVertices = new float[m_loader->getVertices().size()];
		memcpy(m_mesh.mVertices, m_loader->getVertices().data(), sizeof(float)*m_loader->getVertices().size());

		m_mesh.mNormals = new float[m_loader->getNormals().size()];
		memcpy(m_mesh.mNormals, m_loader->getNormals().data(), sizeof(float)*m_loader->getNormals().size());

		m_mesh.mTBNs = new float[m_loader->getTBNs().size()];
		memcpy(m_mesh.mTBNs, m_loader->getTBNs().data(), sizeof(float)*m_loader->getTBNs().size());
	}
	
	m_mesh.numFaces = m_loader->getNumFaces();
	if (m_loader->getNumFaces() != 0)
	{
		m_mesh.mFaces = new int[m_loader->getFaces().size()];
		memcpy(m_mesh.mFaces, m_loader->getFaces().data(), sizeof(float)*m_loader->getFaces().size());
	}

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

	if (m_loader) delete m_loader;
	if (m_collision) delete m_collision;
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