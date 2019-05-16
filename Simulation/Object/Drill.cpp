// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Instrument is 

#include"Drill.h"

#include"OBJLoader/ObjLoader.h"
#include"Logger/include/Logger.h"

#include"Collision\MyCollision.h"

#include"Object\SoftObjectGPU.h"
#include"Object/SurfaceMesh.h"

#include"PhantomDevice.h"

#include"FilamentWinApp/RenderableObject.h"


Drill::Drill(char* fileName) : RigidObject(fileName)
{
	if (initialized == false)
	{
		// 在这里，RigidObject 初始化错误，是不是也没必要再继续增加 phantom 了
		// 至少，在最后，intialized = true 是错误的
		return;
	}

	// Phantom Device
	m_phDevice = new PhantomDevice();
	if (m_phDevice->checkDeviceOK() == false)
	{
		Logger::getMainLogger().log(Logger::Error, "Phantom Device initialize failed.", "Instrument::Instrument");
		
		initialized = false;
		return;
	}

	// collision
	m_collision = new MyCollision(this);

	// 初始化成功
	initialized = true;
}

Drill::~Drill()
{
	if (m_phDevice) delete m_phDevice;

	if (m_collision) delete m_collision;
}


bool Drill::createRenderableObject(RenderableObject* rdFactory, std::string objName)
{
	bool rlt = RigidObject::createRenderableObject(rdFactory, objName);

	return rlt;
}

void Drill::timeStep(float time)
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
}