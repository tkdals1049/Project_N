#include "../../stdafx.h"
#include "ModelSkeleton.h"
#include "ModelSkeletonBone.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"
#include "ModelAnimationKeyFrames.h"

ModelSkeleton::ModelSkeleton()
{
	skinTransforms = NULL;
	boneAnimationTransforms = NULL;

	boneCount = 0;
	min = 0;
}

ModelSkeleton::ModelSkeleton(const ModelSkeleton & otherSkeleton)
{
	boneCount = otherSkeleton.boneCount;

	skinTransforms = NULL;
	boneAnimationTransforms = NULL;

	size_t skeletonBoneCount = (size_t)boneCount;
	skeletonBones.reserve(skeletonBoneCount);
	for (size_t i = 0; i < skeletonBoneCount; i++)
	{
		string tempName = otherSkeleton.skeletonBones[i].first;
		ModelSkeletonBone tempSkeletonBone = *(otherSkeleton.skeletonBones[i].second);

		skeletonBones.push_back({ tempName, new ModelSkeletonBone(tempSkeletonBone) });
	}
	boneCount = 0;
	min = 0;
}

ModelSkeleton::~ModelSkeleton()
{
	for each(Pair temp in skeletonBones)
		SAFE_DELETE(temp.second);

	SAFE_DELETE_ARRAY(boneAnimationTransforms);
	SAFE_DELETE_ARRAY(skinTransforms);
}

D3DXMATRIX ModelSkeleton::GetWeapon(string name)
{
	if (boneAnimationTransforms == NULL)
	{
		D3DXMATRIX dummy;
		D3DXMatrixIdentity(&dummy);
		return dummy;
	}
	return boneAnimationTransforms[FindBoneIndex(name)];

}

string ModelSkeleton::GetWeaponName(int num)
{
	return skeletonBones[num].first;
}

void ModelSkeleton::AddSkeletonBone(ModelSkeletonBone * skeletonBone)
{
	skeletonBones.push_back(Pair(skeletonBone->GetName(), skeletonBone));

	boneCount++;
}

ModelSkeletonBone * ModelSkeleton::FindBone(string boneName)
{
	for each(Pair temp in skeletonBones)
	{
		if (boneName == temp.first)
			return temp.second;
	}

	return NULL;
}

int ModelSkeleton::FindBoneIndex(string boneName)
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

D3DXMATRIX & ModelSkeleton::GetBoneAnimationTransform(int boneIdx)
{
	return boneAnimationTransforms[boneIdx];
}

ModelSkeletonBone * ModelSkeleton::GetSkeletonBone(int index)
{
	return skeletonBones[index].second;
}

void ModelSkeleton::AddAnimationFromOtherSkeleton(string srcAnimName, string dstAnimName, ModelSkeleton & srcSkeleton)
{
	vector<Pair> srcSkeletonBones = srcSkeleton.skeletonBones;

	vector<Pair>::const_iterator it = srcSkeletonBones.begin();
	for (; it != srcSkeletonBones.end(); ++it)
	{
		ModelSkeletonBone* srcBone = it->second;
		ModelSkeletonBone* dstBone = NULL;

		string srcBoneName = it->first;
		dstBone = FindBone(srcBoneName);

		if (dstBone != NULL)
		{
			ModelAnimationKeyFrames* srcAnimKeyFrames = srcBone->GetAnimationKeyFrames(srcAnimName);
			ModelAnimationKeyFrames* dstAnimKeyFrames = new ModelAnimationKeyFrames(*srcAnimKeyFrames);
			dstAnimKeyFrames->SetAnimationName(dstAnimName);
			dstBone->AddAnimationKeyFrames(dstAnimKeyFrames);
		}
	}
}

