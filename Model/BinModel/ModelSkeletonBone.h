#pragma once

class ModelAnimationKeyFrames;
class ModelSkeletonBone
{
public:
	ModelSkeletonBone();
	ModelSkeletonBone(string name, int parentBoneIndex);
	ModelSkeletonBone(const ModelSkeletonBone& otherSkeletonBone);
	~ModelSkeletonBone();

	void AddAnimationKeyFrames(ModelAnimationKeyFrames* keyFrames);

	void SetBindPoseTransform(const D3DXMATRIX& matBindPoseTransform);
	void SetBoneReferenceTransform(const D3DXMATRIX& matBoneReferenceTransform);

	ModelAnimationKeyFrames* GetAnimationKeyFrames(string animationName);

	static void Write(BinaryWriter * w, ModelSkeletonBone * bone);
	static void Read(BinaryReader * r, ModelSkeletonBone * bone);

	const D3DXMATRIX& GetBindPoseTransform()
	{
		return matBindPoseTransform;
	}

	const D3DXMATRIX& GetInvBindPoseTransform()
	{
		return matInvBindPoseTransform;
	}

	const D3DXMATRIX& GetBoneReferenceTransform()
	{
		return matBoneReferenceTransform;
	}

	const D3DXMATRIX& GetInvBoneReferenceTransform()
	{
		return matInvBoneReferenceTransform;
	}

	string GetName() { return name; }
	int GetParentBoneIndex() { return parentBoneIndex; }

private:
	friend class MoSkeletonBone;
	string name;

	D3DXMATRIX matBindPoseTransform;
	D3DXMATRIX matInvBindPoseTransform;
	D3DXMATRIX matBoneReferenceTransform;
	D3DXMATRIX matInvBoneReferenceTransform;

	int parentBoneIndex;

	map<string, ModelAnimationKeyFrames *> animationKeyFrames;
};