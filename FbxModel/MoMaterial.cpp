#include "../stdafx.h"
#include "MoMaterial.h"
#include "../Model/ModelMaterial.h"
#include "../Model/ModelBuffer.h"

MoMaterial::MoMaterial(int number, FbxSurfaceMaterial * material)
	: number(number)
	, ambient(0, 0, 0, 1), diffuse(0, 0, 0, 1), specular(0, 0, 0, 1), emissive(0, 0, 0, 1)
	, shininess(0.0f)
	, ambientFile(""), diffuseFile(""), specularFile(""), emissiveFile("")
{
	name = material->GetName();
	

	ambient = GetProperty(material, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, &ambientFile);
	diffuse = GetProperty(material, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, &diffuseFile);
	specular = GetProperty(material, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &specularFile);
	emissive = GetProperty(material, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &emissiveFile);

	FbxProperty shininessProp = material->FindProperty(FbxSurfaceMaterial::sShininess);
	if (shininessProp.IsValid() == true)
		shininess = (float)shininessProp.Get<FbxDouble>();

	shaderFile = "0"; //0 문자로 쉐이더가 없을경우 기본 쉐이더로 처리
}

MoMaterial::~MoMaterial()
{
}

void MoMaterial::Write(BinaryWriter * w)
{
	w->UInt(number);
	w->String(name);

	w->String(shaderFile);

	w->Color4f(ambient);
	w->Color4f(diffuse);
	w->Color4f(specular);
	w->Color4f(emissive);
	w->Float(shininess);

	w->String(ambientFile);
	w->String(diffuseFile);
	w->String(specularFile);
	w->String(emissiveFile);
}

D3DXCOLOR MoMaterial::GetProperty(FbxSurfaceMaterial * material, const char * name, const char * factorName, string * textureName)
{
	D3DXCOLOR color(0, 0, 0, 1);

	FbxProperty prop = material->FindProperty(name);
	FbxProperty factorProp = material->FindProperty(factorName);
	if (prop.IsValid() == true && factorProp.IsValid() == true)
	{
		FbxDouble3 temp = prop.Get<FbxDouble3>();
		double factor = factorProp.Get<FbxDouble>();

		color.r = (float)(temp[0] * factor);
		color.g = (float)(temp[1] * factor);
		color.b = (float)(temp[2] * factor);
	}

	if (prop.IsValid() == true)
	{
		UINT count = prop.GetSrcObjectCount<FbxFileTexture>();
		if (count > 0)
		{
			FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();
			if (texture != NULL)
				*textureName = (const char *)texture->GetFileName();
		}
		else
		{
			*textureName = "";
		}
	}

	return color;
}
