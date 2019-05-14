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


Knife::Knife(char* fileName) : RigidObject(fileName)
{
	// Phantom Device
	m_phDevice = new PhantomDevice();
	if (m_phDevice->checkDeviceOK() == false)
	{
		Logger::getMainLogger().log(Logger::Error, "Phantom Device initialize failed.", "Instrument::Instrument");

		initialized = false;
		return;
	}

	// 初始化成功
	initialized = true;
}

Knife::~Knife()
{
	if (m_phDevice) delete m_phDevice;
}

bool Knife::createRenderableObject(RenderableObject* rdFactory, std::string objName)
{
	bool rlt = RigidObject::createRenderableObject(rdFactory, objName);

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
	ori.m[3][0] = 10 * m_phDevice->getPhantomPostion().x + 0.15;
	ori.m[3][1] = 10 * m_phDevice->getPhantomPostion().y;
	ori.m[3][2] = 10 * m_phDevice->getPhantomPostion().z + 0.6;

	m_mesh->setOriant(ori);

	RigidObject::timeStep(time);
}