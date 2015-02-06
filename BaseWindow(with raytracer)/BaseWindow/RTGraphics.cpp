#include "RTGraphics.h"


 RTGraphics::RTGraphics(HWND* _hwnd)
: m_mesh(Mesh()),
m_meshTexture(nullptr),
m_time(0.f),
m_fps(0.f)
{
	HRESULT hr = S_OK;

	m_Hwnd = _hwnd;

	computeWrap = new ComputeWrap(g_Device,g_DeviceContext);

	raytracer = computeWrap->CreateComputeShader("Raytracing");

	ID3D11Texture2D* pBackBuffer;
	hr = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		MessageBox(NULL, "failed getting the backbuffer", "RTRenderDX11 Error", S_OK);

	// create shader unordered access view on back buffer for compute shader to write into texture
	hr = g_Device->CreateUnorderedAccessView(pBackBuffer, NULL, &backbuffer);

	//creating constant buffers
	createCBuffers();

	//createing triangle texture
	createTriangleTexture();


}

void RTGraphics::createCBuffers()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	// CPU writable, should be updated per frame
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;

	if (sizeof(cBuffer) % 16 < 16)
	{
		cbDesc.ByteWidth = (int)((sizeof(cBuffer) / 16) + 1) * 16;
	}
	else
	{
		cbDesc.ByteWidth = sizeof(cBuffer);
	}

	hr = g_Device->CreateBuffer(&cbDesc, NULL, &g_cBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, "Failed Making Constant Buffer", "Create Buffer", MB_OK);
	}
	g_DeviceContext->CSSetConstantBuffers(0, 1, &g_cBuffer);

}

void RTGraphics::createTriangleTexture()
{
	///////////////////////////////////////////////////////////////////////////////////////////
	//Mesh
	///////////////////////////////////////////////////////////////////////////////////////////

	//Load OBJ-file
	m_mesh.loadObj("Meshi/kub.obj");
	m_mesh.setColor(XMFLOAT4(1,0,0,1));
	createKdTree(&m_mesh);

	m_meshBuffer = computeWrap->CreateBuffer(STRUCTURED_BUFFER,
											 sizeof(TriangleMat),
											 m_mesh.getNrOfFaces(),
											 true,
											 false,
											 m_mesh.getTriangles(),
											 false,
											 "Structured Buffer: Mesh Texture");


	//from wchat_t to string
	//std::string narrow = converter.to_bytes(wide_utf16_source_string);
	//from string to wchar_t
	std::wstring meshTextureWstring = converter.from_bytes(m_mesh.getTextureString());


	//TEXTURE STUFF
	CreateWICTextureFromFile(g_Device, 
							 g_DeviceContext,
							 meshTextureWstring.c_str(),
							 NULL, 
							 &m_meshTexture);

}

RTGraphics::~RTGraphics()
{
}

void RTGraphics::Update(float _dt)
{
	// updating the constant buffer holding the camera transforms
	XMFLOAT4X4 temp, viewInv, projInv;
	XMFLOAT3 tempp = Cam->getCameraPosition(); // w ska va 1
	XMStoreFloat4x4(&temp, XMMatrixIdentity());

	XMStoreFloat4x4(&temp, XMMatrixTranslation(tempp.x,tempp.y,tempp.z));

	XMStoreFloat4x4(&temp, XMMatrixTranspose(XMLoadFloat4x4(&temp)));

	XMStoreFloat4x4(&viewInv, XMMatrixInverse(&XMMatrixDeterminant(
		XMLoadFloat4x4(&Cam->getViewMatrix())), XMLoadFloat4x4(&Cam->getViewMatrix())));

	XMStoreFloat4x4(&projInv, XMMatrixInverse(&XMMatrixDeterminant(
		XMLoadFloat4x4(&Cam->getProjectionMatrix())), XMLoadFloat4x4(&Cam->getProjectionMatrix())));


	cb.IV = viewInv;
	cb.IP = projInv;
	cb.cameraPos = XMFLOAT4(tempp.x, tempp.y, tempp.z, 1);
	cb.nrOfTriangles = m_mesh.getNrOfFaces();
	g_DeviceContext->UpdateSubresource(g_cBuffer, 0, NULL, &cb, 0, 0);

	m_time += _dt;
	static int frameCnt = 0;
	static float t_base = 0.f;
	frameCnt++;

	if (m_time - t_base >= 1.f)
	{
		frameCnt /= 1;
		m_fps = (float)frameCnt;
		frameCnt = 0;
		t_base += 1.f;
	}

}

