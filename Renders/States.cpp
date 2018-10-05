#include "stdafx.h"
#include "States.h"

D3D11_RASTERIZER_DESC States::rasterizerDesc;
D3D11_DEPTH_STENCIL_DESC States::depthStencilDesc;
D3D11_SAMPLER_DESC States::samplerDesc;
D3D11_BLEND_DESC States::blendDesc;

ID3D11RasterizerState* States::wireRasterizerState;
ID3D11RasterizerState* States::defaultRasterizerState;
ID3D11RasterizerState* States::frontRasterizerState;
ID3D11RasterizerState* States::noneRasterizerState;

ID3D11DepthStencilState* States::defaultDepthStencilState;
ID3D11DepthStencilState* States::offDepthStencilState;
ID3D11DepthStencilState* States::MaskZeroDepthStencilState;

ID3D11BlendState* States::onBlendState;
ID3D11BlendState* States::cloudBlendState;
ID3D11BlendState* States::offBlendState;

ID3D11SamplerState* States::defaultSamplerState;

void States::Create()
{
	CreateRasterizerDesc();
	CreateDepthStencilDesc();
	CreateSamplerDesc();
	CreateBlendDesc();
}

void States::Delete()
{
}

void States::GetRasterizerDesc(D3D11_RASTERIZER_DESC* desc)
{
	memcpy(desc, &rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
}

void States::CreateRasterizer(D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** state)
{
	HRESULT hr;

	hr = D3D::GetDevice()->CreateRasterizerState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetRasterizer(ID3D11RasterizerState * state)
{
	D3D::GetDC()->RSSetState(state);
}

void States::GetDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* desc)
{
	memcpy(desc, &depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
}

void States::CreateDepthStencil(D3D11_DEPTH_STENCIL_DESC * desc, ID3D11DepthStencilState ** state)
{
	HRESULT hr;

	hr = D3D::GetDevice()->CreateDepthStencilState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetBlend(ID3D11BlendState * state)
{
	float blendFactor[4]{0.0f,0.0f,0.0f,0.0f};
	D3D::GetDC()->OMSetBlendState(state, blendFactor, 0xffffffff);
}

void States::GetBlendDesc(D3D11_BLEND_DESC* desc)
{
	memcpy(desc, &blendDesc, sizeof(D3D11_BLEND_DESC));
}
void States::CreateBlend(D3D11_BLEND_DESC * desc, ID3D11BlendState ** state)
{
	HRESULT hr = D3D::GetDevice()->CreateBlendState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetDepthStencil(ID3D11DepthStencilState * state)
{
	D3D::GetDC()->OMSetDepthStencilState(state, 1);
}

void States::GetSamplerDesc(D3D11_SAMPLER_DESC * desc)
{
	memcpy(desc, &samplerDesc, sizeof(D3D11_SAMPLER_DESC));
}

void States::CreateSampler(D3D11_SAMPLER_DESC * desc, ID3D11SamplerState ** state)
{
	HRESULT hr;

	hr = D3D::GetDevice()->CreateSamplerState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetSampler(UINT slot, UINT count, ID3D11SamplerState * state)
{
	D3D::GetDC()->PSSetSamplers(slot, count, &state);
}


void States::CreateRasterizerDesc()
{
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	CreateRasterizer(&rasterizerDesc, &wireRasterizerState);

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	CreateRasterizer(&rasterizerDesc,&defaultRasterizerState);

	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	CreateRasterizer(&rasterizerDesc, &frontRasterizerState);

	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	CreateRasterizer(&rasterizerDesc, &noneRasterizerState);
}

void States::CreateDepthStencilDesc()
{
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	CreateDepthStencil(&depthStencilDesc, &defaultDepthStencilState);

	depthStencilDesc.DepthEnable = false;
	CreateDepthStencil(&depthStencilDesc, &offDepthStencilState);

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	CreateDepthStencil(&depthStencilDesc, &MaskZeroDepthStencilState);
}

void States::CreateSamplerDesc()
{
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = D3D::GetDevice()->CreateSamplerState(&samplerDesc, &defaultSamplerState);
	assert(SUCCEEDED(hr));
}

void States::CreateBlendDesc()
{
	// Clear the blend state description.
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	CreateBlend(&blendDesc, &onBlendState);

	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	CreateBlend(&blendDesc, &offBlendState);

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	CreateBlend(&blendDesc, &cloudBlendState);

}

void States::SetDepthStencilDefault()
{
	D3D::GetDC()->OMSetDepthStencilState(defaultDepthStencilState, 1);
}
void States::SetDepthStencilOff()
{
	D3D::GetDC()->OMSetDepthStencilState(offDepthStencilState, 1);
}
void States::SetDepthStencilMaskZero()
{
	D3D::GetDC()->OMSetDepthStencilState(MaskZeroDepthStencilState, 1);
}

void States::SetBlendOn()
{
	float blendFactor[4]{ 0.0f,0.0f,0.0f,0.0f };
	D3D::GetDC()->OMSetBlendState(onBlendState, blendFactor, 0xffffffff);
}
void States::SetBlendOff()
{
	float blendFactor[4]{ 0.0f,0.0f,0.0f,0.0f };
	D3D::GetDC()->OMSetBlendState(offBlendState, blendFactor, 0xffffffff);
}
void States::SetBlendCloud()
{
	float blendFactor[4]{ 0.0f,0.0f,0.0f,0.0f };
	D3D::GetDC()->OMSetBlendState(cloudBlendState, blendFactor, 0xffffffff);
}

void States::SetRasterizerDefault()
{
	D3D::GetDC()->RSSetState(defaultRasterizerState);
}

void States::SetRasterizerWire()
{
	D3D::GetDC()->RSSetState(wireRasterizerState);
}

void States::SetRasterizerFront()
{
	D3D::GetDC()->RSSetState(frontRasterizerState);
}

void States::SetRasterizerNone()
{
	D3D::GetDC()->RSSetState(noneRasterizerState);
}

void States::SetDefault()
{
	D3D::GetDC()->PSSetSamplers(0, 1, &defaultSamplerState);
}

