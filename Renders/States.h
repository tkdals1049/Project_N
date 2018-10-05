#pragma once

class States
{
public:
	static void Create();
	static void Delete();

	static void GetRasterizerDesc(D3D11_RASTERIZER_DESC* desc);
	static void CreateRasterizer(D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** state);
	static void SetRasterizer(ID3D11RasterizerState* state);

	static void GetDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* desc);
	static void CreateDepthStencil(D3D11_DEPTH_STENCIL_DESC* desc, ID3D11DepthStencilState** state);
	static void SetDepthStencil(ID3D11DepthStencilState* state);

	static void SetBlend(ID3D11BlendState * state);
	static void GetBlendDesc(D3D11_BLEND_DESC * desc);
	static void CreateBlend(D3D11_BLEND_DESC * desc, ID3D11BlendState ** state);

	static void GetSamplerDesc(D3D11_SAMPLER_DESC* desc);
	static void CreateSampler(D3D11_SAMPLER_DESC* desc, ID3D11SamplerState** state);
	static void SetSampler(UINT slot, UINT count, ID3D11SamplerState* state);

	static void SetDefault();

	static void SetRasterizerDefault();
	static void SetRasterizerWire();
	static void SetRasterizerFront();
	static void SetRasterizerNone();

	static void SetDepthStencilDefault();
	static void SetDepthStencilOff();
	static void SetDepthStencilMaskZero();

	static void SetBlendOn();
	static void SetBlendOff();

	static void SetBlendCloud();

private:
	static void CreateRasterizerDesc();
	static void CreateDepthStencilDesc();
	static void CreateSamplerDesc();
	static void CreateBlendDesc();

	static D3D11_RASTERIZER_DESC rasterizerDesc;
	static D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	static D3D11_SAMPLER_DESC samplerDesc;
	static D3D11_BLEND_DESC blendDesc;

	static ID3D11RasterizerState* defaultRasterizerState;
	static ID3D11RasterizerState* wireRasterizerState;
	static ID3D11RasterizerState* frontRasterizerState;
	static ID3D11RasterizerState* noneRasterizerState;

	static ID3D11DepthStencilState* defaultDepthStencilState;
	static ID3D11DepthStencilState* offDepthStencilState;
	static ID3D11DepthStencilState* MaskZeroDepthStencilState;

	static ID3D11BlendState* onBlendState;
	static ID3D11BlendState* cloudBlendState;
	static ID3D11BlendState* offBlendState;

	static ID3D11SamplerState* defaultSamplerState;
};