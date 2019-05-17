#ifndef CONSTRAINT_BASE_H
#define CONSTRAINT_BASE_H

class ObjectBase;

class ConstraintBase
{
public:
	virtual ~ConstraintBase() {};
	virtual void processBeforeTimeStep() {};
	virtual void processAfterTimeStep() {};
protected:
	ObjectBase* m_objA;
	ObjectBase* m_objB;
};

#endif // !CONSTRAINT_BASE_H
