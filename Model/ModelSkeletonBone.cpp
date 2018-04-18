#include "../stdafx.h"
#include "ModelSkeletonBone.h"
#include "ModelAnimationKeyFrames.h"
#include "../Model/ModelSkeleton.h"
#include "../Model/ModelSkeletonBone.h"
#include "../Model/ModelAnimationKeyFrames.h"

ModelSkeletonBone::ModelSkeletonBone()
{
}

ModelSkeletonBone::ModelSkeletonBone(string name, int parentBoneIndex)
	: name(name), parentBoneIndex(parentBoneIndex)
{
	D3DXMatrixIdentity(&matBindPoseTransform);
	D3DXMatrixIdentity(&matInvBindPoseTransform);
	D3DXMatrixIdentity(&matBoneReferenceTransform);
	D3DXMatrixIdentity(&matInvBoneReferenceTransform);
}

ModelSkeletonBone::ModelSkeletonBone(const ModelSkeletonBone & otherSkeletonBone)
{
	name = otherSkeletonBone.name;

	matBindPoseTransform = otherSkeletonBone.matBindPoseTransform;
	matInvBindPoseTransform = otherSkeletonBone.matInvBindPoseTransform;
	matBoneReferenceTransform = otherSkeletonBone.matBoneReferenceTransform;
	matInvBoneReferenceTransform = otherSkeletonBone.matInvBoneReferenceTransform;

	parentBoneIndex = otherSkeletonBone.parentBoneIndex;

	size_t animationKeyFramesCount = otherSkeletonBone.animationKeyFrames.size();

	map<string, ModelAnimationKeyFrames *>::const_iterator iter;
	for (iter = otherSkeletonBone.animationKeyFrames.begin(); iter != otherSkeletonBone.animationKeyFrames.end(); ++iter)
	{
		string tempName = iter->first;
		ModelAnimationKeyFrames tempKeyFrames = *(iter->second);

		animationKeyFrames.insert({ tempName, new ModelAnimationKeyFrames(tempKeyFrames) });
	}
}

ModelSkeletonBone::~ModelSkeletonBone()
{
	map<string, ModelAnimationKeyFrames *>::iterator iter;
	for (iter = animationKeyFrames.begin(); iter != animationKeyFrames.end(); ++iter)
		SAFE_DELETE(iter->second);
}

void ModelSkeletonBone::AddAnimationKeyFrames(ModelAnimationKeyFrames * keyFrames)
{
	animationKeyFrames.insert({ keyFrames->GetAnimationName(), keyFrames });
}

void ModelSkeletonBone::SetBindPoseTransform(const D3DXMATRIX & matBindPoseTransform)
{
	this->matBindPoseTransform = matBindPoseTransform;
	D3DXMatrixInverse(&this->matInvBindPoseTransform, NULL, &this->matBindPoseTransform);
}

void ModelSkeletonBone::SetBoneReferenceTransform(const D3DXMATRIX & matBoneReferenceTransform)
{
	this->matBoneReferenceTransform = matBoneReferenceTransform;
	D3DXMatrixInverse(&this->matInvBoneReferenceTransform, NULL, &this->matBoneReferenceTransform);
}

ModelAnimationKeyFrames * ModelSkeletonBone::GetAnimationKeyFrames(string animationName)
{
	map<string, ModelAnimationKeyFrames *>::iterator iter;
	for (iter = animationKeyFrames.begin(); iter != animationKeyFrames.end(); ++iter)
	{
		if (iter->first == animationName)
			return iter->second;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////


void ModelSkeletonBone::Write(BinaryWriter * w, ModelSkeletonBone* bone)
{
	w->String(bone->name);

	w->Matrix(bone->matBindPoseTransform);
	w->Matrix(bone->matInvBindPoseTransform);
	w->Matrix(bone->matBoneReferenceTransform);
	w->Matrix(bone->matInvBoneReferenceTransform);

	w->UInt(bone->parentBoneIndex);

	UINT count = bone->animationKeyFrames.size();
	w->UInt(count);

	map<string, ModelAnimationKeyFrames *>::iterator iter;
	for (iter = bone->animationKeyFrames.begin(); iter != bone->animationKeyFrames.end(); ++iter)
	{
		string str = iter->first;
		w->String(str);
		iter->second->Write(w, iter->second);
	}
}

void ModelSkeletonBone::Read(BinaryReader * r, ModelSkeletonBone* bone)
{
	bone->name = r->String();

	bone->matBindPoseTransform = r->Matrix();
	bone->matInvBindPoseTransform = r->Matrix();
	bone->matBoneReferenceTransform = r->Matrix();
	bone->matInvBoneReferenceTransform = r->Matrix();

	bone->parentBoneIndex = r->UInt();

	bone->animationKeyFrames.clear();
	UINT animationKeyFramesCount = r->UInt();

	for (size_t i = 0; i < animationKeyFramesCount; i++)
	{
		string name = r->String();
		ModelAnimationKeyFrames* modelAnimationKeyFrames = new ModelAnimationKeyFrames();
		modelAnimationKeyFrames->Read(r, modelAnimationKeyFrames);
		bone->animationKeyFrames.insert(make_pair(name, modelAnimationKeyFrames));
	}
}

/////////////////////////////////////////////////////////////
