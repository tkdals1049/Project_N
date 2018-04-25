#pragma once

class FbxLoder;
class Model;
class Camera;
class Map
{
public:
	Map();
	~Map();

	void CreateFBX(wstring file);
	void PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void Update();
	void PostRender(bool& isUse);
	void Render();

	void SetDot(D3DXVECTOR2 dot){this->dot=dot;}
	void SetModel(Model * model);
	void SetModel();
	Model* GetModel();
	bool GetDifferent(){return different;}
	void SetDifferent(bool different){this->different=different;}
	void AddWeaponVector(Model* model,string weaponName,Model* weaponFile);

	void SetAttitude(Model * model);
	Model* GetAttitude();

	void Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
private:
	bool different;
	Camera* camera;
	vector<Model*> models;

	typedef pair<string, Model*> Weapon;
	map<Model*,vector<Weapon>> weapons;
	
	Model* attitude;
	Model* another;
	D3DXVECTOR2 dot;
	bool OnModel;
	float a;
	string SaveFile;
	float c_temp1[3];
	float c_temp2[3];
	float c_temp3[3];

	const char** skeletonList;

};

