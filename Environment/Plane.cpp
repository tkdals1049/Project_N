#include "stdafx.h"
#include "Plane.h"
#include "Brush.h"
#include "../Content/Texture.h"
#include "../Utilities/Mesh.h"

Plane* Plane::instance = NULL;
Plane * Plane::Get()
{
	if (instance == NULL)
		instance = new Plane();

	return instance;
}

void Plane::Delete()
{
	SAFE_DELETE(instance);
}

Plane::Plane()
:width(100),height(100),number(10),OnTextureList(false),
position(D3DXVECTOR3(-(float)width / 2, 0, -(float)height / 2))
{
	wstring file = Shaders + L"PlaneColor.hlsl";
	shader = new Shader(file);

	worldBuffer = new WorldBuffer();
	planeBuffer = new PlaneBuffer();


	vertex = NULL;
	index = NULL;

	vertexCount = (width + 1) * (height + 1);
	
	vector<D3DXVECTOR2> uv;
	UINT heightIndex = 0;
	vertex = new VertexType[vertexCount];
	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			int index = (width + 1) * z + x;
	
			vertex[index].position.x =(float)x+position.x;
			vertex[index].position.y =(float)0+position.y;
			vertex[index].position.z =(float)z+position.z;
	
			vertex[index].uv.x = (x*number) / (float)width;
			vertex[index].uv.y = (z*number) / (float)height;
			uv.push_back(vertex[index].uv);

			vertex[index].normal = D3DXVECTOR3(0, 0, 0);
			vertex[index].color = D3DXCOLOR(0, 0, 0, 0);
			vertex[index].lerp = D3DXCOLOR(0, 0, 0, 0);
		}//for(x)
	}//for(z)
	
	indexCount = width * height * 6;
	index = new UINT[indexCount];
	
	UINT count = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{ // 좌 하단 구하고, 좌 하단의 z 와 x 에 더해준다.
			index[count + 0] = (width + 1) * z + x;
			index[count + 1] = (width + 1) * (z + 1) + x;
			index[count + 2] = (width + 1) * z + x + 1;
			index[count + 3] = (width + 1) * z + x + 1;
			index[count + 4] = (width + 1) * (z + 1) + x;
			index[count + 5] = (width + 1) * (z + 1) + (x + 1);
	
			count += 6;
		}//for(x)
	}//for(z)

	CreateNormalData();

	HRESULT hr;

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertex;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));


	desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	data = { 0 };
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));

	D3DXMatrixIdentity(&world);
	brush=new Brush(this);
	for (int i = 0; i<5; i++)
	textures[i] = new Textures();
	sample = new Textures();
		//SetTexture(0,L"..\\_Contents\\Textures\\Diffuse\\Dirt.png");
}

