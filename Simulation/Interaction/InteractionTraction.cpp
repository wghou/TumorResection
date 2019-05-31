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
// ���޸�
// ��ʱ�򣬰��°��������첻������
void InteractionTraction::processBeforeTimeStep()
{
	// �Ƿ�ʼ��ײ���
	if (dynamic_cast<Drill*>(m_objA)->m_phDevice->getUserSwitchPressedEvent_1() == true)
	{
		m_startCollisionDetect = !m_startCollisionDetect;

		if (m_startCollisionDetect) Logger::getMainLogger().log(Logger::Debug, "Start collision detection.");
		else Logger::getMainLogger().log(Logger::Debug, "Stop collision detection.");
	}

	// ��δ��ʼ��ײ��⣬�ʷ���
	if (!m_startCollisionDetect) return;

	// ��ǰ��������״̬
	if (m_isTracking)
	{
		float target[3];
		target[0] = m_objA->getLocalPos().x;
		target[1] = m_objA->getLocalPos().y;
		target[2] = m_objA->getLocalPos().z;

		// ��������
		if (dynamic_cast<Drill*>(m_objA)->m_phDevice->getUserSwitchStatus_0() == false)
		{
			m_isTracking = false;
			dynamic_cast<SoftObjectGPU*>(m_objB)->setMoreFixed(target, -1);
			Logger::getMainLogger().log(Logger::Level::Debug, "Stop tracting.");
		}
		// ִ������
		else
		{
			dynamic_cast<SoftObjectGPU*>(m_objB)->setMoreFixed(target, m_colRecorder->col_X_index_1);
		}

		return;
	}
	// ��ǰ���ڰ�ѹ״̬
	else if (m_isPressing)
	{

		//return;
	}


	// ����ֱ����������ˣ���ִ����ײ��⣬��ִ������
	if (dynamic_cast<Drill*>(m_objA)->m_phDevice->getUserSwitchStatus_0() == true)
	{
		// ȡ����ѹ
		m_isPressing = false;

		// ִ����ײ���
		bool colResult = m_objA->getCollisionObject()->computeCollision(m_objB->getCollisionObject(), m_colRecorder);

		// �����������ײ����ִ������
		if (colResult == true)
		{
			m_isTracking = true;
			Logger::getMainLogger().log(Logger::Debug, "Start process tracking.");
		}
	}
	else
	{
		// ȡ������
		if (m_isTracking == true) Logger::getMainLogger().log(Logger::Debug, "Stop process tracking");

		m_isTracking = false;

		// ִ����ײ���
		bool colResult = m_objA->getCollisionObject()->computeCollision(m_objB->getCollisionObject(), m_colRecorder);

		// �����������ײ����ִ�а�ѹ
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