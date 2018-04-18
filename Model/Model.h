#pragma once

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
	D3DXMATRIX GetWeaponWorld(string weapon);

	void Update();
	void Render();

	void AnotherUpdate();
	void AddAni(string file, string mode="", float speed=1.0f, int root=0);
	void AniChage(int num);
	void UpdateAnimation();
	void ProcessAnimations();
	void ProcessAnimation(FbxNode * node, string takeName, float frameRate, float start, float stop);

	bool Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void Reset();
	void SetWorld();
	void SetWorld(D3DXMATRIX & world);
	void AddAnimationKeyFrames(ModelAnimationKeyFrames * animationKeyFrames);
	void GetFileName(string file);
	string GetFile() { return file; }

	void SetPosition(D3DXVECTOR3& position);
	D3DXVECTOR3 GetPosition();
	void SetRotate(D3DXVECTOR3& rotate);
	D3DXVECTOR3 GetRotate();
	void SetScale(D3DXVECTOR3& scale);
	D3DXVECTOR3 GetScale();

	D3DXMATRIX GetAbsoluteTransformFromCurrentTake(FbxNode * node, FbxTime time);

private:
	struct Ani
	{
		string mode;
		UINT num;
		float speed;
		UINT root;

		Ani()
		{
			mode="";
			num=0;
			speed=1.0f;
			root=0;
		}
	};
	friend class MoModel;

	FbxManager* manager;
	FbxScene* scene;
	FbxGeometryConverter* converter;
	FbxImporter* importer;

	D3DXMATRIX matGeometricOffset;
	D3DXMATRIX world;
	D3DXMATRIX weaponWorld;
	string file;

	D3DXVECTOR3 minp,maxp,origin;
	D3DXVECTOR3 position, rotate, scale;
	Camera* camera;

	vector<ModelMaterial *> materials;
	vector<ModelMesh *> meshes;
	ModelSkeleton* skeleton;
	map<string, ModelAnimationKeyFrames*> animationKeyFrames;
	map<string, ModelAnimation*> animations;
	
	vector<Ani> ani;
	ModelAnimationController* animationController;

	BoneBuffer* boneBuffer;

};