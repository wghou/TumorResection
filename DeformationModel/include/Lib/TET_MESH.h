#ifndef	__WHMIN_TET_H__
#define __WHMIN_TET_H__

#include <iostream>
#include <fstream>
#include <stdio.h>
#include "IO_FUNC.h"
#include "MY_MATH.h"
#include "INTERSECTION.h"
#include "MESH.h"
#include "DISTANCE.h"

template <class TYPE>
class TET_MESH
{
public:
	int		max_number;

	//Vertex
	int		number;
	// wghou
	int tetVertNum;
	TYPE*	X;
	TYPE*	M;

	uint16_t*	Tet;
	int		tet_number;
	TYPE*	Dm;
	TYPE*	inv_Dm;
	TYPE*	Vol;

	TYPE density = 50000;

	TET_MESH(): number(0)
	{
		max_number	= 500000;
		X			= new TYPE	[max_number*3];
		M			= new TYPE	[max_number  ];
		Tet			= new uint16_t[max_number*4];
		Dm			= new TYPE	[max_number*9];
		inv_Dm		= new TYPE	[max_number*9];
		Vol			= new TYPE	[max_number  ];
	}
	
	~TET_MESH()
	{
		if(X)		delete[] X;
		if(M)		delete[] M;
		if(Tet)		delete[] Tet;
		if(Dm)		delete[] Dm;
		if(inv_Dm)	delete[] inv_Dm;
		if(Vol)		delete[] Vol;
	}

	void Initialize()
	{
		memset(M, 0, sizeof(TYPE)*number);

		for(int t=0; t<tet_number; t++)
		{
			int p0=Tet[t*4+0]*3;
			int p1=Tet[t*4+1]*3;
			int p2=Tet[t*4+2]*3;
			int p3=Tet[t*4+3]*3;

			Dm[t*9+0]=X[p1+0]-X[p0+0];
			Dm[t*9+3]=X[p1+1]-X[p0+1];
			Dm[t*9+6]=X[p1+2]-X[p0+2];
			Dm[t*9+1]=X[p2+0]-X[p0+0];
			Dm[t*9+4]=X[p2+1]-X[p0+1];
			Dm[t*9+7]=X[p2+2]-X[p0+2];
			Dm[t*9+2]=X[p3+0]-X[p0+0];
			Dm[t*9+5]=X[p3+1]-X[p0+1];
			Dm[t*9+8]=X[p3+2]-X[p0+2];

			Vol[t]=fabs(Matrix_Inverse_3(&Dm[t*9], &inv_Dm[t*9]))/6.0;

			M[p0/3]+=Vol[t]*density;
			M[p1/3]+=Vol[t]*density;
			M[p2/3]+=Vol[t]*density;
			M[p3/3]+=Vol[t]*density;
		}
	}
};


#endif