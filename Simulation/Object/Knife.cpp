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

	// ��ʼ���ɹ�
	initialized = true;
}

Knife::~Knife()
{
	if (m_phDevice) delete m_phDevice;
}


void Knife::timeStep(float time)
{
	// δ��ʼ��
	if (initialized == false) return;

	// �����ֱ���Ϣ
	m_phDevice->updateDeviceStatus();

	m_mesh.m_localOriant = m_phDevice->getPhantomTransform();
	// location
	m_mesh.m_localOriant.m[3][0] = 10 * m_phDevice->getPhantomPostion().x + 0.15;
	m_mesh.m_localOriant.m[3][1] = 10 * m_phDevice->getPhantomPostion().y;
	m_mesh.m_localOriant.m[3][2] = 10 * m_phDevice->getPhantomPostion().z + 0.6;

	RigidObject::timeStep(time);
}