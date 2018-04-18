#pragma once

class ModelBuffer;
class ModelMaterial
{
public:
	bool CanRender() { return shader != NULL; }

	UINT GetNumber() { return number; }
	string GetName() { return name; }

	static void Write(BinaryWriter * w, ModelMaterial * modelMaterial);
	static void Read(BinaryReader * r, ModelMaterial * modelMaterial);

	void Copy(ModelMaterial * modelMaterial);

	void SetAmbient(D3DXCOLOR& color);
	void SetDiffuse(D3DXCOLOR& color);
	void SetSpecular(D3DXCOLOR& color);
	void SetEmissive(D3DXCOLOR& color);

	string GetAmbientFile() { return ambientFile; }
	string GetDiffuseFile() { return diffuseFile; }
	string GetSpecularFile() { return specularFile; }
	string GetEmissiveFile() { return emissiveFile; }

	void SetAmbientTexture(string file);
	void SetDiffuseTexture(string file);
	void SetSpecularTexture(string file);
	void SetEmissiveTexture(string file);

	void SetPSBuffer();
	void OneMore();

private:
	friend class MoModel;
	friend class MoMaterial;
	friend class Model;

	ModelMaterial();
	~ModelMaterial();

	void CreateTexture(string file, ID3D11ShaderResourceView** view);
	
	UINT number;
	string name;

	Shader* shader;
	ModelBuffer* shaderBuffer;
	string shaderFile;

	string ambientFile;
	string diffuseFile;
	string specularFile;
	string emissiveFile;

	ID3D11ShaderResourceView* ambientView;
	ID3D11ShaderResourceView* diffuseView;
	ID3D11ShaderResourceView* specularView;
	ID3D11ShaderResourceView* emissiveView;
};