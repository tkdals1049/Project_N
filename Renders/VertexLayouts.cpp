#include "stdafx.h"
#include "VertexLayouts.h"

#define VERTEX_RG DXGI_FORMAT_R32G32_FLOAT
#define VERTEX_RGB DXGI_FORMAT_R32G32B32_FLOAT
#define VERTEX_RGBA DXGI_FORMAT_R32G32B32A32_FLOAT
#define VERTEX D3D11_INPUT_PER_VERTEX_DATA

#define ALIGNED_ELEMENT D3D11_APPEND_ALIGNED_ELEMENT

//////////////////////////////////////////////////////////

D3D11_INPUT_ELEMENT_DESC Vertex::desc[] =
{
	{ "POSITION", 0, VERTEX_RGB, 0, 0, VERTEX, 0 },
};
UINT Vertex::count = 1;

D3D11_INPUT_ELEMENT_DESC VertexColor::desc[] =
{
	{ "POSITION", 0, VERTEX_RGB, 0, 0, VERTEX, 0 },
	{ "COLOR", 0, VERTEX_RGBA, 0, ALIGNED_ELEMENT, VERTEX, 0 },
};
UINT VertexColor::count = 2;

D3D11_INPUT_ELEMENT_DESC VertexColorNormal::desc[] =
{
	{ "POSITION", 0, VERTEX_RGB, 0, 0, VERTEX, 0 },
	{ "COLOR", 0, VERTEX_RGBA, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "NORMAL", 0, VERTEX_RGB, 0, ALIGNED_ELEMENT, VERTEX, 0 },
};
UINT VertexColorNormal::count = 3;

D3D11_INPUT_ELEMENT_DESC VertexTexture::desc[] =
{
	{ "POSITION", 0, VERTEX_RGB, 0, 0, VERTEX, 0 },
	{ "TEXCOORD", 0, VERTEX_RG, 0, ALIGNED_ELEMENT, VERTEX, 0 },
};
UINT VertexTexture::count = 2;

D3D11_INPUT_ELEMENT_DESC VertexTextureNormal::desc[] =
{
	{ "POSITION", 0, VERTEX_RGB, 0, 0, VERTEX, 0 },
	{ "TEXCOORD", 0, VERTEX_RG, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "NORMAL", 0, VERTEX_RGB, 0, ALIGNED_ELEMENT, VERTEX, 0 },
};
UINT VertexTextureNormal::count = 3;

D3D11_INPUT_ELEMENT_DESC VertexColorTextureNormal::desc[] =
{
	{ "POSITION", 0, VERTEX_RGB, 0, 0, VERTEX, 0 },
	{ "COLOR", 0, VERTEX_RGBA, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "TEXCOORD", 0, VERTEX_RG, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "NORMAL", 0, VERTEX_RGB, 0, ALIGNED_ELEMENT, VERTEX, 0 },
};
UINT VertexColorTextureNormal::count = 4;

D3D11_INPUT_ELEMENT_DESC VertexLerpColorTextureNormal::desc[] =
{
	{ "POSITION", 0, VERTEX_RGB, 0, 0, VERTEX, 0 },
	{ "COLOR", 0, VERTEX_RGBA, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "LERP", 0, VERTEX_RGBA, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "TEXCOORD", 0, VERTEX_RG, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "NORMAL", 0, VERTEX_RGB, 0, ALIGNED_ELEMENT, VERTEX, 0 },
};
UINT VertexLerpColorTextureNormal::count = 5;

D3D11_INPUT_ELEMENT_DESC VertexTextureNormalTangentBlend::desc[] =
{
	{ "POSITION", 0, VERTEX_RGB, 0, 0, VERTEX, 0 },
	{ "TEXCOORD", 0, VERTEX_RG, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "NORMAL", 0, VERTEX_RGB, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "TANGENT", 0, VERTEX_RGB, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "BLENDINDICES", 0, VERTEX_RGBA, 0, ALIGNED_ELEMENT, VERTEX, 0 },
	{ "BLENDWEIGHT", 0, VERTEX_RGBA, 0, ALIGNED_ELEMENT, VERTEX, 0 },
};
UINT VertexTextureNormalTangentBlend::count = 6;

//////////////////////////////////////////////////////////

Vertex::Vertex()
{
	position = D3DXVECTOR3(0, 0, 0);
}

