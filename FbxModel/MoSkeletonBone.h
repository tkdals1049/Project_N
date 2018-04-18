#pragma once

class ModelSkeletonBone;
class MoKeyFrame;
class MoSkeletonBone
{
public:
	MoSkeletonBone(){};
	MoSkeletonBone(string name, int parentBoneIndex);
	~MoSkeletonBone();

	void AddAnimationKeyFrames(MoKeyFrame* keyFrames);

	void SetBindPoseTransform(const D3DXMATRIX& matBindPoseTransform);
	void SetBoneReferenceTransform(const D3DXMATRIX& matBoneReferenceTransform);

	MoKeyFrame* GetAnimationKeyFrames(string animationName);

	void Write(BinaryWriter * w);

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
	typedef pair<string, MoKeyFrame *> Pair;

	string name;

	D3DXMATRIX matBindPoseTransform;
	D3DXMATRIX matInvBindPoseTransform;
	D3DXMATRIX matBoneReferenceTransform;
	D3DXMATRIX matInvBoneReferenceTransform;

	int parentBoneIndex;

	vector<pair<string, MoKeyFrame *>> animationKeyFrames;
};