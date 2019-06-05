#ifndef CNT_SURFACE_MESH_H
#define CNT_SURFACE_MESH_H

#include"Object/SurfaceMesh.h"
#include<map>

class CntSurfaceMesh : public SurfaceMesh
{
public:
	CntSurfaceMesh(int numV, int numF, std::string meshName);
	~CntSurfaceMesh();

	void initSurfaceMesh(float* mVt, int numTet, int tetIdxOffset, uint16_t* mTet);
	virtual void rendering(RenderableObject* rdFactory);
	void updateTetIndex(int srcVtNum, float*srcVtPtr);
	void setSrcVertPtr(int srcVtNum, float*srcVtPtr) { m_srcVertNum = srcVtNum; m_srcVertPtr = srcVtPtr; }
	std::map<uint16_t, uint16_t> &getIndexCpyMap() { return m_VertCpy; }
	
private:
	int m_srcVertNum = 0;
	float* m_srcVertPtr = nullptr;
	std::map<uint16_t, uint16_t> m_VertCpy;
	uint16_t* m_Tets = nullptr;
	int m_numTets = 0;
	int m_TetIdxOffset = 0;
};
#endif // !CNT_SURFACE_MESH_H

