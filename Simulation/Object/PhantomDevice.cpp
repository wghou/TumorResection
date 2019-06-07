#include"PhantomDevice.h"

#include"Logger/include/Logger.h"

PhantomDevice::DeviceHandlerSPtr PhantomDevice::s_handler = nullptr;
chai3d::cGenericHapticDevicePtr PhantomDevice::hapticDevice[MAX_DEVICES];

int PhantomDevice::unusedDeviceIndex = 0;

PhantomDevice::PhantomDevice()
{
	// Phantom Device
	if (s_handler == nullptr)
	{
		// 未初始化设备，开始初始化
		s_handler = DeviceHandlerSPtr(new chai3d::cHapticDeviceHandler());

		// 查询设备个数
		int numHapticDevices = s_handler->getNumDevices();
		if (numHapticDevices == 0)
		{
			Logger::getMainLogger().log(Logger::Warning, "Cannot open phantom device.", "PhantomDevice::PhantomDevice");
			//std::cout << "Cannot open phantom device." << std::endl;
		}

		// 初始化所有的设备
		for (int i = 0; i < MAX_DEVICES; i++)
		{
			if (i < numHapticDevices)
			{
				s_handler->getDevice(hapticDevice[i], i);
				
				if (hapticDevice[i]->open() == false)
				{
					Logger::getMainLogger().log(Logger::Warning, "Open Device Failed.", "PhantomDevice::PhantomDevice");
				}

				if (hapticDevice[i]->calibrate() == false)
				{
					Logger::getMainLogger().log(Logger::Warning, "Calibrate device failed.", "PhantomDevice::PhantomDevice");
				}

				

				hapticDevice[i]->setEnableGripperUserSwitch(true);
			}
			else
			{
				hapticDevice[i] = nullptr;
			}
		}
	}
		

	int num = s_handler->getNumDevices();
	if (unusedDeviceIndex >= num)
	{
		Logger::getMainLogger().log(Logger::Error, "Did not have enough phantom device.", "PhantomDevice::PhantomDevice");
		deviceOK = false;
		return;
	}

	m_phantomDeviceIndex = unusedDeviceIndex;
	unusedDeviceIndex++;

	deviceOK = true;
}

PhantomDevice::~PhantomDevice()
{
	if (m_phantomDeviceIndex == -1 || hapticDevice[m_phantomDeviceIndex] == nullptr) return;

	hapticDevice[m_phantomDeviceIndex]->close();
}

bool PhantomDevice::updateDeviceStatus()
{
	// 设备未初始化成功
	if (deviceOK == false) return false;

	if (m_phantomDeviceIndex == -1 || hapticDevice[m_phantomDeviceIndex] == nullptr) return false;

	// 手柄的位置
	hapticDevice[m_phantomDeviceIndex]->getPosition(m_phantomPosition);

	// 手柄的转角
	hapticDevice[m_phantomDeviceIndex]->getRotation(m_phantomRotation);

	// 按键
	bool sw_0, sw_1;
	hapticDevice[m_phantomDeviceIndex]->getUserSwitch(0, sw_0);
	hapticDevice[m_phantomDeviceIndex]->getUserSwitch(1, sw_1);

	// 按键 按下 / 松开 事件
	userSwitchPressed_0 = false;
	userSwitchUnpressed_0 = false;
	if (userSwitch_0 == true && sw_0 == false)
	{
		userSwitchUnpressed_0 = true;
	}
	else if (userSwitch_0 == false && sw_0 == true)
	{
		userSwitchPressed_0 = true;
	}

	userSwitchPressed_1 = false;
	userSwitchUnpressed_1 = false;
	if (userSwitch_1 == true && sw_1 == false)
	{
		userSwitchUnpressed_1 = true;
	}
	else if (userSwitch_1 == false && sw_1 == true)
	{
		userSwitchPressed_1 = true;
	}

	// 更新按键状态
	userSwitch_0 = sw_0;
	userSwitch_1 = sw_1;

	return true;
}

Vector3f PhantomDevice::getPhantomPostion()
{
	return Vector3f(m_phantomPosition.x(), m_phantomPosition.y(), m_phantomPosition.z());
}

//Matrix3D getPhantomRotation();

// wghou 20181108
// debug
MyMatrix4f PhantomDevice::getPhantomTransform()
{
	return MyMatrix4f(
		m_phantomRotation(0, 0), m_phantomRotation(1, 0), m_phantomRotation(2, 0), 0.0,
		m_phantomRotation(0, 1), m_phantomRotation(1, 1), m_phantomRotation(2, 1), 0.0,
		m_phantomRotation(0, 2), m_phantomRotation(1, 2), m_phantomRotation(2, 2), 0.0,
		0.0, 0.0, 0.0, 1.0);
}