void ModelSkeleton::BuildBoneTransforms(ModelAnimationController* animationController, string animationName,UINT root,int range)
{

	if (skinTransforms == NULL)
		skinTransforms = new D3DXMATRIX[boneCount];
	
	if (boneAnimationTransforms == NULL)
		boneAnimationTransforms = new D3DXMATRIX[boneCount];
	
		
	int keyFrame = animationController->GetCurrentKeyFrame();
	int nextKeyFrame = animationController->GetNextKeyFrame();
	float keyframeFactor = animationController->GetKeyFrameFactor();
	bool useQuaterniokeyFrames = animationController->UseQuaternionKeyFrames();
	
	int index = 0;
	float min = 0, origin = 0;

	vector<Pair>::const_iterator it = skeletonBones.begin();
	for (; it != skeletonBones.end(); ++it)
	{
		ModelSkeletonBone* bone = it->second;
	
		D3DXMATRIX matInvBindPose = bone->GetInvBindPoseTransform();
		D3DXMATRIX matBoneReference = bone->GetBoneReferenceTransform();
		D3DXMATRIX matAnimation;
		D3DXMATRIX matParentAnimation;
	
		D3DXMATRIX matFirstRootTransform;
	
		ModelAnimationKeyFrames* animatiokeyFrames = bone->GetAnimationKeyFrames(animationName);
		
		if (useQuaterniokeyFrames == true)
		{
			const ModelAnimationQuaternionKeyFrame& qKeyFrame1 = animatiokeyFrames->GetQuaternionKeyFrame(keyFrame);
			const ModelAnimationQuaternionKeyFrame& qKeyFrame2 = animatiokeyFrames->GetQuaternionKeyFrame(nextKeyFrame);
			const D3DXQUATERNION& q1 = qKeyFrame1.GetQuaternion();
			const D3DXQUATERNION& q2 = qKeyFrame2.GetQuaternion();
			const D3DXQUATERNION& minus_q2 = -q2;
		
			D3DXQUATERNION q;
		
			if (D3DXQuaternionLengthSq(&(q1 - q2)) > D3DXQuaternionLengthSq(&(q1 + q2)))
				D3DXQuaternionSlerp(&q, &q1, &minus_q2, keyframeFactor); // Quaternion(회전축, 회전각) 선형 구면 보간
			else
				D3DXQuaternionSlerp(&q, &q1, &q2, keyframeFactor);
		
			D3DXMatrixRotationQuaternion(&matAnimation, &q);
		
			D3DXVECTOR3 t1 = qKeyFrame1.GetTranslation();
			D3DXVECTOR3 t2 = qKeyFrame2.GetTranslation();
		
		
			D3DXVECTOR3 vTranslation;
			D3DXVec3Lerp(&vTranslation, &t1, &t2, keyframeFactor);
			
			matAnimation._41 = vTranslation.x;
			matAnimation._42 = vTranslation.y;
			matAnimation._43 = vTranslation.z;
		}
		else
		{
			matAnimation = animatiokeyFrames->GetKeyFrameTransform(keyFrame);
		}
		
		
		int parentBoneIndex = bone->GetParentBoneIndex();
		
		if (parentBoneIndex < range)
		{
			D3DXMatrixIdentity(&matParentAnimation);
			if (root != 0)matAnimation = animatiokeyFrames->GetKeyFrameTransform(range);
			//matAnimation = animatiokeyFrames->GetKeyFrameTransform(0);
			//matAnimation = bone->GetBindPoseTransform();
		}
		else
		{
			matParentAnimation = boneAnimationTransforms[parentBoneIndex];
		}
		boneAnimationTransforms[index] = matAnimation * matParentAnimation;
		skinTransforms[index] = matBoneReference * matInvBindPose *  boneAnimationTransforms[index];

		float boneHeight = boneAnimationTransforms[index]._42;
		if (index == 0) { origin = min = boneHeight; }
		else if (min > boneHeight)min = boneHeight;

		index++;
	}
	this->min = origin - min;

}

void ModelSkeleton::UpdateAnimation(ModelAnimationController * animationController,UINT root,int range)
{
	ModelAnimation* currentAnimation = animationController->GetCurrentAnimation();
	int keyFrame = animationController->GetCurrentKeyFrame();

	if (currentAnimation == NULL)
		return;

	BuildBoneTransforms(animationController, currentAnimation->GetName(),root,range);
}

////////////////////////////////////////////////////////////////////////


void ModelSkeleton::Write(BinaryWriter * w, ModelSkeleton* skeleton)
{
	w->UInt(skeleton->boneCount);

	UINT skeletonBonesCount = skeleton->skeletonBones.size();
	w->UInt(skeletonBonesCount);
	for (size_t i = 0; i < skeletonBonesCount; i++)
	{
		w->String(skeleton->skeletonBones[i].first);
		(skeleton->skeletonBones[i].second)->Write(w, skeleton->skeletonBones[i].second);
	}

	w->Matrix(*skeleton->skinTransforms);
	w->Matrix(*skeleton->boneAnimationTransforms);
}

void ModelSkeleton::Read(BinaryReader * r, ModelSkeleton* skeleton)
{
	skeleton->boneCount = r->Int();

	UINT skeletonBonesCount = r->UInt();
	skeleton->skeletonBones.clear();
	skeleton->boneCount=0;
	skeleton->skeletonBones.reserve(skeletonBonesCount);
	for (size_t i = 0; i < skeletonBonesCount; i++)
	{
		string name = r->String();
		ModelSkeletonBone* modelSkeletonBone = new ModelSkeletonBone();
		modelSkeletonBone->Read(r, modelSkeletonBone);
		skeleton->skeletonBones.push_back(make_pair(name, modelSkeletonBone));
		skeleton->boneCount++;
	}

	SAFE_DELETE_ARRAY(skeleton->skinTransforms);
	skeleton->skinTransforms = new D3DXMATRIX[skeleton->boneCount];
	*skeleton->skinTransforms = r->Matrix();

	SAFE_DELETE_ARRAY(skeleton->boneAnimationTransforms);
	skeleton->boneAnimationTransforms = new D3DXMATRIX[skeleton->boneCount];
	*skeleton->boneAnimationTransforms = r->Matrix();
	SAFE_DELETE_ARRAY(skeleton->boneAnimationTransforms);


}

////////////////////////////////////////////////////////////////////////