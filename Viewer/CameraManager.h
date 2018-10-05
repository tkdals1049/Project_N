#pragma once

#include "Perspective.h"
class CameraManager
{
public:
	static CameraManager* Get();
	static void Delete();

	void SetPlayerCamera(Camera* playerCamera){this->playerCamera= playerCamera ;}
	void SetViewProjectionBuffer(ViewProjectionBuffer* VPbuffer) { this->VPbuffer = VPbuffer; }
	void SetPerspective(Perspective* perspective) { this->perspective = perspective; }

	void SetPosition(D3DXVECTOR3 position){ if (!isAuto)playerCamera->SetPosition(position);}
	void SetRotate(D3DXVECTOR2 rotate){ if (!isAuto)playerCamera->SetRotation(rotate);}
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR2 GetRotate();
	D3DXMATRIX GetView();
	D3DXMATRIX GetProj();
	D3DXVECTOR3 GetPos() { return CameraPos; }

	D3DXMATRIX RenderReflection(float depth = 0, D3DXVECTOR3 normal = D3DXVECTOR3(0, 1, 0));
	void SetViewProjection(D3DXMATRIX view, D3DXMATRIX projection);
	void DefaultCamera();
private:
	CameraManager();
	~CameraManager();

	static CameraManager* instance;

	class Camera* playerCamera;
	class ViewProjectionBuffer* VPbuffer;
	class Perspective* perspective;

	D3DXVECTOR3 RePosition,CameraPos;
	D3DXVECTOR2 ReRotation;
	bool isAuto, isRe;
};