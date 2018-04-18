#pragma once
#include "../Renders/ShaderBuffer.h"


class ModelBuffer : public ShaderBuffer
{
public:
	ModelBuffer() : ShaderBuffer(&data, sizeof(Data))
	{
		data.ambient = D3DXCOLOR(0, 0, 0, 1);
		data.diffuse = D3DXCOLOR(0, 0, 0, 1);
		data.specular = D3DXCOLOR(0, 0, 0, 1);
		data.emissive = D3DXCOLOR(0, 0, 0, 1);
	}

	D3DXCOLOR GetAmbient() { return data.ambient; }
	D3DXCOLOR GetDiffuse() { return data.diffuse; }
	D3DXCOLOR GetSpecular() { return data.specular; }
	D3DXCOLOR GetEmissive() { return data.emissive; }

	void SetAmbient(D3DXCOLOR& color) { data.ambient = color; }
	void SetDiffuse(D3DXCOLOR& color) { data.diffuse = color; }
	void SetSpecular(D3DXCOLOR& color) { data.specular = color; }
	void SetEmissive(D3DXCOLOR& color) { data.emissive = color; }

	struct Data
	{
		D3DXCOLOR ambient;
		D3DXCOLOR diffuse;
		D3DXCOLOR specular;
		D3DXCOLOR emissive;
	};

private:
	Data data;
};
