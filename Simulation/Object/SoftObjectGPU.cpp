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


SoftObjectGPU::SoftObjectGPU(char* fileName)
{
	DfModel_Config config;
	m_deformationModel = new DeformationModelGPU();
	m_deformationModel->Initialize(config);

	m_mesh.numVertices = m_deformationModel->getNodeNumber();
	m_mesh.numFaces = m_deformationModel->getTriNumber();
	m_mesh.numTets = m_deformationModel->getTetNumber();
	
	m_mesh.mVertices = m_deformationModel->getX();
	m_mesh.mNormals = m_deformationModel->getVN();
	m_mesh.mFaces = m_deformationModel->getTriIndex();

	// collision
	m_collision = new MyCollision(this);
	
	initialized = true;
	return;
}

SoftObjectGPU::~SoftObjectGPU()
{
	if (m_deformationModel) delete m_deformationModel;

	if (m_collision) delete m_collision;
}

void SoftObjectGPU::timeStep(float time)
{
	// Î´³õÊ¼»¯
	if (initialized == false) return;

	m_deformationModel->Reset_More_Fixed(more_fixed, dir);
	m_deformationModel->timeStep(time);
	//m_deformationModel->Update(time, 96, dir);
	m_deformationModel->Rotate_Z(0.01);
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