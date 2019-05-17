#ifndef SPHERE_CONSTRAINT_H
#define SPHERE_CONSTRAINT_H

#include<stdint.h>
#include<vector>

#include"Constraint/ConstraintBase.h"

class SftBrain;

class SphereConstraint : public ConstraintBase
{
	friend class SftBrain;
public:
	SphereConstraint(SftBrain* objA);
	virtual ~SphereConstraint();


	virtual void processBeforeTimeStep();
	virtual void processAfterTimeStep();

protected:
	float center[3];
	float radius;
	float ratio;
	uint16_t cvr[100][100];
	std::vector<uint16_t> cstVertices;
};
#endif // !SPHERE_CONSTRAINT_H

