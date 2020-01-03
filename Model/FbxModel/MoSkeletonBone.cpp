#include "stdafx.h"
#include "MoSkeletonBone.h"
#include "MoKeyFrame.h"
#include "../BinModel/ModelSkeletonBone.h"
#include "../BinModel/ModelAnimationKeyFrames.h"

MoSkeletonBone::MoSkeletonBone(string name, int parentBoneIndex)
	: name(name), parentBoneIndex(parentBoneIndex)
{
	D3DXMatrixIdentity(&matBindPoseTransform);
	D3DXMatrixIdentity(&matInvBindPoseTransform);
	D3DXMatrixIdentity(&matBoneReferenceTransform);
	D3DXMatrixIdentity(&matInvBoneReferenceTransform);
}

MoSkeletonBone::~MoSkeletonBone()
{
	for each(Pair temp in animationKeyFrames)
		SAFE_DELETE(temp.second);
}

void MoSkeletonBone::AddAnimationKeyFrames(MoKeyFrame * keyFrames)
{
	animationKeyFrames.push_back(Pair(keyFrames->GetAnimationName(), keyFrames));
}

void MoSkeletonBone::SetBindPoseTransform(const D3DXMATRIX & matBindPoseTransform)
{
	this->matBindPoseTransform = matBindPoseTransform;
	D3DXMatrixInverse(&this->matInvBindPoseTransform, NULL, &this->matBindPoseTransform);
}

void MoSkeletonBone::SetBoneReferenceTransform(const D3DXMATRIX & matBoneReferenceTransform)
{
	this->matBoneReferenceTransform = matBoneReferenceTransform;
	D3DXMatrixInverse(&this->matInvBoneReferenceTransform, NULL, &this->matBoneReferenceTransform);
}

MoKeyFrame * MoSkeletonBone::GetAnimationKeyFrames(string animationName)
{
	for each(Pair temp in animationKeyFrames)
	{
		if (temp.first == animationName)
			return temp.second;
	}
	return NULL;
}

void MoSkeletonBone::Write(BinaryWriter * w)
{
	w->String(name);

	w->Matrix(matBindPoseTransform);
	w->Matrix(matInvBindPoseTransform);
	w->Matrix(matBoneReferenceTransform);
	w->Matrix(matInvBoneReferenceTransform);

	w->UInt(parentBoneIndex);

	UINT count = animationKeyFrames.size();
	w->UInt(count);

	for (size_t i = 0; i < count; i++)
	{
		w->String(animationKeyFrames[i].first);
		animationKeyFrames[i].second->Write(w);
	}
}