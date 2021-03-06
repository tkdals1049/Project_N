#include "stdafx.h"
#include "Plane.h"
#include "Brush.h"
#include "Water.h"
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
:OnTextureList(false),isLoaded(true), quadTree(NULL),number(5)
{
	width = height = (UINT)pow(2,number);
	wstring file = Shaders + L"PlaneColor.hlsl";
	shader = new Shader(file);

	D3DXMatrixIdentity(&world);
	worldBuffer = new WorldBuffer();
	planeBuffer = new PlaneBuffer();
	clipBuffer = new ClipBuffer();

	loadThread=NULL;
	vertex = NULL;
	index = NULL;
	brush=NULL;
	water=NULL;

	//LoadHeightMap();
	position = D3DXVECTOR3(-(float)width / 2, 0, -(float)height / 2);
	CreateBuffer();
}

Plane::~Plane()
{
	for (int i = 0; i < 5; i++)
	SAFE_DELETE(textures[i]);
	SAFE_DELETE(sample);
	SAFE_DELETE(loadThread);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(planeBuffer);
	SAFE_DELETE(clipBuffer);
	SAFE_DELETE(shader);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE_ARRAY(vertex);
	SAFE_DELETE_ARRAY(index);

	if (brush != NULL) SAFE_DELETE(brush);
	if (water != NULL) SAFE_DELETE(water);
	if (quadTree != NULL) SAFE_DELETE(quadTree);
}

void Plane::Update()
{
	worldBuffer->SetMatrix(world);

	if (isLoaded)
	{
		if (loadThread != NULL)
		{
			loadThread->join();
			SAFE_DELETE(loadThread);
		}
		
		indexCount = (UINT)quadTree->GenerateIndex(index, vertex);
		D3D::GetDC()->UpdateSubresource(indexBuffer, 0, NULL, index, sizeof(UINT)*indexCount, 0);
		CreateNormalData();

		if (water != NULL) water->Update();
	}
}

void Plane::Render()
{
	D3DXMATRIX invWorld;
	D3DXMatrixInverse(&invWorld, nullptr, &world);
	D3DXMatrixTranspose(&clipBuffer->Data.invWorld, &invWorld);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);

	if (ShaderManager::Get()->GetOther() != depth&& ShaderManager::Get()->GetOther() != shadow)
	{
		if(ShaderManager::Get()->GetOther() == minimap) planeBuffer->Data.isshadow = 0;
		else  planeBuffer->Data.isshadow = 1;

		for (int i = 0; i < 5; i++)
		{
			D3D::GetDC()->PSSetShaderResources(i, 1, &(textures[i]->texture));
		}
		D3D::GetDC()->PSSetShaderResources(5, 1, ShaderManager::Get()->GetResourceView(shadow));
		
		planeBuffer->SetPSBuffer(1);
		clipBuffer->SetVSBuffer(3);

		shader->Render();

		D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	}
	else
	{	
		D3D::GetDC()->PSSetShaderResources(0, 1, ShaderManager::Get()->GetResourceView(depth));
		ShaderManager::Get()->Render(indexCount);
	}
}

void Plane::waterRender()
{
	if (isLoaded && water != NULL) water->Render();
}

void Plane::PostRender(bool& isUse)
{
	ImGui::Begin("Enviroment",&isUse);
	brush->brushOn= ImGui::CollapsingHeader("Plane");
	if (brush->brushOn)
	{
		static int scale(8);
		ImGui::InputInt("PlaneScale",&scale);
		if (ImGui::Button("ChangeScale", ImVec2(80, 20))) ChangeScale(scale);ImGui::SameLine();
		if (ImGui::Button("WriteMap", ImVec2(80, 20))) SaveMapDialog();ImGui::SameLine();
		if (ImGui::Button("ReadMap", ImVec2(80, 20))) OpenMapDialog();

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
	if(file==L"") return;
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
	if (!isLoaded) return;
		
		brush->Update(origin,direction);
		dot = brush->position;

		D3DXVECTOR4 size = D3DXVECTOR4((float)((dot.x - position.x)*pow(2, number) / width), 
		(float)((dot.y - position.z) * pow(2, number) / height ), (float)brush->type, (float)brush->size);
		D3DXVECTOR4 point = D3DXVECTOR4((dot.x - position.x), (dot.y - position.z), dot.x, dot.y);
		
		planeBuffer->Data.Size=size;
		planeBuffer->Data.Point=point;

}
void Plane::LoadHeightMap()
{
	ID3D11Texture2D* texture;
	Texture::Get()->LoadTexture(Contents+L"./Textures/heightmap.jpg", &texture);

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	UINT* colors = new UINT[desc.Width * desc.Height];
	Texture::Get()->LoadPixel(texture, &colors);

	width =  1+desc.Width;
	height = 1+desc.Height;

	heightData = new BYTE[width * height];
	int count = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			BYTE r;
			if (x == width - 1 || z == height - 1)
			{
				r = 0;
			}
			else
			{
				UINT color = colors[count++];
				r = ((color & 0x00FF0000) >> 16);
			}
			heightData[z * width + x] = r;
		}
	}

	width--;
	height--;

	position = D3DXVECTOR3(-(float)width / 2, 0, -(float)height / 2);
}

