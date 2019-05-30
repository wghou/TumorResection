// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// SftBrainTumor is 

#include"SftBrainTumor.h"

#include"Logger/include/Logger.h"
#include"json/json_helper.h"
#include <iostream>
#include <fstream>

#include"DeformationModelGPU.h"
#include"Collision\MyCollision.h"
#include"Object/DfSurfaceMesh.h"

#include"FilamentWinApp/RenderableObject.h"
#include"utils/Path.h"
#include"ObjLoader/ElementLoader.h"
#include"ObjLoader/TBN.h"


SftBrainTumor::SftBrainTumor(std::string filePath)
{
	// material path
	std::string m_mtlPath_brain;
	// load brain element
	{
		m_loader_brain = new ElementLoader();
		Path _path(filePath);
		if (!_path.exists()) {
			Logger::getMainLogger().log(Logger::Level::Error, "The file " + _path.getName() + " doesnt exist.", "SftBrainTumor::SftBrainTumor");
			initialized = false;
			return;
		}

		std::string objFilePath = _path.concat("brain/brain");
		std::string jsonFileName = _path.concat("brain/brain.json");

		// load .ele .node .obj files
		m_loader_brain->loadElement(objFilePath);

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
				json_helper::readValue(_json, "mtlPath", m_mtlPath_brain);

				// translate, then scale
				m_loader_brain->translate(trans[0], trans[1], trans[2]);
				m_loader_brain->scale(sl);
			}
			catch (std::exception &ex) {
				Logger::getMainLogger().log(Logger::Level::Error, "Error when initial json: " + std::string(ex.what()), "SftBrainTumor::SftBrainTumor");
			}
		}
		else {
			Logger::getMainLogger().log(Logger::Level::Error, "Cannot open config file: " + jsonFileName, "SftBrainTumor::SftBrainTumor");
		}


		if (m_loader_brain->getNumVertices() == 0 || m_loader_brain->getNumTets() == 0 || m_loader_brain->getNumVertices() == 0) {
			Logger::getMainLogger().log(Logger::Level::Error, "There is no mesh in the eleFile.", "SftBrainTumor::SftBrainTumor");
			initialized = false;
			return;
		}
	}

	// material path
	std::string m_mtlPath_tumor;
	// load tumor element
	{
		m_loader_tumor = new ElementLoader();
		Path _path(filePath);
		if (!_path.exists()) {
			Logger::getMainLogger().log(Logger::Level::Error, "The file " + _path.getName() + " doesnt exist.", "SftBrainTumor::SftBrainTumor");
			initialized = false;
			return;
		}

		std::string objFilePath = _path.concat("tumor/tumor");
		std::string jsonFileName = _path.concat("tumor/tumor.json");

		// load .ele .node .obj files
		m_loader_tumor->loadElement(objFilePath);


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
				json_helper::readValue(_json, "mtlPath", m_mtlPath_tumor);

				// translate, then scale
				m_loader_tumor->translate(trans[0], trans[1], trans[2]);
				m_loader_tumor->scale(sl);
			}
			catch (std::exception &ex) {
				Logger::getMainLogger().log(Logger::Level::Error, "Error when initial json: " + std::string(ex.what()), "SftBrainTumor::SftBrainTumor");
			}
		}
		else {
			Logger::getMainLogger().log(Logger::Level::Error, "Cannot open config file: " + jsonFileName, "SftBrainTumor::SftBrainTumor");
		}


		if (m_loader_tumor->getNumVertices() == 0 || m_loader_tumor->getNumTets() == 0 || m_loader_tumor->getNumVertices() == 0) {
			Logger::getMainLogger().log(Logger::Level::Error, "There is no mesh in the eleFile.", "SftBrainTumor::SftBrainTumor");
			initialized = false;
			return;
		}
	}

	// generate element of connect
	uint16_t numVertex_connect = 0;
	float* mVertices_connect = nullptr;
	uint16_t numTet_connect = 0;
	uint16_t* mTet_connect = nullptr;
	uint16_t numFace_connect = 0;
	uint16_t* mFace_connect = nullptr;

	// collect all element buffers
	float* mVertices_all = nullptr;
	uint16_t* mTet_all = nullptr;

	// buffer structure:
	// brain vertex  - tumor vertex - connect vertex
	mVertices_all = new float[(m_loader_brain->getTetVertNum() + m_loader_tumor->getTetVertNum() + numVertex_connect) * 3];
	mTet_all = new uint16_t[(m_loader_brain->getNumTets() + m_loader_brain->getNumTets() + numTet_connect) * 4];
	memcpy(mVertices_all, m_loader_brain->getVertices(), sizeof(float)*m_loader_brain->getTetVertNum() * 3);
	memcpy(mVertices_all + m_loader_brain->getTetVertNum() * 3, m_loader_tumor->getVertices(), sizeof(float)*m_loader_tumor->getTetVertNum() * 3);
	//memcpy(mVertices_all + m_loader_brain->getTetVertNum() * 3 + m_loader_tumor->getTetVertNum() * 3, 
	//	mVertices_connect, sizeof(float)*numVertex_connect * 3);
	memcpy(mTet_all, m_loader_brain->getTets(), sizeof(float)*m_loader_brain->getNumTets() * 4);
	memcpy(mTet_all + m_loader_brain->getNumTets() * 4, m_loader_tumor->getTets(), sizeof(float)*m_loader_tumor->getNumTets() * 4);
	//memcpy(mTet_all + m_loader_brain->getTetVertNum() * 3 + m_loader_tumor->getTetVertNum() * 4, 
	//	mTet_connect, sizeof(float)*numTet_connect * 4);

	
	// init deformation model
	DfModel_Config config;
	config.numVertex = m_loader_brain->getTetVertNum() + m_loader_tumor->getTetVertNum() + numVertex_connect;
	config.mVertices = mVertices_all;
	config.numTet = m_loader_brain->getNumTets() + m_loader_tumor->getNumTets() + numTet_connect;
	config.mTets = mTet_all;

	// only the brain contain fixed vertices
	config.fixedVertices.assign(m_loader_brain->getFixed().begin(), m_loader_brain->getFixed().end());

	m_deformationModel = new DeformationModelGPU();
	m_deformationModel->Initialize(config);

	delete[] mVertices_all; delete[] mTet_all;


	// init brain surface mesh
	m_mesh_brain = new DfSurfaceMesh(m_loader_brain->getNumVertices(), m_loader_brain->getNumFaces(), "brain");
	m_mesh_brain->initSurfaceMesh(m_loader_brain->getVertices(), m_loader_brain->getFaces(), m_loader_brain->getUVs(), m_mtlPath_brain);
	dynamic_cast<DfSurfaceMesh*>(m_mesh_brain)->setVertCpys(m_loader_brain->getTetVertNum(), m_deformationModel->getX(), m_loader_brain->getVertCpys());


	// init tumor surface mesh
	m_mesh_tumor = new DfSurfaceMesh(m_loader_tumor->getNumVertices(), m_loader_tumor->getNumFaces(), "tumor");
	m_mesh_tumor->initSurfaceMesh(m_loader_tumor->getVertices(), m_loader_tumor->getFaces(), m_loader_tumor->getUVs(), m_mtlPath_tumor);
	dynamic_cast<DfSurfaceMesh*>(m_mesh_tumor)->setVertCpys(m_loader_tumor->getTetVertNum(), m_deformationModel->getX() + m_loader_brain->getTetVertNum() * 3, m_loader_tumor->getVertCpys());


	// init connect surface mesh
	m_mesh_connect = new SurfaceMesh(numVertex_connect, numFace_connect, "connect");
	m_mesh_connect->initSurfaceMesh(mVertices_connect, mFace_connect, nullptr, "");


	// collision
	m_collision = new MyCollision(this);

	//m_objName = _path.getNameWithoutExtension();
	initialized = true;
	return;
}


