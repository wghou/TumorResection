#include"Object/SftBrain.h"

#include"DeformationModelGPU.h"
#include"Constraint/SphereConstraint.h"
#include"ObjLoader/ElementLoader.h"

SftBrain::SftBrain(std::string filePath) : SoftObjectGPU(filePath)
{
	m_cst = new SphereConstraint(this);

	// 初始化 SphereConstraint
	m_cst->center[0] = -40; m_cst->center[1] = 0; m_cst->center[2] = 0;

	m_cst->radius = 40;

	m_cst->ratio = 1;

	m_cst->cstVertices.assign(m_loader->getCstVertices().begin(), m_loader->getCstVertices().end());
}


SftBrain::~SftBrain()
{
	if (m_cst) delete m_cst;
}


void SftBrain::timeStep(float time)
{
	// 未初始化
	if (initialized == false) return;

	// process constraint
	m_cst->processBeforeTimeStep();

	// time step
	SoftObjectGPU::timeStep(time);

	// process constraint
	m_cst->processAfterTimeStep();
}