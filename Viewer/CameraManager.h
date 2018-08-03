#pragma once
class CameraManager
{
public:
	static CameraManager* Get();
	static void Delete();

	void SetPlayerCamera(Camera* playerCamera){this->playerCamera= playerCamera ;}
	void SetPosition(D3DXVECTOR3 position){ if (!isAuto)playerCamera->SetPosition(position);}
	void SetRotate(D3DXVECTOR2 rotate){ if (!isAuto)playerCamera->SetRotation(rotate);}

private:
	CameraManager();
	~CameraManager();

	static CameraManager* instance;
	class Camera* playerCamera;

	bool isAuto;
};