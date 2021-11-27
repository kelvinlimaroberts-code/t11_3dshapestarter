#ifndef SHADERTYPES_H
#define SHADERTYPES_H

#include <d3d11.h>

#include "SimpleMath.h"



/*
This is what our vertex data will look like
*/
struct VertexPosColour
{
	DirectX::SimpleMath::Vector3 Pos;			//local space position of the vertex
	DirectX::SimpleMath::Vector4 Colour;		//colour red, green, blue, alpha

	//a description of this structure that we can pass to d3d
	static const D3D11_INPUT_ELEMENT_DESC sVertexDesc[2];
	//handy function to convert 0->255 int colour intensities to 0->1 values for D3D
	void SetColour(unsigned int argb)
	{
		Colour.w = (float)(argb >> 24) / 255.f;
		Colour.x = (float)((argb >> 16) & 255) / 255.f;
		Colour.y = (float)((argb >> 8) & 255) / 255.f;
		Colour.z = (float)((argb >> 24) & 255) / 255.f;
	}
};

/*
This is a constant buffer providing important information
to the shader. It's size must be a multiple of 16bytes (16byte aligned).
*/
struct GfxParamsPerObj
{
	DirectX::SimpleMath::Matrix wvp;
};


static_assert((sizeof(GfxParamsPerObj) % 16) == 0, "CB size not padded correctly");

#endif
