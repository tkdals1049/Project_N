#pragma once
struct ST_OBB // OBB구조체
{
	D3DXVECTOR3 vCenterPos; // 상자 중앙의 좌표
	D3DXVECTOR3 vAxisDir[3]; //상자에 평행한 세 축의 단위벡터
	float  fAxisLen[3]; // 상자의 평행한 세 축의 길이 fAxisLen[n]은 vAxisDir[n]에 각각 대응한다.
	ST_OBB()
	{
		vCenterPos = D3DXVECTOR3(0, 0, 0);
		vAxisDir[0] = D3DXVECTOR3(0, 0, 0);
		vAxisDir[1] = D3DXVECTOR3(0, 0, 0);
		vAxisDir[2] = D3DXVECTOR3(0, 0, 0);
		fAxisLen[0] = 0.0f;
		fAxisLen[1] = 0.0f;
		fAxisLen[2] = 0.0f;
	}

	ST_OBB(D3DXVECTOR3 vCenterPos, D3DXVECTOR3 vAxisDir[3], float  fAxisLen[3])
	{
		this->vCenterPos = vCenterPos;
		this->vAxisDir[0] = vAxisDir[0];
		this->vAxisDir[1] = vAxisDir[1];
		this->vAxisDir[2] = vAxisDir[2];
		this->fAxisLen[0] = fAxisLen[0];
		this->fAxisLen[1] = fAxisLen[1];
		this->fAxisLen[2] = fAxisLen[2];

	}
};

class ColorBuffer : public ShaderBuffer
{
public:
	ColorBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.Color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	}

	struct Struct
	{
		D3DXCOLOR Color;
	};

	Struct Data;
};

class ModelSkeleton;
class ModelMaterial;
class ModelMesh;
class ModelAnimation;
class ModelAnimationController;
class ModelAnimationKeyFrames;
class BoneBuffer;
class Model
{
public:

	Model();
	~Model();

	D3DXMATRIX GetGeometricOffset() { return matGeometricOffset; }
	D3DXMATRIX GetWorld() { return world; }
	D3DXMATRIX GetWeaponWorld() { return weaponWorld;}
	D3DXMATRIX GetWeaponWorld(string weapon);
	
	ModelSkeleton* GetSkeleton(){return skeleton;}
	ModelAnimationController* GetAnimationController() { return animationController; }

	void MakeMaterial(int num, string diffusePath);
	void MeshMaterial();
	vector<ModelMaterial *> GetMaterialVector(){return materials;}

	ModelMaterial* GetMatchMaterial(UINT number);
	ModelMaterial* GetMaterial(UINT number){return materials[number];}

	UINT GetMaterialCount() { return materials.size(); }
	void SetMaterial(ModelMaterial* material);
	void ClearMaterial();

	void Update();
	void Render();

	void AnotherUpdate();
	void AddAni(string file, string mode="");
	void AniChange(int num);
	void AniChange(string mode);
	void UpdateAnimation();
	void ProcessAnimations(string mode);
	void ProcessAnimation(FbxNode * node, string takeName, float frameRate, float start, float stop);

	bool Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void Reset();
	void SetWorld();
	void SetWorld(D3DXMATRIX & world);
	void SetGeometricOffset(D3DXMATRIX& world);
	void AddAnimationKeyFrames(ModelAnimationKeyFrames * animationKeyFrames);
	void GetFileName(string file);
	string GetFile() { return file; }

	void SetPosition(D3DXVECTOR3& position);
	D3DXVECTOR3 GetPosition();
	void SetRotate(D3DXVECTOR3& rotate);
	D3DXVECTOR3 GetRotate();
	void SetScale(D3DXVECTOR3& scale);
	D3DXVECTOR3 GetScale();
	void SetAdjust(D3DXVECTOR3& adjust);
	D3DXVECTOR3 GetAdjust();
	void SetAniPlay(int root,int range,float speed);

	D3DXVECTOR3 GetMaxP() { return maxp; }
	D3DXVECTOR3 GetMinP() { return minp; }

	D3DXMATRIX GetAbsoluteTransformFromCurrentTake(FbxNode * node, FbxTime time);
	void isDamage();

private:
	friend class MoModel;

	FbxManager* manager;
	FbxScene* scene;
	FbxGeometryConverter* converter;
	FbxImporter* importer;

	D3DXMATRIX matGeometricOffset;
	D3DXMATRIX world;
	D3DXMATRIX weaponWorld;
	string file;

	D3DXVECTOR3 minp,maxp,origin,adjust;
	D3DXVECTOR3 position, rotate, scale;
	Camera* camera;

	vector<ModelMaterial *> materials;
	vector<ModelMesh *> meshes;
	ModelSkeleton* skeleton;
	map<string, ModelAnimationKeyFrames*> animationKeyFrames;
	map<string, ModelAnimation*> animations;
	
	ModelAnimationController* animationController;

	BoneBuffer* boneBuffer;
	ColorBuffer* colorBuffer;
	float damageColor,damageTime;

	UINT root;
	UINT range;
	float speed;
};