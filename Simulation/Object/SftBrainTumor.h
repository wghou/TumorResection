#ifndef SFT_BRAIN_TUMOR_H
#define SFT_BRAIN_TUMOR_H

#include"Object/SoftObjectGPU.h"

class MyCollision;
class ElementLoader;
class DeformationModelGPU;

class SftBrainTumor : public SoftObjectGPU
{
	friend MyCollision;

public:
	SftBrainTumor(std::string filePath);
	~SftBrainTumor();

	// 仿真步骤相关
protected:
	virtual bool createObjectFromFile(std::string filePath);
};

#endif // !SFT_BRAIN_TUMOR_H

