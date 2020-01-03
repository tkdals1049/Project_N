#include "stdafx.h" 
#include "PerlinNoise.h"
#include "../Renders/RenderTarget.h"

PerlinNoise::PerlinNoise()
	:vertexBlob(NULL), pixelBlob(NULL)
	, vertexShader(NULL), pixelShader(NULL)
{
	vertexShaderfinal = NULL;
	pixelShaderfinal = NULL;
	layoutfinal = NULL;

	basePath = Shaders;
	CreateBuffer();
	CreateShader(L"perlin.hlsl");

	rndNoise = new RenderTarget();
	renderReady = false;
}

PerlinNoise::~PerlinNoise()
{
	SAFE_DELETE(rndNoise);

	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(layout);
	SAFE_RELEASE(vertexShaderfinal);
	SAFE_RELEASE(pixelShaderfinal);
	SAFE_RELEASE(layoutfinal);
	SAFE_RELEASE(vertexBlob);
	SAFE_RELEASE(pixelBlob);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(colorBuffer);
}

void PerlinNoise::MakePerlinNoise(float r, float g, float b) 
{
	rndNoise->SetTarget();
	rndNoise->Clear();

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//시드 세팅
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = (float)rand()/256;

	D3D11_MAPPED_SUBRESOURCE subResource;
	HRESULT hr = D3D::GetDC()->Map
	(
		colorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);
	memcpy(subResource.pData, &color, sizeof(D3DXCOLOR));
	D3D::GetDC()->Unmap(colorBuffer, 0);
	D3D::GetDC()->PSSetConstantBuffers(2, 1, &colorBuffer);

	D3D::GetDC()->IASetInputLayout(layout);
	D3D::GetDC()->VSSetShader(vertexShader, NULL, 0);
	D3D::GetDC()->PSSetShader(pixelShader, NULL, 0);

	D3D::GetDC()->DrawIndexed(6, 0, 0);
	D3D::Get()->SetRenderTarget();
}

void PerlinNoise::Render()
{
	if (!renderReady) 
	{
		ReadyToRender();
		renderReady = true;
	}
	//D3D::Get()->SetBlender_Off();
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBufferfinal, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//여기까지 모델에서

	D3D::GetDC()->IASetInputLayout(layoutfinal);
	D3D::GetDC()->VSSetShader(vertexShaderfinal, NULL, 0);
	D3D::GetDC()->PSSetShader(pixelShaderfinal, NULL, 0);
	D3D::GetDC()->PSSetShaderResources(0, 1, rndNoise->GetResourceView());
	
	States::SetDefault();
	D3D::GetDC()->DrawIndexed(6, 0, 0);
}
void PerlinNoise::ReadyToRender()
{
	//화면 그리기용 버텍스버퍼
	VertexTexture* rect = new VertexTexture[4];

	rect[0].position = D3DXVECTOR3(-1, -1, 0);
	rect[1].position = D3DXVECTOR3(-1, 1, 0);
	rect[2].position = D3DXVECTOR3(1, -1, 0);
	rect[3].position = D3DXVECTOR3(1, 1, 0);

	rect[0].uv = D3DXVECTOR2(0, 2);
	rect[1].uv = D3DXVECTOR2(0, 0);
	rect[2].uv = D3DXVECTOR2(2, 2);
	rect[3].uv = D3DXVECTOR2(2, 0);

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexTexture) * 4;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = rect;

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBufferfinal);
	assert(SUCCEEDED(hr));
	SAFE_DELETE_ARRAY(rect);

	//쉐이더 만들고
	wstring file = L"finalDraw.fx";

	SAFE_RELEASE(vertexBlob);
	//SAFE_RELEASE(vertexShader);

	SAFE_RELEASE(pixelBlob);
	//SAFE_RELEASE(pixelShader);
	ID3D10Blob* error;
	hr = D3DX10CompileFromFile
	(
		(basePath + file).c_str(), NULL, NULL, "VS", "vs_5_0"
		, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &vertexBlob, &error, NULL
	);

	if (FAILED(hr))
	{
		if (error != NULL)
		{
			string str = (const char *)error->GetBufferPointer();
			MessageBoxA(NULL, str.c_str(), "Shader Error", MB_OK);
		}
		assert(false);
	}

	hr = D3D::GetDevice()->CreateVertexShader
	(
		vertexBlob->GetBufferPointer()
		, vertexBlob->GetBufferSize()
		, NULL
		, &vertexShaderfinal
	);
	assert(SUCCEEDED(hr));

	hr = D3DX10CompileFromFile
	(
		(basePath + file).c_str(), NULL, NULL, "PS", "ps_5_0"
		, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pixelBlob, &error, NULL
	);

	if (FAILED(hr))
	{
		if (error != NULL)
		{
			string str = (const char *)error->GetBufferPointer();
			MessageBoxA(NULL, str.c_str(), "Shader Error", MB_OK);
		}
		assert(false);
	}

	hr = D3D::GetDevice()->CreatePixelShader
	(
		pixelBlob->GetBufferPointer()
		, pixelBlob->GetBufferSize()
		, NULL
		, &pixelShaderfinal
	);
	assert(SUCCEEDED(hr));

	//레이아웃 만들고
	hr = D3D::GetDevice()->CreateInputLayout
	(
		VertexTexture::desc
		, VertexTexture::count
		, vertexBlob->GetBufferPointer()
		, vertexBlob->GetBufferSize()
		, &layoutfinal
	);
	if (!SUCCEEDED(hr))
		assert(0);
}
ID3D11ShaderResourceView** PerlinNoise::GetPerlinNoise()
{
	return rndNoise->GetResourceView();
}