SftBrainTumor::~SftBrainTumor()
{
	if (m_deformationModel) delete m_deformationModel;
	if (m_collision) delete m_collision;
	if (m_mesh) delete m_mesh;
	if (m_mesh_brain) delete m_mesh_brain;
	if (m_mesh_tumor) delete m_mesh_tumor;
	if (m_mesh_connect) delete m_mesh_connect;
	if (m_loader_brain) delete m_loader_brain;
	if (m_loader_tumor) delete m_loader_tumor;
}


bool SftBrainTumor::createRenderableObject(RenderableObject* rdFactory, std::string objName)
{
	m_rdFactory = rdFactory;

	if (m_rdFactory == nullptr) {
		Logger::getMainLogger().log(Logger::Level::Error, "The *rdFactory is NULL.", "SoftObjectGPU::createRenderableObject");
		return false;
	}

	bool rlt1 = m_mesh_brain->createRenderableObject(rdFactory, "brain");
	bool rlt2 = m_mesh_tumor->createRenderableObject(rdFactory, "tumor");
	//bool rlt3 = m_mesh_connect->createRenderableObject(rdFactory, "connect");
	return rlt1 && rlt2;
}


void SftBrainTumor::timeStep(float time)
{
	// δ��ʼ��
	if (initialized == false) return;

	m_deformationModel->Reset_More_Fixed(more_fixed, dir);
	m_deformationModel->timeStep(time);
}


void SftBrainTumor::collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder)
{
	// δ��ʼ��
	if (initialized == false) return;


	// ��������ײ���
	if (!isPerformCollisionDetection) return;


	// ִ����ײ���
	this->m_collision->computeCollision(obj_other->getCollisionObject(), recorder);
}


void SftBrainTumor::post2Render()
{
	m_mesh_brain->rendering(m_rdFactory);
	m_mesh_tumor->rendering(m_rdFactory);
	//m_mesh_connect->rendering(m_rdFactory);
}

