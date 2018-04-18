#pragma once

class ModelSkeleton;
class MoSkeleton;
class MoKeyFrame;
class MoMaterial;
class MoModelMesh;
class MoBoneWeights;
class MoAnimationController;
class Model;
class MoModel
{
private:
	friend class MoLoader;

	MoModel(string file);
	~MoModel();

	void PushMaterial(MoMaterial* material);
	void PushVertex
	(
		MoMaterial* material
		, D3DXVECTOR3& position
		, D3DXVECTOR3& normal
		, D3DXVECTOR2& uv
		, const MoBoneWeights& boneWeights
	);

	MoSkeleton* GetSkeleton();
	void SetSkeleton(MoSkeleton* skeleton){this->skeleton=skeleton;}

	void AddAnimationKeyFrames(MoKeyFrame * animationKeyFrames);

	void Write(string file);
	static void Read(string file, Model** model);
	static void Write(string file, Model* model);

	string file;
	D3DXMATRIX geometricOffset;

	MoSkeleton* skeleton;
	MoAnimationController* animationController;

	typedef pair<string, MoKeyFrame *> Pair;
	vector<Pair> animationKeyFrames;

	vector<MoMaterial *> materials;
	vector<MoModelMesh* > meshes;
};