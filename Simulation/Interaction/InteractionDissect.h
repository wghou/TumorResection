#ifndef INTERACTION_DISSECT_H
#define INTERACTION_DISSECT_H

#include"InteractioBase.h"

class InteractionDissect;
class Knife;
class CollisionRecorder;

class InteractionDissect : public InteractionBase
{
public:
	InteractionDissect();
	~InteractionDissect();

	void processBeforeTimeStep();
	void processAfterTimeStep();

	void init(Knife *obA, ObjectBase *obB);

private:
	CollisionRecorder *m_colRecorder = nullptr;	// ��¼��ײ�����

	bool m_startCollisionDetect = false;	// ��ʼִ����ײ���
};

#endif // !INTERACTION_DISSECT_H

