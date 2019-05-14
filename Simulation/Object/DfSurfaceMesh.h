#ifndef DF_SURFACE_MESH_H
#define DF_SURFACE_MESH_H

#include "Object/SurfaceMesh.h"


class DfSurfaceMesh : public SurfaceMesh
{
public:
	DfSurfaceMesh(int numV, int numF, std::string meshName);
	~DfSurfaceMesh();

	virtual void rendering(RenderableObject* rdFactory);
	void setVertCpys(int srcVtNum, float* srcVtPtr, std::vector<uint16_t> &cpy);

private:
	int m_srcVertNum = 0;
	float* m_srcVertPtr = nullptr;
	std::vector<uint16_t> m_VertCpy;
};
#endif // !DF_SURFACE_MESH_H



