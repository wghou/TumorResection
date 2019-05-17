#include"Object/SftBrain.h"

#include"DeformationModelGPU.h"
#include"Constraint/SphereConstraint.h"

SftBrain::SftBrain(std::string filePath) : SoftObjectGPU(filePath)
{
	m_cst = new SphereConstraint(this);
}


SftBrain::~SftBrain()
{
	if (m_cst) delete m_cst;
}


void SftBrain::timeStep(float time)
{
	// Î´³õÊ¼»¯
	if (initialized == false) return;

	// process constraint
	m_cst->processBeforeTimeStep();

	// time step
	SoftObjectGPU::timeStep(time);

	// process constraint
	m_cst->processAfterTimeStep();
}