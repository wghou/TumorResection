#ifndef MYINTERACTION_H_
#define MYINTERACTION_H_

#include"InteractioBase.h"

class MyInteraction;
class DMForces;
class SoftObjectGPU;

class MyInteraction : public InteractionBase
{
public:
	MyInteraction();
	~MyInteraction();

	void processBeforeTimeStep();
	void processAfterTimeStep();

	void init(SoftObjectGPU *obA, SoftObjectGPU *obB, float distance);

private:
	void imposeInteractionForces(float k, float damp);
	void disconnectSpring(bool st, float mul);


private:
	DMForces* m_dmForces = nullptr;
};


#endif // !MYINTERACTION_H_