Plane::~Plane()
{
	SAFE_DELETE_ARRAY(vertex);
	SAFE_DELETE_ARRAY(index);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Plane::Update()
{
	worldBuffer->SetMatrix(world);
}

void Plane::Render()
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0;i < 5;i++)
	{
		D3D::GetDC()->PSSetShaderResources(i, 1, &(textures[i]->texture));
	}
	
	planeBuffer->SetPSBuffer(1);
	worldBuffer->SetVSBuffer(1);	

	shader->Render();

	States::SetDefault();
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void Plane::PostRender(bool& isUse)
{
	ImGui::Begin("Enviroment",&isUse);
	brush->brushOn= ImGui::CollapsingHeader("Plane");
	if (brush->brushOn)
	{

		ImGui::ColorEdit3("Color", (float *)&planeBuffer->Data.Color);
		ImGui::ColorEdit3("Ambient", (float *)&planeBuffer->Data.Ambient);
		ImGui::ColorEdit3("Diffuse", (float *)&planeBuffer->Data.Diffuse);

		ImGui::Text("Mode : ");ImGui::SameLine();
		ImGui::RadioButton("Brush", (int *)&brush->mode, 0); ImGui::SameLine();
		ImGui::RadioButton("Brush2", (int *)&brush->mode, 1); ImGui::SameLine();
		ImGui::RadioButton("Color", (int *)&brush->mode, 2); ImGui::SameLine();
		ImGui::RadioButton("Texture", (int *)&brush->mode, 3);

		ImGui::Text("Type : ");ImGui::SameLine();
		ImGui::RadioButton("Square", (int *)&brush->type, 0); ImGui::SameLine();
		ImGui::RadioButton("Circle", (int *)&brush->type, 1);

		ImGui::InputInt("Size", (int *)&brush->size);

		ImGui::Separator();

		if (brush->mode == 1)ImGui::InputFloat("Brush Height", &brush->floor);
		else if (brush->mode == 2)	ImGui::ColorEdit3("Brush COlor", (float *)&brush->color);
		else if (brush->mode == 3)
		{
			static int curTexture = 0,preTexture= -1,textureNum=0;
			if(curTexture!=preTexture) 
			{
				SetSample(curTexture);
				preTexture=curTexture;
			}

			ImGui::InputInt("Texture Number", (int *)&brush->num);

			ImGui::PushItemWidth(160);
			ImGui::ListBox("", &curTexture, Texture::Get()->GetTextureList(), Texture::Get()->GetTexturePathSize(), 4);
			ImGui::SameLine();
			ImGui::Image(sample->texture, ImVec2(80, 80));
			if (ImGui::Button("Refresh", ImVec2(60, 20)))
			{
				Texture::Get()->Refresh();
			}
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(100);
			ImGui::InputInt("0<=Num<5",&textureNum);
			ImGui::SameLine();
			if (ImGui::Button("Input"))
			{
				if(textureNum>=0&& textureNum<5)
				SetTexture(textureNum, String::StringToWString(Texture::Get()->GetTexturePath(curTexture)));
			}
			ImGui::PopItemWidth();

			ImGui::InputText("DiffuseTexture",(char*)String::WStringToString(textures[0]->file).c_str(),textures[0]->file.size());
			ImGui::InputText("Stage1Texture", (char*)String::WStringToString(textures[1]->file).c_str(), textures[1]->file.size());
			ImGui::InputText("Stage2Texture", (char*)String::WStringToString(textures[2]->file).c_str(), textures[2]->file.size());
			ImGui::InputText("Stage3Texture", (char*)String::WStringToString(textures[3]->file).c_str(), textures[3]->file.size());
			ImGui::InputText("Stage4Texture", (char*)String::WStringToString(textures[4]->file).c_str(), textures[4]->file.size());
		}
	}
	ImGui::End();
	//ImGui::Checkbox("Demo Window", &OnTextureList);
	//if (OnTextureList == true)
	//{
	//	ImGui::Begin("Texture List Window", &OnTextureList, ImGuiWindowFlags_AlwaysAutoResize);
	//	ImGui::End();
	//}
}
void Plane::SetSample(int num)
{
	sample->file = String::StringToWString(Texture::Get()->GetTexturePath(num));

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice()
		, sample->file.c_str()
		, NULL
		, NULL
		, &sample->texture
		, NULL
	);
	assert(SUCCEEDED(hr));

}
void Plane::SetTexture(int num, wstring file)
{
	textures[num]->file = file;

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice()
		, file.c_str()
		, NULL
		, NULL
		, &textures[num]->texture
		, NULL
	);
	assert(SUCCEEDED(hr));
}

void Plane::UpdatePointBuffer(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	brush->Update(origin,direction);
	dot = brush->position;

	D3DXVECTOR4 size = D3DXVECTOR4((float)((dot.x - position.x) * number / width), (float)((dot.y - position.z) * number / height), (float)brush->type, (float)brush->size);
	D3DXVECTOR4 point = D3DXVECTOR4((dot.x - position.x), (dot.y - position.z), dot.x, dot.y);

	planeBuffer->Data.Size=size;
	planeBuffer->Data.Point=point;
}

void Plane::CreateNormalData()
{
	for (UINT i = 0;i<vertexCount;i++)
		vertex[i].normal = D3DXVECTOR3(0, 0, 0);
	for (UINT i = 0; i < (indexCount / 3); i++)
	{
		UINT index0 = index[i * 3 + 0];
		UINT index1 = index[i * 3 + 1];
		UINT index2 = index[i * 3 + 2];

		VertexType v0 = vertex[index0];
		VertexType v1 = vertex[index1];
		VertexType v2 = vertex[index2];

		D3DXVECTOR3 x = v1.position - v0.position;
		D3DXVECTOR3 y = v2.position - v0.position;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &x, &y);

		vertex[index0].normal += normal;
		vertex[index1].normal += normal;
		vertex[index2].normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
	{
		D3DXVec3Normalize
		(
			&vertex[i].normal
			, &vertex[i].normal
		);
	}
}