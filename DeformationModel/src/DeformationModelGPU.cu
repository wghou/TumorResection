// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// DeformationModelGPU is based on descent method.

#include"DeformationModelGPU.h"

#include"logger.h"
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
	m_model->stiffness_0 = 40000;	//2000000
	m_model->stiffness_1 = 200000;	//2000000
	m_model->stiffness_2 = 0;	//2000000
	m_model->stiffness_3 = 0.5;
	m_model->stiffness_p = 1000000;

	//m_model->model = NH_MODEL;
	//m_model->stiffness_0 = 200000;	//2000000
	//m_model->stiffness_1 = 2000000;	//2000000
	//m_model->stiffness_2 = 0;	//2000000
	//m_model->stiffness_3 = 0.5;
	//m_model->stiffness_p = 1000000;

	m_model->gravity = 0;

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


	m_model->control_mag = 1000000;
	m_model->profile_v[2] = 0.9997;
}


DeformationModelGPU::~DeformationModelGPU()
{
	if (m_model) delete m_model;
}


void DeformationModelGPU::Initialize(DfModel_Config & config)
{
	// read node coordinate frome file
	// trans -> rotate -> scale
	m_model->Read_Original_File(config.fileName.c_str(), config.indexDec);

	if(config.centralize) m_model->Centralize();
	else m_model->Translate(config.trans[0], config.trans[1], config.trans[2]);

	m_model->Rotate_X(config.rote[0] * 3.14159);
	m_model->Rotate_Y(config.rote[1] * 3.14159);
	m_model->Rotate_Z(config.rote[2] * 3.14159);

	m_model->Scale(config.scale);


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

	Logger::getMainLogger().log(Logger::Level::Info, "vertex number: " + std::to_string(m_model->number) + "   tetrahedron number: " + std::to_string(m_model->tet_number), "");
	//printf("N: %d, %d\n", m_model->number, m_model->tet_number);

	m_model->Initialize(1.0f);

	m_model->Build_VN();
}


void DeformationModelGPU::timeStep(float time)
{
	m_model->Update(time, 97, vertexDir);
	m_model->Build_VN();
}


void DeformationModelGPU::Reset_More_Fixed(int select_v, float dir[])
{
	m_model->Reset_More_Fixed(select_v);
	vertexDir[0] = dir[0];
	vertexDir[1] = dir[1];
	vertexDir[2] = dir[2];
}


void DeformationModelGPU::SetExternalForce(float* externalForce)
{
	m_model->SetExternalForce(externalForce);
}


int DeformationModelGPU::getNodeNumber() { return m_model->number; }

float* DeformationModelGPU::getX() { return m_model->X; }

float* DeformationModelGPU::getVel() { return m_model->V; }

float* DeformationModelGPU::getVN() { return m_model->VN; }

int DeformationModelGPU::getTriNumber() { return m_model->t_number; }

uint16_t* DeformationModelGPU::getTriIndex() { return m_model->T; }

uint16_t* DeformationModelGPU::getTet() { return m_model->Tet; }

int DeformationModelGPU::getTetNumber() { return m_model->tet_number; }


void DeformationModelGPU::Scale(float s)
{
	m_model->Scale(s);
}

void DeformationModelGPU::Translate(float x, float y, float z)
{
	m_model->Translate(x, y, z);
}

void DeformationModelGPU::Rotate_X(float angle)
{
	m_model->Rotate_X(angle);
}

void DeformationModelGPU::Rotate_Y(float angle)
{
	m_model->Rotate_Y(angle);
}

void DeformationModelGPU::Rotate_Z(float angle)
{
	m_model->Rotate_Z(angle);
}