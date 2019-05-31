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
	if (createObjectFromFile(filePath) == false) {
		initialized = false;
		return;
	}
}


SftBrainTumor::~SftBrainTumor()
{

}


bool SftBrainTumor::createObjectFromFile(std::string filePath)
{
	// material path
	ElementLoader m_loader_brain;
	std::string m_mtlPath_brain;
	// load brain element
	{
		Path _path(filePath);
		if (!_path.exists()) {
			Logger::getMainLogger().log(Logger::Level::Error, "The file " + _path.getName() + " doesnt exist.", "SftBrainTumor::SftBrainTumor");
			initialized = false;
			return false;
		}

		std::string objFilePath = _path.concat("brain/brain");
		std::string jsonFileName = _path.concat("brain/brain.json");

		// load .ele .node .obj files
		m_loader_brain.loadElement(objFilePath);

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
				m_loader_brain.translate(trans[0], trans[1], trans[2]);
				m_loader_brain.scale(sl);
			}
			catch (std::exception &ex) {
				Logger::getMainLogger().log(Logger::Level::Error, "Error when initial json: " + std::string(ex.what()), "SftBrainTumor::SftBrainTumor");
			}
		}
		else {
			Logger::getMainLogger().log(Logger::Level::Error, "Cannot open config file: " + jsonFileName, "SftBrainTumor::SftBrainTumor");
		}


		if (m_loader_brain.getNumVertices() == 0 || m_loader_brain.getNumTets() == 0 || m_loader_brain.getNumVertices() == 0) {
			Logger::getMainLogger().log(Logger::Level::Error, "There is no mesh in the eleFile.", "SftBrainTumor::SftBrainTumor");
			initialized = false;
			return false;
		}
	}

	// material path
	std::string m_mtlPath_tumor;
	ElementLoader m_loader_tumor;
	// load tumor element
	{
		Path _path(filePath);
		if (!_path.exists()) {
			Logger::getMainLogger().log(Logger::Level::Error, "The file " + _path.getName() + " doesnt exist.", "SftBrainTumor::SftBrainTumor");
			initialized = false;
			return false;
		}

		std::string objFilePath = _path.concat("tumor/tumor");
		std::string jsonFileName = _path.concat("tumor/tumor.json");

		// load .ele .node .obj files
		m_loader_tumor.loadElement(objFilePath);

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
				m_loader_tumor.translate(trans[0], trans[1], trans[2]);
				m_loader_tumor.scale(sl);
			}
			catch (std::exception &ex) {
				Logger::getMainLogger().log(Logger::Level::Error, "Error when initial json: " + std::string(ex.what()), "SftBrainTumor::SftBrainTumor");
			}
		}
		else {
			Logger::getMainLogger().log(Logger::Level::Error, "Cannot open config file: " + jsonFileName, "SftBrainTumor::SftBrainTumor");
		}


		if (m_loader_tumor.getNumVertices() == 0 || m_loader_tumor.getNumTets() == 0 || m_loader_tumor.getNumVertices() == 0) {
			Logger::getMainLogger().log(Logger::Level::Error, "There is no mesh in the eleFile.", "SftBrainTumor::SftBrainTumor");
			initialized = false;
			return false;
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
	mVertices_all = new float[(m_loader_brain.getTetVertNum() + m_loader_tumor.getTetVertNum() + numVertex_connect) * 3];
	mTet_all = new uint16_t[(m_loader_brain.getNumTets() + m_loader_brain.getNumTets() + numTet_connect) * 4];
	memcpy(mVertices_all, m_loader_brain.getVertices(), sizeof(float)*m_loader_brain.getTetVertNum() * 3);
	memcpy(mVertices_all + m_loader_brain.getTetVertNum() * 3, m_loader_tumor.getVertices(), sizeof(float)*m_loader_tumor.getTetVertNum() * 3);
	//memcpy(mVertices_all + m_loader_brain->getTetVertNum() * 3 + m_loader_tumor->getTetVertNum() * 3, 
	//	mVertices_connect, sizeof(float)*numVertex_connect * 3);
	memcpy(mTet_all, m_loader_brain.getTets(), sizeof(float)*m_loader_brain.getNumTets() * 4);
	memcpy(mTet_all + m_loader_brain.getNumTets() * 4, m_loader_tumor.getTets(), sizeof(float)*m_loader_tumor.getNumTets() * 4);
	for (int i = 0; i < m_loader_tumor.getNumTets() * 4; i++) {
		mTet_all[i + m_loader_brain.getNumTets() * 4] += m_loader_brain.getTetVertNum();
	}
	//memcpy(mTet_all + m_loader_brain->getTetVertNum() * 3 + m_loader_tumor->getTetVertNum() * 4, 
	//	mTet_connect, sizeof(float)*numTet_connect * 4);


	// init deformation model
	DfModel_Config config;
	config.numVertex = m_loader_brain.getTetVertNum() + m_loader_tumor.getTetVertNum() + numVertex_connect;
	config.mVertices = mVertices_all;
	config.numTet = m_loader_brain.getNumTets() + m_loader_tumor.getNumTets() + numTet_connect;
	config.mTets = mTet_all;

	// only the brain contain fixed vertices
	config.fixedVertices.assign(m_loader_brain.getFixed().begin(), m_loader_brain.getFixed().end());

	m_deformationModel = new DeformationModelGPU();
	m_deformationModel->Initialize(config);

	delete[] mVertices_all; delete[] mTet_all;


	// init brain surface mesh
	SurfaceMesh* m_mesh_brain = new DfSurfaceMesh(m_loader_brain.getNumVertices(), m_loader_brain.getNumFaces(), "brain");
	m_mesh_brain->initSurfaceMesh(m_loader_brain.getVertices(), m_loader_brain.getFaces(), m_loader_brain.getUVs(), m_mtlPath_brain);
	dynamic_cast<DfSurfaceMesh*>(m_mesh_brain)->setVertCpys(m_loader_brain.getTetVertNum(), 0,
		m_deformationModel->getX(), m_loader_brain.getVertCpys());
	m_mesh.push_back(m_mesh_brain);

	// init tumor surface mesh
	SurfaceMesh* m_mesh_tumor = new DfSurfaceMesh(m_loader_tumor.getNumVertices(), m_loader_tumor.getNumFaces(), "tumor");
	m_mesh_tumor->initSurfaceMesh(m_loader_tumor.getVertices(), m_loader_tumor.getFaces(), m_loader_tumor.getUVs(), m_mtlPath_tumor);
	dynamic_cast<DfSurfaceMesh*>(m_mesh_tumor)->setVertCpys(m_loader_tumor.getTetVertNum(), m_loader_brain.getTetVertNum(), m_deformationModel->getX(), m_loader_tumor.getVertCpys());
	m_mesh.push_back(m_mesh_tumor);

	// init connect surface mesh
	SurfaceMesh* m_mesh_connect = new DfSurfaceMesh(numVertex_connect, numFace_connect, "connect");
	m_mesh_connect->initSurfaceMesh(mVertices_connect, mFace_connect, nullptr, "");
	//dynamic_cast<DfSurfaceMesh*>(m_mesh_tumor)->setVertCpys(config.numVertex, m_loader_brain->getTetVertNum() + m_loader_tumor->getTetVertNum(), 
	//	m_deformationModel->getX(), NULL);
	//m_mesh.push_back(m_mesh_connect);


	// collision
	m_collision = new MyCollision(this);

	//m_objName = _path.getNameWithoutExtension();
	initialized = true;
	return true;
}

