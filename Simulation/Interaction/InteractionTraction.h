#ifndef INTERACTIONTRACTION_H_
#define INTERACTIONTRACTION_H_

#include"InteractioBase.h"

class InteractionTraction;
class Drill;
class SoftObjectGPU;
class CollisionRecorder;


class InteractionTraction : public InteractionBase
{
public:
	InteractionTraction();
	~InteractionTraction();

	void processBeforeTimeStep();
	void processAfterTimeStep();

	void init(Drill *obA, SoftObjectGPU *obB);


private:
	//Instrument* m_objA = nullptr;
	//SoftObjectGPU* m_objB = nullptr;
	CollisionRecorder *m_colRecorder = nullptr;	// ��¼��ײ�����
	
	bool m_startCollisionDetect = false;	// ��ʼִ����ײ���
	bool m_isTracking = false;				// ��������״̬
	bool m_isPressing = false;				// ���ڰ�ѹ״̬
};

#endif // !INTERACTIONTRACTION_H_
