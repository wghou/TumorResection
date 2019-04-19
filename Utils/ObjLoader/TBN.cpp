#include "TBN.h"

#include <math/norm.h>
#include <math/vec3.h>
#include <math/quat.h>
#include <math/mat3.h>
#include <math/TVecHelpers.h>

#include"Vertex.h"

using namespace filament::math;
using namespace filament::math::details;

bool TBN::updateTBNs(vector<float> &vn, vector<float> &tbn)
{
	

	tbn.clear();

	int numVert = vn.size() / 3;
	// calculate the tangents
	for (int i = 0; i < numVert; i++)
	{
		float3 normal = float3{ vn[i * 3 + 0], vn[i * 3 + 1], vn[i * 3 + 2] };
		float3 tangent;
		float3 bitangent;

		// calculate tangent
		bitangent = normalize(cross(normal, float3{ 1.0, 0.0, 0.0 }));
		tangent = normalize(cross(normal, bitangent));

		// calculate the quaternion
		//quatf q = filament::math::details::TMat33<float>::packTangentFrame({ tangent, bitangent, normal });
		//short4 qs4 = packSnorm16(q.xyzw);

		short4 qs4 = filament::math::packSnorm16(mat3f::packTangentFrame(mat3f{ tangent,bitangent, normal }).xyzw);

		// store
		tbn.push_back(qs4.x);
		tbn.push_back(qs4.y);
		tbn.push_back(qs4.z);
		tbn.push_back(qs4.w);
	}
	return true;
}

template <class T> __forceinline
T normal(const T *p0, const T *p1, const T *p2, T *normal, bool normalized = true)
{
	T e0[3], e1[3];
	for (int i = 0; i < 3; i++)
	{
		e0[i] = p1[i] - p0[i];
		e1[i] = p2[i] - p0[i];
	}
	CROSS(e0, e1, normal);
	if (normalized)	return Normalize(normal);
	return	0;
}

bool TBN::buildVns(int numFaces, uint16_t* faces, int numVertices, float* vertices, float* vn)
{
	vector<Normal> TNs;

	for (int i = 0; i < numFaces; i++)
	{
		float3 p0 = { vertices[faces[i * 3 + 0] * 3 + 0],vertices[faces[i * 3 + 0] * 3 + 1],vertices[faces[i * 3 + 0] * 3 + 2] };
		float3 p1 = { vertices[faces[i * 3 + 1] * 3 + 0],vertices[faces[i * 3 + 1] * 3 + 1],vertices[faces[i * 3 + 1] * 3 + 2] };
		float3 p2 = { vertices[faces[i * 3 + 2] * 3 + 0],vertices[faces[i * 3 + 2] * 3 + 1],vertices[faces[i * 3 + 2] * 3 + 2] };

		float3 e0 = p1 - p0;
		float3 e1 = p2 - p0;

		float3 normal = normalize(cross(e0, e1));
		TNs.push_back({ normal.x, normal.y, normal.z });
	}

	memset(vn, 0, numVertices * 3 * sizeof(float));
	for (int i = 0; i < numFaces; i++)
	{
		int v0 = faces[i * 3 + 0];
		int v1 = faces[i * 3 + 1];
		int v2 = faces[i * 3 + 2];

		vn[v0 * 3 + 0] += TNs[i].u;
		vn[v0 * 3 + 1] += TNs[i].v;
		vn[v0 * 3 + 2] += TNs[i].w;

		vn[v1 * 3 + 0] += TNs[i].u;
		vn[v1 * 3 + 1] += TNs[i].v;
		vn[v1 * 3 + 2] += TNs[i].w;

		vn[v2 * 3 + 0] += TNs[i].u;
		vn[v2 * 3 + 1] += TNs[i].v;
		vn[v2 * 3 + 2] += TNs[i].w;
	}

	float length2, inv_length;
	for (int i = 0; i < numVertices; i++)
	{
		if (length(float3(vn[i * 3 + 0], vn[i * 3 + 1], vn[i * 3 + 2])) < 1e-16) continue;

		float3 normal = normalize( float3(vn[i * 3 + 0],vn[i * 3 + 1], vn[i * 3 + 2]));
		vn[i * 3 + 0] = normal.x;
		vn[i * 3 + 1] = normal.y;
		vn[i * 3 + 2] = normal.z;
	}

	return true;
}