void RTGraphics::Render(float _dt)
{
	//set shader
	raytracer->Set();

	//set buffers
	g_DeviceContext->CSSetUnorderedAccessViews(0,1,&backbuffer,NULL);

	//set textures
	ID3D11ShaderResourceView *srv[] = { m_meshTexture, m_meshBuffer->GetResourceView() };
	g_DeviceContext->CSSetShaderResources(0, 2, srv);

	//dispatch
	g_DeviceContext->Dispatch(NROFTHREADSWIDTH, NROFTHREADSHEIGHT, 1);

	//unset buffers
	ID3D11UnorderedAccessView* nulluav[] = { NULL, NULL, NULL, NULL };
	g_DeviceContext->CSSetUnorderedAccessViews(0, 4, nulluav, NULL);

	//unset shader
	raytracer->Unset();

	//present scene
	if (FAILED(g_SwapChain->Present(0, 0)))
		MessageBox(NULL,"Failed to present the swapchain","RT Render Error",S_OK);

	//Title text and some timers
	char title[256];
	sprintf_s(
		title,
		sizeof(title),
		"Super mega awesume project 3 - fps: %f",
		m_fps
		);
	SetWindowText(*m_Hwnd, title);
}

void RTGraphics::createKdTree(Mesh *_mesh)
{
	std::vector<TriangleMat> *triangleList = _mesh->getTriangleList();
	std::vector<AABB> aabbList;


	// CREATION OF AABB LIST
	for (int i = 0; i < triangleList->size(); i++)
	{
		float min = INFINITY;
		float max = -INFINITY;

		TriangleMat tri = triangleList->at(i);
		AABB aabb;

		aabb.maxPoint.x = (tri.pos0.x > tri.pos1.x) ? tri.pos0.x : tri.pos1.x;
		aabb.maxPoint.x = (aabb.maxPoint.x > tri.pos2.x) ? aabb.maxPoint.x : tri.pos2.x;

		aabb.maxPoint.y = (tri.pos0.y > tri.pos1.y) ? tri.pos0.y : tri.pos1.y;
		aabb.maxPoint.y = (aabb.maxPoint.y > tri.pos2.y) ? aabb.maxPoint.y : tri.pos2.y;

		aabb.maxPoint.z = (tri.pos0.z > tri.pos1.z) ? tri.pos0.z : tri.pos1.z;
		aabb.maxPoint.z = (aabb.maxPoint.z > tri.pos2.z) ? aabb.maxPoint.z : tri.pos2.z;

		aabb.minPoint.x = (tri.pos0.x < tri.pos1.x) ? tri.pos0.x : tri.pos1.x;
		aabb.minPoint.x = (aabb.minPoint.x < tri.pos2.x) ? aabb.minPoint.x : tri.pos2.x;

		aabb.minPoint.y = (tri.pos0.y < tri.pos1.y) ? tri.pos0.y : tri.pos1.y;
		aabb.minPoint.y = (aabb.minPoint.y < tri.pos2.y) ? aabb.minPoint.y : tri.pos2.y;

		aabb.minPoint.z = (tri.pos0.z < tri.pos1.z) ? tri.pos0.z : tri.pos1.z;
		aabb.minPoint.z = (aabb.minPoint.z < tri.pos2.z) ? aabb.minPoint.z : tri.pos2.z;

		aabb.triangleIndex = i;

		aabbList.push_back(aabb);
	}

	createKDNodeSplit(&aabbList,m_rootNode,1);

}

void sortAABBX(std::vector<AABB>* _AABBList)
{

}


void RTGraphics::createKDNodeSplit(std::vector<AABB>* _aabbList, Node _node, int _split)
{
	switch (_split)
	{
	case 1:		// SPLITT IN X

		break;
	case 2:		// SPLITT IN Y

		break;
	case 3:		// SPLITT IN Z

		break;
	}
}


