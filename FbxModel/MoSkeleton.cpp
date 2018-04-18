#include "../stdafx.h"
#include "MoSkeleton.h"
#include "MoSkeletonBone.h"
#include "MoKeyFrame.h"
#include "../Model/ModelSkeleton.h"
#include "../Model/ModelSkeletonBone.h"
#include "../Model/ModelAnimationKeyFrames.h"

MoSkeleton::MoSkeleton()
{
	skinTransforms = NULL;
	boneAnimationTransforms = NULL;
	root = 0;
	boneCount = 0;
}

MoSkeleton::~MoSkeleton()
{
	for each(Pair temp in skeletonBones)
		SAFE_DELETE(temp.second);

	SAFE_DELETE_ARRAY(boneAnimationTransforms);
	SAFE_DELETE_ARRAY(skinTransforms);
}

D3DXMATRIX MoSkeleton::GetWeapon(string name)
{
	return boneAnimationTransforms[FindBoneIndex(name)];
}

void MoSkeleton::AddSkeletonBone(MoSkeletonBone * skeletonBone)
{
	skeletonBones.push_back(Pair(skeletonBone->GetName(), skeletonBone));

	boneCount++;
}

MoSkeletonBone * MoSkeleton::FindBone(string boneName)
{
	for each(Pair temp in skeletonBones)
	{
		if (boneName == temp.first)
			return temp.second;
	}

	return NULL;
}

int MoSkeleton::FindBoneIndex(string boneName)
{
	int index = 0;
	for each(Pair temp in skeletonBones)
	{
		if (boneName == temp.first)
			return index;

		index++;
	}

	return -1;
}

MoSkeletonBone * MoSkeleton::GetSkeletonBone(int index)
{
	return skeletonBones[index].second;
}

void MoSkeleton::Write(BinaryWriter * w)
{
	w->UInt(boneCount);

	UINT skeletonBonesCount = skeletonBones.size();
	w->UInt(skeletonBonesCount);
	for (size_t i = 0; i < skeletonBonesCount; i++)
	{
		w->String(skeletonBones[i].first);
		(skeletonBones[i].second)->Write(w);
	}

	w->Matrix(*skinTransforms);
	w->Matrix(*boneAnimationTransforms);

}
