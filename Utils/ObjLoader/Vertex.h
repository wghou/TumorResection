#ifndef VERTEX_H
#define VERTEX_H

#include <stdint.h>

struct FaceVertex
{
	uint16_t index;
	uint16_t v;
	uint16_t vt;
	uint16_t vn;

	bool operator ==(const FaceVertex& fv) {
		return (v == fv.v && vt == fv.vt && vn == fv.vn);
	}

	bool isDuplication(const FaceVertex& fv) {
		return (v == fv.v && (vt != fv.vt || vn != fv.vn));
	}
};

struct Tetrahedron 
{
	uint16_t v0, v1, v2, v3;
};

struct Vertex
{
	float x, y, z;
};

struct Normal
{
	float u, v, w;
};

struct TxtCord
{
	float x, y;
};
#endif // !VERTEX_H

