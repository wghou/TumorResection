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
	CollisionRecorder *m_colRecorder = nullptr;	// ¼ÇÂ¼Åö×²¼ì²â½á¹û

	bool m_startCollisionDetect = false;	// ¿ªÊ¼Ö´ÐÐÅö×²¼ì²â
};

#endif // !INTERACTION_DISSECT_H

