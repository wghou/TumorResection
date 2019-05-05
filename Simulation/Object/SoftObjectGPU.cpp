// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// SoftObjectGPU is 

#include"SoftObjectGPU.h"

#include"logger.h"
#include <iostream>
#include <fstream>

#include"DeformationModelGPU.h"
#include"Collision\MyCollision.h"

#include"FilamentWinApp/RenderableObject.h"
#include"ElementLoader.h"
#include"TBN.h"


SoftObjectGPU::SoftObjectGPU(char* fileName)
{
	ElementLoader m_loader;
	m_loader.loadElement(fileName);

	if (m_loader.getNumVertices() == 0 || m_loader.getNumTets() == 0 || m_loader.getNumVertices() == 0) {
		Logger::getMainLogger().log(Logger::Level::Error, "There is no mesh in the eleFile.", "SoftObjectGPU::SoftObjectGPU");
		initialized = false;
		return;
	}

	DfModel_Config config;
	config.numVertex = m_loader.getNumVertices();
	config.mVertices = m_loader.getVertices();
	config.numTet = m_loader.getNumTets();
	config.mTets = m_loader.getTets();

	m_deformationModel = new DeformationModelGPU();
	m_deformationModel->Initialize(config);

	m_mesh.numVertices = m_loader.getNumVertices();
	m_mesh.mVertices = m_deformationModel->getX();
	m_mesh.mNormals = new float[m_mesh.numVertices * 3];
	m_mesh.mTBNs = new float[m_mesh.numVertices * 4];
	m_mesh.mUV0 = new float[m_mesh.numVertices * 2];
	memcpy(m_mesh.mUV0, m_loader.getUVs(), m_mesh.numVertices * sizeof(float) * 2);

	// surface mesh
	m_mesh.numFaces = m_loader.getNumFaceVertices();
	m_mesh.mFaces = new uint16_t[m_mesh.numFaces * 3];
	memcpy(m_mesh.mFaces, m_loader.getFaces(), sizeof(uint16_t)*m_mesh.numFaces * 3);

	TBN::buildVns(m_mesh.numFaces, m_mesh.mFaces, m_mesh.numVertices, m_mesh.mVertices, m_mesh.mNormals);
	TBN::updateTBNs(m_mesh.numVertices, m_mesh.mNormals, m_mesh.mTBNs);

	// collision
	m_collision = new MyCollision(this);
	
	initialized = true;
	return;
}

SoftObjectGPU::~SoftObjectGPU()
{
	if (m_deformationModel) delete m_deformationModel;

	if (m_collision) delete m_collision;

	// delete mesh buffer
	if (m_mesh.mFaces) delete[] m_mesh.mFaces;
	if (m_mesh.mNormals) delete[] m_mesh.mNormals;
	if (m_mesh.mTBNs) delete[] m_mesh.mTBNs;
	if (m_mesh.mUV0) delete[] m_mesh.mUV0;
}


bool SoftObjectGPU::createRenderableObject(RenderableObject* rdFactory, std::string objName)
{
	m_rdFactory = rdFactory;

	bool rlt = rdFactory->genRenderable(objName, m_mesh.numVertices, m_mesh.mVertices,
		m_mesh.mTBNs, m_mesh.mUV0, m_mesh.numFaces, m_mesh.mFaces);
	return rlt;
}


void SoftObjectGPU::timeStep(float time)
{
	// Î´³õÊ¼»¯
	if (initialized == false) return;

	m_deformationModel->Reset_More_Fixed(more_fixed, dir);
	m_deformationModel->timeStep(time);

	TBN::buildVns(m_mesh.numFaces, m_mesh.mFaces, m_mesh.numVertices, m_mesh.mVertices, m_mesh.mNormals);
	TBN::updateTBNs(m_mesh.numVertices, m_mesh.mNormals, m_mesh.mTBNs);

	//
}


void SoftObjectGPU::collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder)
{
	// Î´³õÊ¼»¯
	if (initialized == false) return;


	// ²»²ÎÓëÅö×²¼ì²â
	if (!isPerformCollisionDetection) return;


	// Ö´ÐÐÅö×²¼ì²â
	this->m_collision->computeCollision(obj_other->getCollisionObject(), recorder);
}