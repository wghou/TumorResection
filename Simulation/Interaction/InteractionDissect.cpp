#include"InteractionDissect.h"

#include"logger\include\Logger.h"

#include"Collision\MyCollision.h"
#include"Collision\CollisionBasic.h"

#include"Object/Object.h"
#include"Object\PhantomDevice.h"
#include"Object/CntSurfaceMesh.h"
#include"DeformationModelGPU.h"

InteractionDissect::InteractionDissect()
{
	m_colRecorder = new CollisionRecorder();
}


InteractionDissect::~InteractionDissect()
{
	if (m_colRecorder) delete m_colRecorder;
}


void InteractionDissect::processBeforeTimeStep()
{
	// �Ƿ�ʼ��ײ���
	if (dynamic_cast<Knife*>(m_objA)->m_phDevice->getUserSwitchPressedEvent_1() == true)
	{
		m_startCollisionDetect = !m_startCollisionDetect;

		if (m_startCollisionDetect) Logger::getMainLogger().log(Logger::Debug, "Start collision detection.");
		else Logger::getMainLogger().log(Logger::Debug, "Stop collision detection.");
	}

	// ��δ��ʼ��ײ��⣬�ʷ���
	if (!m_startCollisionDetect) return;



	// ����ֱ����������ˣ���ִ����ײ��⣬��ִ������
	if (dynamic_cast<Knife*>(m_objA)->m_phDevice->getUserSwitchStatus_0() == true)
	{

		// ִ����ײ���
		bool colResult = m_objA->getCollisionObject()->computeCollision(m_objB->getCollisionObject(), m_colRecorder);

		// �����������ײ����ִ������
		if (colResult == true)
		{
			Logger::getMainLogger().log(Logger::Debug, "Start process removing tet.");

			SftBrainTumor* bt = dynamic_cast<SftBrainTumor*>(m_objB);
			std::vector<SurfaceMesh*>& m_mesh = bt->getMesh();

			for each(auto mesh in m_mesh) {
				if (typeid(*mesh) == typeid(CntSurfaceMesh)) {
					int tetIdxOffset = dynamic_cast<CntSurfaceMesh*>(mesh)->getTetIdxOffset();

					dynamic_cast<CntSurfaceMesh*>(mesh)->getRmTets()[m_colRecorder->col_X_index_1] = 1;
					bt->getDeformationModel()->removeTet(tetIdxOffset + m_colRecorder->col_X_index_1);
					dynamic_cast<CntSurfaceMesh*>(mesh)->updateSurfaceMesh();
				}
			}

			
		}
	}
}


void InteractionDissect::processAfterTimeStep()
{

}

void InteractionDissect::init(Knife *obA, ObjectBase *obB) 
{
	m_objA = obA;
	m_objB = obB;
}