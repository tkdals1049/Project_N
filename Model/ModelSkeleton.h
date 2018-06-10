#pragma once

class ModelSkeletonBone;
class ModelAnimationController;
class ModelSkeleton
{
public:
	ModelSkeleton();
	ModelSkeleton(const ModelSkeleton& otherSkeleton);
	~ModelSkeleton();

	D3DXMATRIX GetWeapon(string name);

	void AddSkeletonBone(ModelSkeletonBone* skeletonBone);
	ModelSkeletonBone* FindBone(string boneName);
	int FindBoneIndex(string boneName);

	D3DXMATRIX& GetBoneAnimationTransform(int boneIdx);

	ModelSkeletonBone* GetSkeletonBone(int index);
	const vector<pair<string, ModelSkeletonBone *>>& GetSkeletonBones() { return skeletonBones; }

	D3DXMATRIX* GetSkinTransforms() { return skinTransforms; }

	int GetBoneCount() { return boneCount; }

	void AddAnimationFromOtherSkeleton(string srcAnimName, string dstAnimName, ModelSkeleton& srcSkeleton);

	void UpdateAnimation(ModelAnimationController* animationController, UINT root=0, int range = 0);

	static void Write(BinaryWriter * w, ModelSkeleton * skeleton);
	static void Read(BinaryReader * r, ModelSkeleton * skeleton);

private:
	friend class MoSkeleton;

	void BuildBoneTransforms(ModelAnimationController* animationController, string animationName, UINT root=0, int range=0);


	int boneCount;

	typedef pair<string, ModelSkeletonBone *> Pair;
	vector<Pair> skeletonBones;

	D3DXMATRIX* skinTransforms; /// 최종적으로 Vertex에 적용될 Transform Matrix 배열
	D3DXMATRIX* boneAnimationTransforms; /// 각 Bone의 Transform Matrix 배열

};
