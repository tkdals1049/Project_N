#pragma once
#include "./Renders/GlobalBuffers.h"
class LightManager {

public:
	struct LightData 
	{
		D3DXMATRIX lightView;
		D3DXMATRIX lightProjection;
		D3DXVECTOR3 lightDirection;
		float padding;
	};
	static LightManager* Get();

	void SetBuffer(GlobalLightBuffer* lightBuffer) { this->lightBuffer = lightBuffer; }

	void GetView(D3DXMATRIX* view);
	void GetProjection(D3DXMATRIX* view);

	void GetLightDirection(D3DXVECTOR3* out_lightDir);

	LightData* GetLightData() {
		return &lightData;
	}

	void Update();

	void Delete();

private:

	LightManager();
	~LightManager();
	GlobalLightBuffer* lightBuffer;
	LightData lightData;
	static LightManager* instance;

	D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0),forward;
	D3DXVECTOR3 position = D3DXVECTOR3(0, 0, 0);

	float speed;
	float anglex, angley;
};