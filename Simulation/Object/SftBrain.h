#ifndef SFT_BRAIN_H
#define SFT_BRAIN_H

#include"Object/SoftObjectGPU.h"
#include"DeformationModelBase.h"

class BrModel_Config : public DfModel_Config
{
	float center[3];
	float radius;
	float ratio;
	uint16_t cvr[100][100];
	std::vector<uint16_t> cstVertices;
};

class SftBrain : public SoftObjectGPU
{
public:
	SftBrain(std::string filePath = "../assets/models/pulse/brain/");
	~SftBrain();

private:

};

#endif // !SFT_BRAIN_H

