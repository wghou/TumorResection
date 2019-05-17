#ifndef SFT_BRAIN_H
#define SFT_BRAIN_H

#include"Object/SoftObjectGPU.h"

class SphereConstraint;

class SftBrain : public SoftObjectGPU
{
public:
	SftBrain(std::string filePath = "../assets/models/pulse/brain/");
	~SftBrain();

	virtual void timeStep(float time);

private:
	SphereConstraint* m_cst;
};

#endif // !SFT_BRAIN_H

