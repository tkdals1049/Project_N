#pragma once

class Model;
class Actor
{
public:
	Actor();
	~Actor();

	void PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void Update();
	void PostRender(bool& isUse);
	void Render();

	void SetModel(string file);

	void SetDot(D3DXVECTOR2 dot) { this->dot = dot; }
	void SetModel(Model * model);
	Model* GetModel();

	bool GetDifferent() { return different; }
	void SetDifferent(bool different) { this->different = different; }

	void AniChange(string mode);

	void Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void SetAdjust(D3DXVECTOR3 adjust);
	void SetAniPlay(D3DXVECTOR3 ani);
	bool CalSkeleton(ST_OBB * enemy);
protected:
	BOOL CheckOBBCollision(ST_OBB * Box1, ST_OBB * Box2);
	void CalMatrix(ST_OBB* box, D3DXVECTOR3 max, D3DXVECTOR3 min, D3DXVECTOR3 size, D3DXMATRIX mat);
	bool different;
	class Camera* camera;
	
	Model* model;
private:
	D3DXMATRIX world;
	D3DXVECTOR2 dot;
	bool OnModel;
	float a;
	string SaveFile;
	float c_temp1[3];
	float c_temp2[3];
	float c_temp3[3];

	const char** skeletonList;

};
