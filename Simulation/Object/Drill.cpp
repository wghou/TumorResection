// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Instrument is 

#include"Drill.h"

#include"OBJLoader.h"
#include"Logger.h"

#include"Collision\MyCollision.h"

#include"Object\SoftObjectGPU.h"

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

	// 初始化成功
	initialized = true;
}

Drill::~Drill()
{
	if (m_phDevice) delete m_phDevice;
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

	m_mesh.m_localPos = 
		Vector3f(10 * m_phDevice->getPhantomPostion().x + 0.45, 
		10 * m_phDevice->getPhantomPostion().y, 
		10 * m_phDevice->getPhantomPostion().z + 0.6);
	
	m_mesh.m_localOriant = m_phDevice->getPhantomTransform();

	float pos[3];
	pos[0] = m_mesh.m_localPos.x;
	pos[1] = m_mesh.m_localPos.y;
	pos[2] = m_mesh.m_localPos.z;

	m_rdFactory->updateObjectOriant("Drill", pos, &m_mesh.m_localOriant.m[0][0]);
}