#ifndef _STDAFX__H
#define _STDAFX__H

#include <windows.h>
#include <D3D11.h>
#include <d3dCompiler.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "Constants.h"
#include <DirectXMath.h>
using namespace DirectX;

#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = NULL; }
#define SAFE_DELETE(x) if( x ) { delete(x); (x) = NULL; }
#define SAFE_DELETE_ARRAY(x) if( x ) { delete[](x); (x) = NULL; }

struct Vertex
{
	XMFLOAT4 pos;
	XMFLOAT4 normal;
	XMFLOAT2 texC;
};


struct cBuffer
{
	XMFLOAT4 cameraPos;
	XMFLOAT4X4 IP;
	XMFLOAT4X4 IV;
	int nrOfTriangles;
	XMFLOAT3 pad;
};



struct Triangle
{
	XMFLOAT4 pos[3];
	XMFLOAT4 color;
	XMFLOAT4 normal;
	int ID;
	XMFLOAT3 pad;
};

struct TriangleMat
{
	XMFLOAT4	pos0;
	XMFLOAT4	pos1;
	XMFLOAT4	pos2;
	XMFLOAT2	textureCoordinate0;
	XMFLOAT2	textureCoordinate1;
	XMFLOAT2	textureCoordinate2;
	int			ID;
	float		pad;
	XMFLOAT4	color;
	XMFLOAT4	normal;
};

struct cbWorld
{

	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};


struct COLOR
{
	FLOAT d[4];
	COLOR(float r, float g, float b, float a)
	{
		d[0] = r;
		d[1] = g;
		d[2] = b;
		d[3] = a;
	}

	operator const FLOAT* () { return (FLOAT*)this; }
};

struct AABB
{
	XMFLOAT4 minPoint;
	XMFLOAT4 maxPoint;
	int triangleIndex;
};

struct NodeAABB
{
	XMFLOAT4 minPoint;
	XMFLOAT4 maxPoint;
};

struct Node
{
	Node *right = NULL;
	Node *left = NULL;
	NodeAABB aabb;
	std::vector<int> index;
};

struct NodePass1
{
	int index;					//index to start indices
	int nrOfTriangles;			//how many indices to read
	int parent;					//parent id
	int left_right_bool;		//0 == left child node; 1 == right child node
};

struct NodePass2
{
	int index;					//index to start indices
	int nrOfTriangles;			//how many indices to read
	XMFLOAT2 split;				//0 == splitAxis, 1 == splitValue
	NodeAABB aabb;				//AABB collisionbox
};

struct Light
{
	XMFLOAT4 pos;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	float range;
	XMFLOAT3 pad;
};

struct Sphere
{
	XMFLOAT4 pos;
	XMFLOAT4 color;
	float radie;
	XMFLOAT3 pad;
};

struct cLightBuffer
{
	Light lightList[NROFLIGHTS];
};

struct cSphereBuffer
{
	Sphere sphereList[NROFLIGHTS];
};

struct cToggles
{
	int lightSpheres;	//Toggle on debug light spheres
	XMFLOAT3 togglePad;
};

struct depthcBuffer
{
	int depth;
	XMFLOAT3 padDepth;
};

//////////////////////////////////////////////////////////////////////////
// to find memory leaks
//////////////////////////////////////////////////////////////////////////
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define myMalloc(s)       _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define myCalloc(c, s)    _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define myRealloc(p, s)   _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define myExpand(p, s)    _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define myFree(p)         _free_dbg(p, _NORMAL_BLOCK)
#define myMemSize(p)      _msize_dbg(p, _NORMAL_BLOCK)
#define myNew new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define myDelete delete  // Set to dump leaks at the program exit.
//#define myInitMemoryCheck() \
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
//#define myDumpMemoryLeaks() \
//	_CrtDumpMemoryLeaks()
#else
#define myMalloc malloc
#define myCalloc calloc
#define myRealloc realloc
#define myExpand _expand
#define myFree free
#define myMemSize _msize
#define myNew new
#define myDelete delete
#define myInitMemoryCheck()
#define myDumpMemoryLeaks()
#endif 
//////////////////////////////////////////////////////////////////////////



#endif
