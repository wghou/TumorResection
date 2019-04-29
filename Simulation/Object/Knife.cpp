// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Instrument is 

#include"Knife.h"

#include"OBJLoader.h"
#include"Logger.h"
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


	// �� phantomDevice ��ȡλ��
	m_mesh.m_localPos =
		Vector3f(10 * m_phDevice->getPhantomPostion().x + 0.45,
			10 * m_phDevice->getPhantomPostion().y,
			10 * m_phDevice->getPhantomPostion().z + 0.6);

	m_mesh.m_localOriant = m_phDevice->getPhantomTransform();
}