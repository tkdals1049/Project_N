#pragma once

class ModelSkeleton;
class ModelSkeletonBone;
class MoSkeletonBone;
class MoSkeleton
{
public:
	MoSkeleton();
	~MoSkeleton();

	D3DXMATRIX GetWeapon(string name);

	void GetRoot(UINT root) { this->root = root; }
	void AddSkeletonBone(MoSkeletonBone* skeletonBone);
	MoSkeletonBone* FindBone(string boneName);
	int FindBoneIndex(string boneName);

	MoSkeletonBone* GetSkeletonBone(int index);
	const vector<pair<string, MoSkeletonBone *>>& GetSkeletonBones()
	{
		return skeletonBones;
	}

	D3DXMATRIX* GetSkinTransforms()
	{
		return skinTransforms;
	}

	int GetBoneCount() { return boneCount; }

	void Write(BinaryWriter * w);

private:
	typedef pair<string, MoSkeletonBone *> Pair;
	vector<Pair> skeletonBones;

	D3DXMATRIX* skinTransforms;
	D3DXMATRIX* boneAnimationTransforms;
	UINT root;
	int boneCount;
};