Vertex::Vertex(D3DXVECTOR3 position)
{
	this->position = position;
}

//////////////////////////////////////////////////////////

VertexColor::VertexColor()
{
	position = D3DXVECTOR3(0, 0, 0);
	color = D3DXCOLOR(0, 0, 0, 1);
}

VertexColor::VertexColor(D3DXVECTOR3 position, D3DXCOLOR color)
{
	this->position = position;
	this->color = color;
}

//////////////////////////////////////////////////////////

VertexColorNormal::VertexColorNormal()
{
	position = D3DXVECTOR3(0, 0, 0);
	color = D3DXCOLOR(0, 0, 0, 1);
	normal = D3DXVECTOR3(0, 0, 0);
}

VertexColorNormal::VertexColorNormal(D3DXVECTOR3 position, D3DXCOLOR color, D3DXVECTOR3 normal)
{
	this->position = position;
	this->color = color;
	this->normal = normal;
}

//////////////////////////////////////////////////////////

VertexTexture::VertexTexture()
{
	position = D3DXVECTOR3(0, 0, 0);
	uv = D3DXVECTOR2(0, 0);
}

VertexTexture::VertexTexture(D3DXVECTOR3 position, D3DXVECTOR2 uv)
{
	this->position = position;
	this->uv = uv;
}

//////////////////////////////////////////////////////////

VertexTextureNormal::VertexTextureNormal()
{
	position = D3DXVECTOR3(0, 0, 0);
	uv = D3DXVECTOR2(0, 0);
	normal = D3DXVECTOR3(0, 0, 0);
}

VertexTextureNormal::VertexTextureNormal(D3DXVECTOR3 position, D3DXVECTOR2 uv, D3DXVECTOR3 normal)
{
	this->position = position;
	this->uv = uv;
	this->normal = normal;
}

//////////////////////////////////////////////////////////

VertexColorTextureNormal::VertexColorTextureNormal()
{
	position = D3DXVECTOR3(0, 0, 0);
	color = D3DXCOLOR(0, 0, 0, 1);
	uv = D3DXVECTOR2(0, 0);
	normal = D3DXVECTOR3(0, 0, 0);
}

VertexColorTextureNormal::VertexColorTextureNormal(D3DXVECTOR3 position, D3DXCOLOR color, D3DXVECTOR2 uv, D3DXVECTOR3 normal)
{
	this->position = position;
	this->color = color;
	this->uv = uv;
	this->normal = normal;
}

//////////////////////////////////////////////////////////

VertexLerpColorTextureNormal::VertexLerpColorTextureNormal()
{
	position = D3DXVECTOR3(0, 0, 0);
	color = D3DXCOLOR(0, 0, 0, 1);
	lerp = D3DXCOLOR(0, 0, 0, 0);
	uv = D3DXVECTOR2(0, 0);
	normal = D3DXVECTOR3(0, 0, 0);
}

VertexLerpColorTextureNormal::VertexLerpColorTextureNormal(D3DXVECTOR3 position, D3DXCOLOR color, D3DXCOLOR lerp, D3DXVECTOR2 uv, D3DXVECTOR3 normal)
{
	this->position = position;
	this->color = color;
	this->lerp = lerp;
	this->uv = uv;
	this->normal = normal;
}


//////////////////////////////////////////////////////////

VertexTextureNormalTangentBlend::VertexTextureNormalTangentBlend()
{
	this->position = D3DXVECTOR3(0, 0, 0);
	this->uv = D3DXVECTOR2(0, 0);
	this->normal = D3DXVECTOR3(0, 0, 0);
	this->tangent = D3DXVECTOR3(0, 0, 0);
	this->blendIndices = D3DXVECTOR4(0, 0, 0, 0);
	this->blendWeights = D3DXVECTOR4(0, 0, 0, 0);
}

VertexTextureNormalTangentBlend::VertexTextureNormalTangentBlend(D3DXVECTOR3 position, D3DXVECTOR2 uv, D3DXVECTOR3 normal, D3DXVECTOR3 tangent, D3DXVECTOR4 blendIndices, D3DXVECTOR4 blendWeights)
{
	this->position = position;
	this->uv = uv;
	this->normal = normal;
	this->tangent = tangent;
	this->blendIndices = blendIndices;
	this->blendWeights = blendWeights;
}
