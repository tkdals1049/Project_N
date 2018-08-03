#include "../../stdafx.h"
#include "ModelMaterial.h"
#include "ModelBuffer.h"

ModelMaterial::ModelMaterial()
	: number(-1)
	, ambientFile(""), emissiveFile(""), diffuseFile(""), specularFile("")
	, ambientView(NULL), diffuseView(NULL), specularView(NULL), emissiveView(NULL)
	, shader(NULL), shaderFile(String::WStringToString(Shaders) + "Model.hlsl")
{
	shader = new Shader(String::StringToWString(shaderFile));
	shaderBuffer = new ModelBuffer();
}

ModelMaterial::~ModelMaterial()
{
	SAFE_RELEASE(ambientView);
	SAFE_RELEASE(emissiveView);
	SAFE_RELEASE(diffuseView);
	SAFE_RELEASE(specularView);

	SAFE_DELETE(shaderBuffer);
	SAFE_DELETE(shader);
}

void ModelMaterial::OneMore()
{
	SetAmbientTexture(ambientFile);
	SetDiffuseTexture(diffuseFile);
	SetSpecularTexture(specularFile);
	SetEmissiveTexture(emissiveFile);
}

void ModelMaterial::CreateTexture(string file, ID3D11ShaderResourceView ** view)
{
	DWORD fileValue = GetFileAttributesA(file.c_str());
	if (fileValue < 0xFFFFFFFF)
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFileA
		(
			D3D::GetDevice()
			, file.c_str()
			, NULL
			, NULL
			, view
			, NULL
		);
		assert(SUCCEEDED(hr));
	}
}
/////////////////////////////////////////////////////////////////////////////////////

void ModelMaterial::Write(BinaryWriter * w, ModelMaterial * modelMaterial)
{
	w->UInt(modelMaterial->number);
	w->String(modelMaterial->name);

	w->String(modelMaterial->shaderFile);

	ModelBuffer* buffer = modelMaterial->shaderBuffer;
	w->Color4f(buffer->GetAmbient());
	w->Color4f(buffer->GetDiffuse());
	w->Color4f(buffer->GetSpecular());
	w->Color4f(buffer->GetEmissive());
	w->Float(0);

	w->String(modelMaterial->ambientFile);
	w->String(modelMaterial->diffuseFile);
	w->String(modelMaterial->specularFile);
	w->String(modelMaterial->emissiveFile);
}

void ModelMaterial::Copy( ModelMaterial * modelMaterial)
{
	modelMaterial->number =number;
	modelMaterial->name = name;

	ModelBuffer* buffer = modelMaterial->shaderBuffer;
	modelMaterial->SetAmbient(buffer->GetAmbient());
	modelMaterial->SetDiffuse(buffer->GetDiffuse());
	modelMaterial->SetSpecular(buffer->GetSpecular());
	modelMaterial->SetEmissive(buffer->GetEmissive());

	modelMaterial->SetAmbientTexture(ambientFile);
	modelMaterial->SetDiffuseTexture(diffuseFile);
	modelMaterial->SetSpecularTexture(specularFile);
	modelMaterial->SetEmissiveTexture(emissiveFile);
}

void ModelMaterial::Read(BinaryReader * r, ModelMaterial * modelMaterial)
{
	modelMaterial->number = r->UInt();
	modelMaterial->name = r->String();

	string shaderFile = r->String();

	D3DXCOLOR color;

	color = r->Color4f();
	modelMaterial->SetAmbient(color);

	color = r->Color4f();
	modelMaterial->SetDiffuse(color);

	color = r->Color4f();
	modelMaterial->SetSpecular(color);

	color = r->Color4f();
	modelMaterial->SetEmissive(color);

	float shininess = r->Float();


	string file = "";

	file = r->String();
	modelMaterial->SetAmbientTexture(file);

	file = r->String();
	modelMaterial->SetDiffuseTexture(file);

	file = r->String();
	modelMaterial->SetSpecularTexture(file);

	file = r->String();
	modelMaterial->SetEmissiveTexture(file);
}

/////////////////////////////////////////////////////////////////////////////////////

void ModelMaterial::SetAmbient(D3DXCOLOR & color)
{
	shaderBuffer->SetAmbient(color);
}

void ModelMaterial::SetDiffuse(D3DXCOLOR & color)
{
	shaderBuffer->SetDiffuse(color);
}

void ModelMaterial::SetSpecular(D3DXCOLOR & color)
{
	shaderBuffer->SetSpecular(color);
}

void ModelMaterial::SetEmissive(D3DXCOLOR & color)
{
	shaderBuffer->SetEmissive(color);
}

void ModelMaterial::SetAmbientTexture(string file)
{
	SAFE_RELEASE(ambientView);

	ambientFile = file;
	CreateTexture(file, &ambientView);
}

void ModelMaterial::SetDiffuseTexture(string file)
{
	SAFE_RELEASE(diffuseView);

	diffuseFile = file;
	CreateTexture(file, &diffuseView);
}

void ModelMaterial::SetSpecularTexture(string file)
{
	SAFE_RELEASE(specularView);

	specularFile = file;
	CreateTexture(file, &specularView);
}

void ModelMaterial::SetEmissiveTexture(string file)
{
	SAFE_RELEASE(emissiveView);

	emissiveFile = file;
	CreateTexture(file, &emissiveView);
}

void ModelMaterial::SetPSBuffer()
{
	if (diffuseView == NULL)
	{
		ID3D11ShaderResourceView* n[1]{ NULL };
		D3D::GetDC()->PSSetShaderResources(0, 1, n);
	}
	else
		D3D::GetDC()->PSSetShaderResources(0, 1, &diffuseView);

	shaderBuffer->SetPSBuffer(1);
	shader->Render();
}
