#ifndef INTERACTIONBASE_H_
#define INTERACTIONBASE_H_

class InteractionBase;
class ObjectBase;

class InteractionBase
{
public:
	virtual ~InteractionBase() {};
	virtual void processBeforeTimeStep() {};
	virtual void processAfterTimeStep() {};

protected:
	ObjectBase* m_objA;
	ObjectBase* m_objB;
};

#endif // !INTERACTIONBASE_H_

