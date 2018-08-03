#pragma once

class Model;
class Boss
{
public:
	Boss();
	~Boss();

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
	void AddWeaponVector(Model* model, string weaponName, Model* weaponFile);


	void Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
private:
	bool different;
	class Camera* camera;
	
	Model* model;
	typedef pair<string, Model*> Weapon;
	vector<Weapon> weapon;

	D3DXVECTOR2 dot;
	bool OnModel;
	float a;
	string SaveFile;
	float c_temp1[3];
	float c_temp2[3];
	float c_temp3[3];

	const char** skeletonList;

};

