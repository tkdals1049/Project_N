#pragma once

class Actor
{
public:
	Actor();
	virtual ~Actor();

	void PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void Update();
	void PostRender(bool& isUse);
	void Render();

	void SetModel(string file);
	Model* GetModel(){return model;}
	void SetModel(Model * model);

	void SetDot(D3DXVECTOR2 dot) { this->dot = dot; }
	const string GetMode() { return mode; }
	const string GetPremode() { return Premode; }
	const bool GetisEquip() { return isEquip; }
	const bool GetisAttack() { return isAttack; }
	void SetoffAttack() { isAttack=false; }
	int GetAttackRangeSize(){return attackRange.size(); }
	ST_OBB* GetAttackRange(int num=0) {	return (attackRange.size()!=0 ? attackRange[num] : NULL);}
	void AniChange(string mode)	{model->AniChange(mode);}
	D3DXVECTOR3 GetPosition() { if (model == NULL) return D3DXVECTOR3(0, 0, 0); return model->GetPosition(); }

	void Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void SetAdjust(D3DXVECTOR3 adjust);
	void SetAniPlay(D3DXVECTOR3 ani);
	bool CalSkeleton(ST_OBB * enemy);
	void CalMatrix(ST_OBB* box, D3DXVECTOR3 max, D3DXVECTOR3 min, D3DXVECTOR3 size, D3DXMATRIX mat);

	float hp,st;
protected:
	//class Blood* blood;
	class Blood* blood;
	BOOL CheckOBBCollision(ST_OBB * Box1, ST_OBB * Box2);
	
	Model* model;
	string mode, Premode;
	bool isControl, isEquip, isHeight, isAttack;

	vector<ST_OBB*> attackRange;

private:
	D3DXMATRIX world;
	D3DXVECTOR2 dot;
	bool OnModel;
	string SaveFile;

	float c_temp1[3];
	float c_temp2[3];
	float c_temp3[3];

	const char** skeletonList;
};
