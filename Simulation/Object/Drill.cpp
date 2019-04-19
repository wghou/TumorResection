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


Drill::Drill(char* fileName) : RigidObject(fileName)
{
	if (initialized == false)
	{
		// �����RigidObject ��ʼ�������ǲ���Ҳû��Ҫ�ټ������� phantom ��
		// ���٣������intialized = true �Ǵ����
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

	// ��ʼ���ɹ�
	initialized = true;
}

Drill::~Drill()
{
	if (m_phDevice) delete m_phDevice;
}


void Drill::timeStep(float time)
{
	// δ��ʼ��
	if (initialized == false) return;

	// �����ֱ���Ϣ
	m_phDevice->updateDeviceStatus();

	m_mesh.m_localPos = 
		Vector3d(10 * m_phDevice->getPhantomPostion().x + 0.45, 
		10 * m_phDevice->getPhantomPostion().y, 
		10 * m_phDevice->getPhantomPostion().z + 0.6);
	
	m_mesh.m_localOriant = m_phDevice->getPhantomTransform();
}