void Plane::CreateBuffer()
{
	vertexCount = (width + 1) * (height + 1);

	vertex = new VertexType[vertexCount];
	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			int index = (width + 1) * z + x;

			vertex[index].position.x = (float)x + position.x;
			//vertex[index].position.y = (float)heightData[index] * size / 7.5f-5+ position.y;
			vertex[index].position.y = 0.0f;
			vertex[index].position.z = (float)z + position.z;

			vertex[index].uv.x = (float)(x);// (float)width;
			vertex[index].uv.y = (float)(z);// (float)height;

			vertex[index].normal = D3DXVECTOR3(0, 0, 0);
			vertex[index].color = D3DXVECTOR4(0, 0, 0, 0);
			vertex[index].lerp = D3DXVECTOR4(0, 0, 0, 0);
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

	for (int i = 0; i<5; i++)
	textures[i] = new Textures();
	sample = new Textures();

	SetTexture(0, Contents + L"./Textures/Diffuse/Dirt.png");

	if(brush!=NULL) SAFE_DELETE(brush);
	brush = new Brush(this);

	if (water != NULL) SAFE_DELETE(water);
	water = new Water(width,height);

	if(quadTree!=NULL) SAFE_DELETE(quadTree);
	quadTree = new ZQuadTree(width, height);
	quadTree->Build(vertex);

	clipBuffer->Data.Clip.w=-water->GetDepth();
}


void Plane::CreateNormalData()
{
	for (UINT i = 0; i < vertexCount; i++)
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
	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*vertexCount, 0);
}
void Plane::ChangeScale(int num)
{
	isLoaded=true;
	number=num;
	width = height=(UINT)pow(2,num);
	position=D3DXVECTOR3(-(float)width / 2, 0, -(float)height / 2);
	CreateBuffer();
}

void Plane::OpenMapDialog(wstring file)
{
	if (file.length() < 1)
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		function<void(wstring)> func = bind(&Plane::OpenMapDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::BinMapFilter, BinModels, func, desc.Handle);
	}
	else
	{
		loadThread = new thread([&](wstring fileName)
		{
			isLoaded = false;
			ReadMap(fileName);
			isLoaded = true;
		}, file);
	}
}

void Plane::SaveMapDialog(wstring file)
{
	if (file.length() < 1)
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		function<void(wstring)> func = bind(&Plane::SaveMapDialog, this, placeholders::_1);
		Path::SaveFileDialog(L"", Path::BinMapFilter, BinModels, func, desc.Handle);
	}
	else
	{
		WriteMap(file);
	}
}

void Plane::WriteMap(wstring file)
{
	BinaryWriter* w = new BinaryWriter();
	wstring temp = file;

	w->Open(temp);
	{
		w->UInt(width);
		w->UInt(height);
		w->UInt(number);
		w->Vector3(position);

		w->UInt(vertexCount);
		for (UINT i = 0;i < vertexCount;i++)
		{
			w->Vector3(vertex[i].position);
			w->Vector2(vertex[i].uv);
			w->Vector3(vertex[i].normal);
			w->Vector4(vertex[i].color);
			w->Vector4(vertex[i].lerp);
		}
		w->UInt(indexCount);
		for (UINT i = 0;i < indexCount;i++)w->UInt(index[i]);

		for(UINT i=0;i<5;i++)w->String(String::WStringToString(textures[i]->file));
	}
	w->Close();

	SAFE_DELETE(w);
}

void Plane::ReadMap(wstring file)
{
	BinaryReader* r = new BinaryReader();
	wstring temp = file;
	SAFE_DELETE(index);
	SAFE_DELETE(vertex);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	UINT count = 0;
	r->Open(temp);
	{
		width=r->UInt();
		height=r->UInt();
		number=r->UInt();
		position=r->Vector3();

		CreateBuffer();

		vertexCount=r->UInt();
		for (UINT i = 0;i < vertexCount;i++)
		{
			vertex[i].position=r->Vector3();
			vertex[i].uv=r->Vector2();
			vertex[i].normal=r->Vector3();
			vertex[i].color = r->Vector4();
			vertex[i].lerp =r->Vector4();
		}
		UINT dummyCount=r->UInt();
		UINT* dummy=new UINT[dummyCount];
		for (UINT i = 0;i < dummyCount;i++)dummy[i]=r->UInt();

		for (UINT i = 0;i<5;i++)
		{
			SetTexture(i,String::StringToWString(r->String()) );
		}

		D3D::GetDC()->UpdateSubresource
		(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*vertexCount, 0);

		D3D::GetDC()->UpdateSubresource
		(indexBuffer, 0, NULL, index, sizeof(UINT)*indexCount, 0);
	}
	r->Close();
	SAFE_DELETE(r);
}

void Plane::Frustum()
{
	UINT i[4];
	BOOL b[4];
	indexCount = 0;

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			i[0] = (width + 1) * z + x;
			i[1] = (width + 1) * (z + 1) + x;
			i[2] = (width + 1) * z + x + 1;
			i[3] = (width + 1) * (z + 1) + x + 1;

			for (int num = 0;num<4;num++)
				b[num] = ZFrustum::Get()->IsIn(&vertex[i[num]].position);

			if (b[0] | b[1] | b[2])
			{
				index[indexCount++] = i[0];
				index[indexCount++] = i[1];
				index[indexCount++] = i[2];
			}
			if (b[2] | b[1] | b[3])
			{
				index[indexCount++] = i[2];
				index[indexCount++] = i[1];
				index[indexCount++] = i[3];
			}
		}//for(x)
	}//for(z)

	D3D::GetDC()->UpdateSubresource(indexBuffer, 0, NULL, index, sizeof(UINT)*indexCount, 0);
}

float Plane::GetHeight(float x, float y)
{
	x+=width/2;	y+=height/2;
	int index = (width + 1) * (int)y + (int)x;
	
	if(index<0||(UINT)index>=vertexCount|| !isLoaded) return 0.0f;	else return vertex[index].position.y;
}