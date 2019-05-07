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

	// 手柄状态信息 - 按键
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
	// Phantom 相关
	int m_phantomDeviceIndex = -1;
	chai3d::cVector3d m_phantomPosition;
	chai3d::cMatrix3d m_phantomRotation;
	chai3d::cTransform m_transform;


	bool deviceOK = false;			// 设备是否初始化成功
	bool userSwitch_0 = false;			// 按键 0 状态
	bool userSwitch_1 = false;
	bool userSwitchPressed_0 = false;	// 按键 0 按下 _ 事件
	bool userSwitchUnpressed_0 = false;	// 按键 0 松开 _ 事件
	bool userSwitchPressed_1 = false;	// 按键 1 按下 _ 事件
	bool userSwitchUnpressed_1 = false;	// 按键 1 松开 _ 事件

private:
	// 将一次性扫描并初始化所有可用的 phantom device，并将设备指针存放到 hapticDevice[MAX_DEVICES] 中
	// 新建一个 PhantomDevice 实例时，便从中选取一个设备，并记录编号到 m_phantomDeviceIndex
	// unusedDeviceIndex 则记录当前为使用的设备编号（最小值）
	static DeviceHandlerSPtr s_handler;
	static chai3d::cGenericHapticDevicePtr hapticDevice[MAX_DEVICES];
	static int unusedDeviceIndex;
};



#endif // !PHANTOMDEVICE_H
