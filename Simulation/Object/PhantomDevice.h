// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Instrument is 
#ifndef PHANTOMDEVICE_H
#define PHANTOMDEVICE_H

#include"Chai3d/chai3d.h"
#include"Math\MyMath.h"


// maximum number of devices supported by this application
const static int MAX_DEVICES = 16;

class PhantomDevice
{
public:
	typedef std::shared_ptr<chai3d::cHapticDeviceHandler> DeviceHandlerSPtr;

	PhantomDevice();
	~PhantomDevice();

	bool updateDeviceStatus();

	// �ֱ�״̬��Ϣ - ����
	bool checkDeviceOK() { return deviceOK; }
	bool getUserSwitchStatus_0() { return userSwitch_0; }
	bool getUserSwitchStatus_1() { return userSwitch_1; }
	bool getUserSwitchPressedEvent_0() { return userSwitchPressed_0; }
	bool getUserSwitchUnpressedEvent_0() { return userSwitchUnpressed_0; }
	bool getUserSwitchPressedEvent_1() { return userSwitchPressed_1; }
	bool getUserSwitchUnpressedEvent_1() { return userSwitchUnpressed_1; }

	Vector3f getPhantomPostion();
	//Matrix3D getPhantomRotation();
	MyMatrix4f getPhantomTransform();

private:
	// Phantom ���
	int m_phantomDeviceIndex = -1;
	chai3d::cVector3d m_phantomPosition;
	chai3d::cMatrix3d m_phantomRotation;
	chai3d::cTransform m_transform;


	bool deviceOK = false;			// �豸�Ƿ��ʼ���ɹ�
	bool userSwitch_0 = false;			// ���� 0 ״̬
	bool userSwitch_1 = false;
	bool userSwitchPressed_0 = false;	// ���� 0 ���� _ �¼�
	bool userSwitchUnpressed_0 = false;	// ���� 0 �ɿ� _ �¼�
	bool userSwitchPressed_1 = false;	// ���� 1 ���� _ �¼�
	bool userSwitchUnpressed_1 = false;	// ���� 1 �ɿ� _ �¼�

private:
	// ��һ����ɨ�貢��ʼ�����п��õ� phantom device�������豸ָ���ŵ� hapticDevice[MAX_DEVICES] ��
	// �½�һ�� PhantomDevice ʵ��ʱ�������ѡȡһ���豸������¼��ŵ� m_phantomDeviceIndex
	// unusedDeviceIndex ���¼��ǰΪʹ�õ��豸��ţ���Сֵ��
	static DeviceHandlerSPtr s_handler;
	static chai3d::cGenericHapticDevicePtr hapticDevice[MAX_DEVICES];
	static int unusedDeviceIndex;
};



#endif // !PHANTOMDEVICE_H