void PerlinNoise::CreateShader(wstring file)
{
	SAFE_RELEASE(vertexBlob);
	SAFE_RELEASE(vertexShader);

	SAFE_RELEASE(pixelBlob);
	SAFE_RELEASE(pixelShader);
	ID3D10Blob* error;
	HRESULT hr = D3DX10CompileFromFile
	(
		(basePath + file).c_str(), NULL, NULL, "VS", "vs_5_0"
		, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &vertexBlob, &error, NULL
	);

	if (FAILED(hr))
	{
		if (error != NULL)
		{
			string str = (const char *)error->GetBufferPointer();
			MessageBoxA(NULL, str.c_str(), "Shader Error", MB_OK);
		}
		assert(false);
	}

	hr = D3D::GetDevice()->CreateVertexShader
	(
		vertexBlob->GetBufferPointer()
		, vertexBlob->GetBufferSize()
		, NULL
		, &vertexShader
	);
	assert(SUCCEEDED(hr));

	hr = D3DX10CompileFromFile
	(
		(basePath + file).c_str(), NULL, NULL, "PS", "ps_5_0"
		, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pixelBlob, &error, NULL
	);

	if (FAILED(hr))
	{
		if (error != NULL)
		{
			string str = (const char *)error->GetBufferPointer();
			MessageBoxA(NULL, str.c_str(), "Shader Error", MB_OK);
		}
		assert(false);
	}

	hr = D3D::GetDevice()->CreatePixelShader
	(
		pixelBlob->GetBufferPointer()
		, pixelBlob->GetBufferSize()
		, NULL
		, &pixelShader
	);
	assert(SUCCEEDED(hr));

	hr = D3D::GetDevice()->CreateInputLayout
	(
		VertexTexture::desc
		, VertexTexture::count
		, vertexBlob->GetBufferPointer()
		, vertexBlob->GetBufferSize()
		, &layout
	);
	if (!SUCCEEDED(hr))
		assert(0);

}

void PerlinNoise::CreateBuffer()
{

	VertexTexture* rect = new VertexTexture[4];

	rect[0].position = D3DXVECTOR3(-1, -1, 0);
	rect[1].position = D3DXVECTOR3(-1, 1,  0);
	rect[2].position = D3DXVECTOR3(1, -1,  0);
	rect[3].position = D3DXVECTOR3(1, 1,   0);

	rect[0].uv = D3DXVECTOR2(0, 1);
	rect[1].uv = D3DXVECTOR2(0, 0);
	rect[2].uv = D3DXVECTOR2(1, 1);
	rect[3].uv = D3DXVECTOR2(1, 0);

	UINT* index = new UINT[6]{0,1,2,2,1,3};

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexTexture) * 4;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = rect;

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	//2. Index Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * 6;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(rect);
	SAFE_DELETE_ARRAY(index);

	//seedBuffer
//	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(D3DXVECTOR4);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &colorBuffer);
	assert(SUCCEEDED(hr));
}
