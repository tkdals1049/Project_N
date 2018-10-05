#include "../stdafx.h"
#include "LightManager.h"


LightManager* LightManager::instance = nullptr;

LightManager * LightManager::Get()
{
	if (instance == nullptr)
		instance = new LightManager();
	return instance;
}

void LightManager::GetView(D3DXMATRIX * view)
{
	memcpy(view, lightData.lightView, sizeof(D3DXMATRIX));
}

void LightManager::GetProjection(D3DXMATRIX * view)
{
	memcpy(view, lightData.lightProjection, sizeof(D3DXMATRIX));
}

void LightManager::GetLightDirection(D3DXVECTOR3 * out_lightDir)
{
	memcpy(out_lightDir, lightData.lightDirection, sizeof(D3DXVECTOR3));
}

void LightManager::Update()
{
	if (Keyboard::Get()->Press('1')) speed += 0.01f;
	else if (Keyboard::Get()->Press('2')) speed -= 0.01f;
	else if (Keyboard::Get()->Press('3')) speed =0.0f;
	
	angley -= (float)D3DX_PI / 180 * speed;
	
	lightData.lightDirection.x = cosf(angley);
	lightData.lightDirection.y = sinf(angley);

	D3DXMATRIX x, y, matRotation;
	D3DXMatrixRotationZ(&matRotation, angley);
	
	D3DXVec3Normalize(&lightData.lightDirection, &lightData.lightDirection);
	float size = (float)Plane::Get()->GetWidth()*Time::Get()->GetMagn();
	D3DXMatrixLookAtLH(&lightData.lightView, &(position - lightData.lightDirection*size), &(position), &up);
	D3DXMatrixOrthoLH(&lightData.lightProjection, size, size, 0.1f, 1000.0f);
	//D3DXMatrixPerspectiveFovLH(&lightData.lightProjection, D3DX_PI / 4, 1.0f, 0.1f, 1000.0f);

	lightBuffer->SetDirection(lightData.lightDirection*size);
	lightBuffer->SetView(lightData.lightView);
	lightBuffer->SetProjection(lightData.lightProjection);
}

void LightManager::Delete()
{
	SAFE_DELETE(instance);
}

LightManager::LightManager()
{
	lightData.lightDirection=D3DXVECTOR3(0,-1,0);

	anglex = angley = (float)D3DX_PI / 180 * -89;
	speed = 0;
}

LightManager::~LightManager()
{
}
