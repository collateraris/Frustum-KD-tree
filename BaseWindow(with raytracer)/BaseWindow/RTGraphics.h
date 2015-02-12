#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"
#include "ComputeHelp.h"
#include <locale>
#include <codecvt>
#include <string>

extern ID3D11Device* g_Device;
extern ID3D11DeviceContext* g_DeviceContext;
extern IDXGISwapChain* g_SwapChain;

extern Camera* Cam;

class RTGraphics
{
public:
	RTGraphics(HWND* _hwnd);
	~RTGraphics();

	void Update(float _dt);
	void Render(float _dt);
	void release();

private:
	//Converter (sting to wstring)
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	void createCBuffers();
	void createTriangleTexture();
	void createNodeBuffer(int _nrOfNodes, Node* _rootNode);
	void createKdTree(Mesh *_mesh);
	void createKDNodeSplit(std::vector<AABB>* _aabbList, Node* _node, int _split);
	void splitListX(Node* _node, std::vector<AABB>* _AABBList);
	void splitListY(Node* _node, std::vector<AABB>* _AABBList);
	void splitListZ(Node* _node, std::vector<AABB>* _AABBList);
	ComputeWrap *computeWrap;

	ComputeShader *raytracer = NULL;

	ComputeBuffer *triangleBuffer = NULL;

	ID3D11Buffer *g_cBuffer;
	cBuffer cb;

	ID3D11UnorderedAccessView *backbuffer;

	//MESH
	Mesh						m_mesh;
	ComputeBuffer				*m_meshBuffer;
	ID3D11ShaderResourceView	*m_meshTexture;
	
	//blululululu
	ComputeBuffer				*m_NodeBuffer;


	Node						m_rootNode;
	
	//HWND
	HWND*						m_Hwnd;

	//Timers
	float						m_time;
	float						m_fps;
};

