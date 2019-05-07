// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// SoftObjectGPU is 

#include"SoftObjectGPU.h"

#include"Logger/include/Logger.h"
#include"json/json_helper.h"
#include <iostream>
#include <fstream>

#include"DeformationModelGPU.h"
#include"Collision\MyCollision.h"

#include"FilamentWinApp/RenderableObject.h"
#include"utils/Path.h"
#include"ObjLoader/ElementLoader.h"
#include"ObjLoader/TBN.h"


SoftObjectGPU::SoftObjectGPU(char* filePath)
{
	ElementLoader m_loader;

	Path _path(filePath);
	if (!_path.exists()) {
		Logger::getMainLogger().log(Logger::Level::Error, "The file " + _path.getName() + " doesnt exist.", "RigidObject::RigidObject");
		initialized = false;
		return;
	}

	std::string objFilePath = _path.concat(_path.getName());
	std::string jsonFileName = _path.concat(_path.getName() + ".json");

	// load .ele .node .obj files
	m_loader.loadElement(objFilePath);

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
	
	m_objName = _path.getNameWithoutExtension();
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

	m_rdFactory->updateVertexBuffer(m_objName);
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