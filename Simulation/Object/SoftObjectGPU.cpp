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
#include"Object/DfSurfaceMesh.h"

#include"FilamentWinApp/RenderableObject.h"
#include"utils/Path.h"
#include"ObjLoader/ElementLoader.h"
#include"ObjLoader/TBN.h"


SoftObjectGPU::SoftObjectGPU()
{

}

SoftObjectGPU::~SoftObjectGPU()
{
	if (m_deformationModel) delete m_deformationModel;
	if (m_collision) delete m_collision;

	for each(auto mesh in m_mesh) {
		if (mesh) delete mesh;
	}

	if (m_loader) delete m_loader;
}

bool SoftObjectGPU::createObjectFromFile(std::string filePath)
{
	m_loader = new ElementLoader();
	Path _path(filePath);
	if (!_path.exists()) {
		Logger::getMainLogger().log(Logger::Level::Error, "The file " + _path.getName() + " doesnt exist.", "RigidObject::RigidObject");
		initialized = false;
		return;
	}

	std::string objFilePath = _path.concat(_path.getName());
	std::string jsonFileName = _path.concat(_path.getName() + ".json");

	// load .ele .node .obj files
	m_loader->loadElement(objFilePath);

	// material path
	std::string m_mtlPath;

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
			m_loader->translate(trans[0], trans[1], trans[2]);
			m_loader->scale(sl);
		}
		catch (std::exception &ex) {
			Logger::getMainLogger().log(Logger::Level::Error, "Error when initial json: " + std::string(ex.what()), "RigidObject::RigidObject");
		}
	}
	else {
		Logger::getMainLogger().log(Logger::Level::Error, "Cannot open config file: " + jsonFileName, "RigidObject::RigidObject");
	}


	if (m_loader->getNumVertices() == 0 || m_loader->getNumTets() == 0 || m_loader->getNumVertices() == 0) {
		Logger::getMainLogger().log(Logger::Level::Error, "There is no mesh in the eleFile.", "SoftObjectGPU::SoftObjectGPU");
		initialized = false;
		return;
	}


	// init deformation model
	DfModel_Config config;
	config.numVertex = m_loader->getTetVertNum();
	config.mVertices = m_loader->getVertices();
	config.numTet = m_loader->getNumTets();
	config.mTets = m_loader->getTets();

	config.fixedVertices.assign(m_loader->getFixed().begin(), m_loader->getFixed().end());

	m_deformationModel = new DeformationModelGPU();
	m_deformationModel->Initialize(config);


	// init surface mesh
	SurfaceMesh* mesh = new DfSurfaceMesh(m_loader->getNumVertices(), m_loader->getNumFaces(), m_objName);
	mesh->initSurfaceMesh(m_loader->getVertices(), m_loader->getFaces(), m_loader->getUVs(), m_mtlPath);
	dynamic_cast<DfSurfaceMesh*>(mesh)->setVertCpys(m_loader->getTetVertNum(), 0, m_deformationModel->getX(), m_loader->getVertCpys());
	m_mesh.push_back(mesh);

	// collision
	m_collision = new MyCollision(this);

	m_objName = _path.getNameWithoutExtension();
	initialized = true;
}


bool SoftObjectGPU::createRenderableObject(RenderableObject* rdFactory, std::string objName)
{
	m_rdFactory = rdFactory;

	if (m_rdFactory == nullptr) {
		Logger::getMainLogger().log(Logger::Level::Error, "The *rdFactory is NULL.", "SoftObjectGPU::createRenderableObject");
		return false;
	}

	bool rlt = true;
	for each(auto mesh in m_mesh) {
		if (!mesh) continue;

		if (mesh->createRenderableObject(rdFactory) == false) rlt = false;
	}
	return rlt;
}


void SoftObjectGPU::timeStep(float time)
{
	// Î´³õÊ¼»¯
	if (initialized == false) return;

	m_deformationModel->Reset_More_Fixed(more_fixed, dir);
	m_deformationModel->timeStep(time);
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


void SoftObjectGPU::post2Render()
{
	for each(auto mesh in m_mesh) {
		if (!mesh) continue;

		mesh->rendering(m_rdFactory);
	}
}