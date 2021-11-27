#include "D3D.h"
#include "SimpleMath.h"
#include "d3dutil.h"
#include "WindowUtils.h"
#include "ShaderTypes.h"
#include "FX.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

ID3D11InputLayout* gInputLayout;				//vertex description for the gpu
ID3D11Buffer* gQuadVB;							//vertex buffer describing a quad
ID3D11Buffer* gQuadIB;							//index buffer describing a quad
ID3D11VertexShader* pVertexShader = nullptr;	//handle a d3d vertex shader
ID3D11PixelShader* pPixelShader = nullptr;		//handle to a d3d pixel shader
GfxParamsPerObj gGfxData;						//a structure containing extra info the gpu needs to render something
ID3D11Buffer* gpGfxConstsPerObjBuffer;			//handle to a d3d object constant buffer to put the data above into

//the three matrices any 3D renderer needs
Matrix gWorld;
Matrix gView;
Matrix gProj;


const float MAIN_SCALE = 0.75f;		//scale everything the same to fit the screen
float gXTrans = 0, gXVel = 1;		//used in kinematics

//make a sprite geometry buffer
void BuildQuad()
{
	// Create vertex buffer for a quad (two triangle square)
	VertexPosColour vertices[] =
	{
		{ Vector3(-0.5f, -0.5f, 0.f), Colours::White },
		{ Vector3(-0.5f, +0.5f, 0.f), Colours::Black },
		{ Vector3(+0.5f, +0.5f, 0.f), Colours::Red },
		{ Vector3(+0.5f, -0.5f, 0.f), Colours::Green }
	};

	CreateVertexBuffer(WinUtil::Get().GetD3D().GetDevice(), sizeof(VertexPosColour) * 4, vertices, gQuadVB);

	// Create the index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3
	};

	CreateIndexBuffer(WinUtil::Get().GetD3D().GetDevice(), sizeof(UINT) * 6, indices, gQuadIB);
}

//build all buffers
void BuildGeometryBuffers()
{
	BuildQuad();
}

//copy new game constants across to the gpu needed for rendering some geometry
void UpdateConstsPerObj()
{
	gGfxData.wvp = gWorld * gView * gProj;
	WinUtil::Get().GetD3D().GetDeviceCtx().UpdateSubresource(gpGfxConstsPerObjBuffer, 0, nullptr, &gGfxData, 0, 0);
}

//load up everything needed to use shaders
bool BuildFX()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	CheckShaderModel5Supported(d3d.GetDevice());

	// Create the constant buffers for the variables defined in the vertex shader.
	CreateConstantBuffer(d3d.GetDevice(), sizeof(GfxParamsPerObj), &gpGfxConstsPerObjBuffer);

	//load in a pre-compiled vertex shader
	char* pBuff = nullptr;
	unsigned int bytes = 0;
	pBuff = ReadAndAllocate("../bin/data/SimpleVS.cso", bytes);
	CreateVertexShader(d3d.GetDevice(), pBuff, bytes, pVertexShader);
	//create a link between our data and the vertex shader
	CreateInputLayout(d3d.GetDevice(), VertexPosColour::sVertexDesc, 2, pBuff, bytes, &gInputLayout);
	delete[] pBuff;

	//load in a pre-compiled pixel shader	
	pBuff = ReadAndAllocate("../bin/data/SimplePS.cso", bytes);
	CreatePixelShader(d3d.GetDevice(), pBuff, bytes, pPixelShader);
	delete[] pBuff;

	return true;
}


void InitGame()
{
	BuildGeometryBuffers();
	BuildFX();

	gWorld = Matrix::Identity;
	CreateProjectionMatrix(gProj, 0.25f*PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	CreateViewMatrix(gView, Vector3(0, 0, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
}

void ReleaseGame()
{
	ReleaseCOM(pVertexShader);
	ReleaseCOM(pPixelShader);
	ReleaseCOM(gpGfxConstsPerObjBuffer);
	ReleaseCOM(gQuadVB);
	ReleaseCOM(gQuadIB);
	ReleaseCOM(gInputLayout);
}


/*
* render some geometry - setup all buffer, shaders and constants
* pVB - IN vertex buffer
* pIB - IN index buffer
* numIndices - IN how many primitives (triangles) x 3
*/
void RenderShape(ID3D11Buffer *pVB, ID3D11Buffer *pIB, int numIndices)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	d3d.InitInputAssembler(gInputLayout, pVB, sizeof(VertexPosColour), pIB);
	d3d.GetDeviceCtx().VSSetShader(pVertexShader, nullptr, 0);
	d3d.GetDeviceCtx().PSSetShader(pPixelShader, nullptr, 0);
	UpdateConstsPerObj();
	d3d.GetDeviceCtx().VSSetConstantBuffers(0, 1, &gpGfxConstsPerObjBuffer);
	d3d.GetDeviceCtx().DrawIndexed(numIndices, 0, 0);
}

//kinematics update
void Update(float dTime)
{
	gXTrans += gXVel * dTime;
	if (gXTrans > 3)
	{
		gXVel *= -1;
		gXTrans = 3;
	}
	else if (gXTrans < -3)
	{
		gXVel *= -1;
		gXTrans = -3;
	}
}

//render three shapes
void Render(float dTime)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	d3d.BeginRender(Colours::Black);

	gWorld = Matrix::CreateScale(MAIN_SCALE) * Matrix::CreateRotationZ(sinf(GetClock()*0.5f)*10.f);
	RenderShape(gQuadVB, gQuadIB, 6);

	gWorld = Matrix::CreateScale((cosf(GetClock()*4.f)*0.5f + 1.f)*MAIN_SCALE);
	RenderShape(gQuadVB, gQuadIB, 6);

	gWorld = Matrix::CreateScale(MAIN_SCALE) * Matrix::CreateTranslation(gXTrans, 0, 0);
	RenderShape(gQuadVB, gQuadIB, 6);

	d3d.EndRender();
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const float camInc = 0.1f;

	//do something game specific here
	switch (msg)
	{
		// Respond to a keyboard event.
	case WM_CHAR:
		switch (wParam)
		{
		case 27:
			PostQuitMessage(0);
			return 0;
		}
	}

	//default message handling (resize window, full screen, etc)
	return WinUtil::Get().DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

void OnResize(int screenWidth, int screenHeight, MyD3D& d3d)
{
	CreateProjectionMatrix(gProj, 0.25f*PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	d3d.OnResize_Default(screenWidth, screenHeight);
}
