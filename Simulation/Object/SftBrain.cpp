#include"Object/SftBrain.h"

#include"DeformationModelGPU.h"
#include"Collision\MyCollision.h"
#include"Object/DfSurfaceMesh.h"

SftBrain::SftBrain(std::string filePath)
{

}


SftBrain::~SftBrain()
{

}


void SftBrain::timeStep(float time)
{
	// Î´³õÊ¼»¯
	if (initialized == false) return;

	float gravity = 0.5*sin(time*0.9)*sin(time*0.9) + 0.05;
	m_deformationModel->setGravity(gravity);

	SoftObjectGPU::timeStep(time);
}