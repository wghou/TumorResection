#include"InteractionTraction.h"

#include"logger\include\Logger.h"

#include"Object\SoftObjectGPU.h"
#include"Object/Object.h"
#include"Object\Drill.h"
#include"Object\PhantomDevice.h"
#include"Object/SurfaceMesh.h"

#include"Collision\MyCollision.h"
#include"Collision\CollisionBasic.h"

InteractionTraction::InteractionTraction()
{
	m_colRecorder = new CollisionRecorder();
}


InteractionTraction::~InteractionTraction()
{
	if (m_colRecorder) delete m_colRecorder;
}


// bug
// 待修复
// 有时候，按下按键后，拉伸不起作用
void InteractionTraction::processBeforeTimeStep()
{
	// 是否开始碰撞检测
	if (dynamic_cast<Drill*>(m_objA)->m_phDevice->getUserSwitchPressedEvent_1() == true)
	{
		m_startCollisionDetect = !m_startCollisionDetect;

		if (m_startCollisionDetect) Logger::getMainLogger().log(Logger::Debug, "Start collision detection.");
		else Logger::getMainLogger().log(Logger::Debug, "Stop collision detection.");
	}

	// 还未开始碰撞检测，故返回
	if (!m_startCollisionDetect) return;

	// 当前处于拉扯状态
	if (m_isTracking)
	{
		float target[3];
		target[0] = m_objA->getLocalPos().x;
		target[1] = m_objA->getLocalPos().y;
		target[2] = m_objA->getLocalPos().z;

		// 结束拉扯
		if (dynamic_cast<Drill*>(m_objA)->m_phDevice->getUserSwitchStatus_0() == false)
		{
			m_isTracking = false;
			dynamic_cast<SoftObjectGPU*>(m_objB)->setMoreFixed(target, -1);
			Logger::getMainLogger().log(Logger::Level::Debug, "Stop tracting.");
		}
		// 执行拉扯
		else
		{
			dynamic_cast<SoftObjectGPU*>(m_objB)->setMoreFixed(target, m_colRecorder->col_X_index_1);
		}

		return;
	}
	// 当前处于按压状态
	else if (m_isPressing)
	{

		//return;
	}


	// 如果手柄按键按下了，则执行碰撞检测，并执行拉扯
	if (dynamic_cast<Drill*>(m_objA)->m_phDevice->getUserSwitchStatus_0() == true)
	{
		// 取消按压
		m_isPressing = false;

		// 执行碰撞检测
		bool colResult = m_objA->getCollisionObject()->computeCollision(m_objB->getCollisionObject(), m_colRecorder);

		// 如果发生了碰撞，则执行拉扯
		if (colResult == true)
		{
			m_isTracking = true;
			Logger::getMainLogger().log(Logger::Debug, "Start process tracking.");
		}
	}
	else
	{
		// 取消拉扯
		if (m_isTracking == true) Logger::getMainLogger().log(Logger::Debug, "Stop process tracking");

		m_isTracking = false;

		// 执行碰撞检测
		bool colResult = m_objA->getCollisionObject()->computeCollision(m_objB->getCollisionObject(), m_colRecorder);

		// 如果发生了碰撞，则执行按压
		if (colResult == true)
		{
			if(m_isPressing == false) Logger::getMainLogger().log(Logger::Debug, "Start process pressing.");

			m_isPressing = true;
		}
		else
		{
			m_isPressing = false;
		}
	}
}


void InteractionTraction::processAfterTimeStep()
{

}


void InteractionTraction::init(Drill *obA, ObjectBase *obB)
{
	m_objA = obA;
	m_objB = obB;
}