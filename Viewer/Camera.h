#pragma once

class Camera
{
public:
	virtual ~Camera();

	void GetPosition(D3DXVECTOR3* vec)
	{
		*vec = position;
	}

	void SetPosition(float x, float y)
	{
		SetPosition(D3DXVECTOR3(x, y, 0.0f));
	}

	void SetPosition(float x, float y, float z)
	{
		SetPosition(D3DXVECTOR3(x, y, z));
	}

	void SetPosition(D3DXVECTOR3 vec)
	{ 
		this->position = vec;
	}

	void GetRotation(D3DXVECTOR2* vec)
	{
		*vec = rotation;
	}

	void SetRotation(D3DXVECTOR2 vec)
	{
		this->rotation = vec;
	}

	void GetRotationDegree(D3DXVECTOR2* vec)
	{
		*vec = rotation * 180.0f / (float)D3DX_PI;
	}

	void SetRotationDegree(D3DXVECTOR2 vec)
	{
		this->rotation = vec * (float)D3DX_PI / 180.0f;
		UpdateRotation();
	}

	void GetMatrix(D3DXMATRIX* mat);
	void SetMatrix(D3DXMATRIX * mat);

	virtual void Update() = 0;

protected:
	Camera();

	virtual void UpdateView();
	virtual void UpdateRotation(D3DXVECTOR2 quike=D3DXVECTOR2(0,0));

	D3DXVECTOR3 GetForward(){return forward;}
	D3DXVECTOR3 GetRight() { return right; }
	D3DXVECTOR3 GetUp() { return up; }
	
private:
	D3DXVECTOR3 position;
	D3DXVECTOR2 rotation;

	D3DXVECTOR3 forward;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;

	D3DXMATRIX matRotation;
	D3DXMATRIX matView;
};
