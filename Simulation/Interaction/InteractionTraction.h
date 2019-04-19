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
	CollisionRecorder *m_colRecorder = nullptr;	// 记录碰撞检测结果
	
	bool m_startCollisionDetect = false;	// 开始执行碰撞检测
	bool m_isTracking = false;				// 处于拉扯状态
	bool m_isPressing = false;				// 处于按压状态
};

#endif // !INTERACTIONTRACTION_H_
