// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Instrument is 

#include"Knife.h"

#include"OBJLoader/ObjLoader.h"
#include"Logger/include/Logger.h"
#include"Collision\MyCollision.h"
#include"PhantomDevice.h"
#include"Object/SurfaceMesh.h"


Knife::Knife(std::string fileName)
{
	if (RigidObject::createObjectFromFile(fileName) == false) {
		initialized = false;
		return;
	}

	// Phantom Device
	m_phDevice = new PhantomDevice();
	if (m_phDevice->checkDeviceOK() == false)
	{
		Logger::getMainLogger().log(Logger::Error, "Phantom Device initialize failed.", "Knife::Knife");

		initialized = false;
		return;
	}

	// collision
	m_collision = new MyCollision(this);

	// 初始化成功
	initialized = true;
}

Knife::~Knife()
{
	if (m_phDevice) delete m_phDevice;

	if (m_collision) delete m_collision;
}

bool Knife::createRenderableObject(RenderableObject* rdFactory, std::string objName)
{
	bool rlt = RigidObject::createRenderableObject(rdFactory);

	return rlt;
}


void Knife::timeStep(float time)
{
	// 未初始化
	if (initialized == false) return;

	// 更新手柄信息
	m_phDevice->updateDeviceStatus();

	MyMatrix4f ori = m_phDevice->getPhantomTransform();
	// location
	ori.m[3][0] = 15 * m_phDevice->getPhantomPostion().x + 0.15;
	ori.m[3][1] = 15 * m_phDevice->getPhantomPostion().y;
	ori.m[3][2] = 15 * m_phDevice->getPhantomPostion().z + 0.6;

	m_localOriant = ori;

	for each(auto mesh in m_mesh) {
		if (!mesh) continue;

		mesh->setOriant(ori);
	}
}