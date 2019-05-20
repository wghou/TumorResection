// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// DeformationModelGPU is based on descent method.

#include"DeformationModelGPU.h"

#include"Logger\include\Logger.h"
#include <iostream>
#include <fstream>

#include "Lib/CUDA_HYPER_TET_MESH.h"



DeformationModelGPU::DeformationModelGPU()
{
	m_model = new CUDA_HYPER_TET_MESH<float>();

	//Neo-Hookean
	m_model->stiffness_0 = 2000000;	//2000000
	m_model->stiffness_1 = 2000000;	//2000000

										//stVK
										//	stiffness_0 = 100000;	//2000000
										//	stiffness_1 = 6000000;	//2000000
										//	stiffness_k = 5000;	//2000

										//Mooney
										//	stiffness_0	= 2000000;	//2000000
										//   stiffness_1	= 2000000;	//2000000
										//   stiffness_2	= 2000000;	//2000000
										//	stiffness_3 = 0.5;



	m_model->control_mag = 100000;  //100000

										//Neo-Hookean
	m_model->stiffness_0 = 2000000;	//2000000
	m_model->stiffness_1 = 2000000;	//2000000

										//stVK

	m_model->model = NH_MODEL;
	m_model->stiffness_0 = 1000;	//2000000
	m_model->stiffness_1 = 5000;	//2000000
	m_model->stiffness_2 = 0;	//2000000
	m_model->stiffness_3 = 0.25;
	m_model->stiffness_p = 24000;

	//m_model->model = NH_MODEL;
	//m_model->stiffness_0 = 200000;	//2000000
	//m_model->stiffness_1 = 2000000;	//2000000
	//m_model->stiffness_2 = 0;	//2000000
	//m_model->stiffness_3 = 0.5;
	//m_model->stiffness_p = 1000000;

	m_model->gravity = 0.0;
	m_model->density = 1000;

	//m_model->model = NH_MODEL;
	//m_model->stiffness_0 = 2000000;	//2000000
	//m_model->stiffness_1 = 20000000;	//2000000
	//m_model->stiffness_2 = 0;	//2000000
	//m_model->stiffness_3 = 0.5;
	//m_model->stiffness_p = 10000000;

	m_model->lower_bound = 0.15;
	m_model->upper_bound = 1000.0;



	//Mooney
	// stiffness_0	= 2000000;	//2000000
	// stiffness_1	= 2000000;	//2000000
	// stiffness_2	= 2000000;	//2000000
	// stiffness_3 = 0.5;


	m_model->control_mag = 10000;
	m_model->profile_v[2] = 0.9997;
}


DeformationModelGPU::~DeformationModelGPU()
{
	if (m_model) delete m_model;
}


void DeformationModelGPU::Initialize(DfModel_Config & config)
{
	m_model->number = config.numVertex;
	memcpy(m_model->X, config.mVertices, sizeof(float) * 3 * config.numVertex);
	m_model->tet_number = config.numTet;
	memcpy(m_model->Tet, config.mTets, sizeof(uint16_t) * 4 * config.numTet);

	// 当 fixedXYZ == 0 时，不固定
	for (int i = 0; i < m_model->number; i++)
	{
		if( (config.fixedAxisUp[0] != 0 && m_model->X[i * 3 + 0] > config.fixedAxisUp[0]) ||
			(config.fixedAxisBt[0] != 0 && m_model->X[i * 3 + 0] < config.fixedAxisBt[0]) ||
			(config.fixedAxisUp[1] != 0 && m_model->X[i * 3 + 1] > config.fixedAxisUp[1]) ||
			(config.fixedAxisBt[1] != 0 && m_model->X[i * 3 + 1] < config.fixedAxisBt[1]) ||
			(config.fixedAxisUp[2] != 0 && m_model->X[i * 3 + 2] > config.fixedAxisUp[2]) ||
			(config.fixedAxisBt[2] != 0 && m_model->X[i * 3 + 2] < config.fixedAxisBt[2]) )
			m_model->fixed[i] = 100000;	
	}

	// 固定 fixed
	for (int i = 0; i < config.fixedVertices.size(); i++) {
		m_model->fixed[config.fixedVertices[i]] = 100000;
	}
	//std::cout << config.fixedVertices.size();

	m_model->Initialize(1.0f);
}


void DeformationModelGPU::timeStep(float time)
{
	m_model->Update(time, 97, vertexDir);
}


void DeformationModelGPU::Reset_More_Fixed(int select_v, float dir[])
{
	if (select_v >= m_model->number) return;

	m_model->Reset_More_Fixed(select_v);
	vertexDir[0] = dir[0];
	vertexDir[1] = dir[1];
	vertexDir[2] = dir[2];
}


void DeformationModelGPU::SetExternalForce(float* externalForce)
{
	//m_model->SetExternalForce(externalForce);
}


int DeformationModelGPU::getNodeNumber() { return m_model->number; }

float* DeformationModelGPU::getX() { return m_model->X; }

float* DeformationModelGPU::getVel() { return m_model->V; }

uint16_t* DeformationModelGPU::getTet() { return m_model->Tet; }

int DeformationModelGPU::getTetNumber() { return m_model->tet_number; }