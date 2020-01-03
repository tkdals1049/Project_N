#include "stdafx.h"
#include "Blood.h"

Blood::Blood():width(3),height(3),lookNum(0),isblood(false),time(0),position(D3DXVECTOR3(0,0,0)),rotate(D3DXVECTOR3(0,0,0))
{
	shader = new Shader(Shaders + L"Texture.hlsl");
	worldBuffer = new WorldBuffer();
		
	CreateBuffer();

	fileName = Contents + L"Textures/Diffuse/Blood.png";
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice()
		, fileName.c_str()
		, NULL
		, NULL
		, &texture
		, NULL
	);
	assert(SUCCEEDED(hr));
}

Blood::~Blood()
{
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	SAFE_RELEASE(texture);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE_ARRAY(index);
	SAFE_DELETE_ARRAY(vertex);
}

void Blood::PreUpdate()
{
}

void Blood::Update()
{
	if (isblood)
	{
		D3DXVECTOR3 cameraPos;

		D3DXVec3TransformCoord(&cameraPos, &D3DXVECTOR3(0, 0, 0), &CameraManager::Get()->GetView());

		rotate.y =  atan2(cameraPos.z - position.z, cameraPos.x - position.x);
		rotate.x =  atan2(cameraPos.y - position.y,sqrt(pow(cameraPos.z - position.z, 2) + pow(cameraPos.x - position.x, 2)));
		
		time += Time::Get()->Delta();
		if (time >0.05f)
		{
			lookNum++;
			time = 0;
		}
		D3DXMATRIX world,scale,rotateX,rotateY;
		D3DXMatrixRotationX(&rotateX, rotate.x);
		D3DXMatrixRotationY(&rotateY, rotate.y);
		D3DXMatrixTranslation(&world,position.x,position.y,position.z);
		D3DXMatrixScaling(&scale,(float)(lookNum+1), (float)(lookNum + 1), (float)(lookNum + 1));
		worldBuffer->SetMatrix(rotateX*rotateY*scale*world);
	//D3DXMATRIX world, scale, rotateX, rotateY;
	worldBuffer->SetMatrix(rotateX*scale*world);
	}
	if (lookNum >= width * height) { lookNum = 0;isblood = false; }

}

void Blood::Render()
{

	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);

	shader->Render();

	D3D::GetDC()->PSSetShaderResources(0, 1, &texture);

	States::SetDepthStencilMaskZero();
	States::SetBlendOn();
	{
		dc->DrawIndexed(6*isblood, 6*lookNum, 0);
	}
	States::SetBlendOff();
	States::SetDepthStencilDefault();

}

void Blood::CreateBuffer()
{
	HRESULT hr;

	vertex = NULL;
	vertexCount = width*height*4;
	vertex = new VertexType[vertexCount];
	
	index = NULL;
	indexCount = width * height *6;
	index = new UINT[indexCount];

	float temp=0.33333333f;
	int num[6]{0,1,2,2,1,3};

	for (int i = 0;i < (int)height;i++)
	{
		for (int j = 0;j < (int)width;j++)
		{
			int count = j * width + i;

			vertex[count * 4 + 0].position = D3DXVECTOR3(-1, -1, 0);
			vertex[count * 4 + 1].position = D3DXVECTOR3(-1, 1, 0);
			vertex[count * 4 + 2].position = D3DXVECTOR3(1, -1, 0);
			vertex[count * 4 + 3].position = D3DXVECTOR3(1, 1, 0);
			
			vertex[count * 4 + 0].uv= D3DXVECTOR2(temp*(j), temp*(i));
			vertex[count * 4 + 1].uv= D3DXVECTOR2(temp*(j+1), temp*(i));
			vertex[count * 4 + 2].uv= D3DXVECTOR2(temp*(j), temp*(i+1));
			vertex[count * 4 + 3].uv= D3DXVECTOR2(temp*(j+1), temp*(i+1));

			for (int k = 0;k<6;k++) index[count * 6 + k] = count * 4 + num[k];
		}
	}

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertex;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	//2. Index Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));
}