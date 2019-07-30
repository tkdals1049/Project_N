#include "../../stdafx.h"
#include "ModelAnimationKeyFrames.h"

ModelAnimationKeyFrames::ModelAnimationKeyFrames(string animationName)
	: animationName(animationName)
{

}

ModelAnimationKeyFrames::ModelAnimationKeyFrames(ModelAnimationKeyFrames & other)
{
	animationName = other.animationName;
	keyFrames = other.keyFrames;
}

ModelAnimationKeyFrames::~ModelAnimationKeyFrames()
{

}

void ModelAnimationKeyFrames::AddKeyFrame(const D3DXMATRIX & transform)
{
	keyFrames.push_back(make_pair(transform, ModelAnimationQuaternionKeyFrame(transform)));
}

string ModelAnimationKeyFrames::GetAnimationName()
{
	return animationName;
}

void ModelAnimationKeyFrames::SetAnimationName(string animationName)
{
	this->animationName = animationName;
}

const D3DXMATRIX & ModelAnimationKeyFrames::GetKeyFrameTransform(int keyFrame) const
{
	float a = keyFrames[keyFrame].first._42;
	float b = keyFrames[keyFrame].second.GetTranslation().y;
	return keyFrames[keyFrame].first;
}

const ModelAnimationQuaternionKeyFrame & ModelAnimationKeyFrames::GetQuaternionKeyFrame(int keyFrame) const
{
	return keyFrames[keyFrame].second;
}

///////////////////////////////////////////////////////////////////////

void ModelAnimationKeyFrames::Write(BinaryWriter * w, ModelAnimationKeyFrames * keyframe)
{
	w->String(keyframe->animationName);

	UINT keyFramesCount = keyframe->keyFrames.size();
	w->UInt(keyFramesCount);
	for (size_t i = 0; i < keyFramesCount; i++)
	{
		w->Matrix(keyframe->keyFrames[i].first);
		keyframe->keyFrames[i].second.Write(w, &keyframe->keyFrames[i].second);
	}
}

void ModelAnimationKeyFrames::Read(BinaryReader * r, ModelAnimationKeyFrames * keyframe)
{
	keyframe->animationName = r->String();

	UINT keyFramesCount = r->UInt();
	keyframe->keyFrames.clear();
	keyframe->keyFrames.reserve(keyFramesCount);
	for (size_t i = 0; i < keyFramesCount; i++)
	{
		D3DXMATRIX transform = r->Matrix();
		ModelAnimationQuaternionKeyFrame quternion;
		quternion.Read(r, &quternion);
		keyframe->keyFrames.push_back(make_pair(transform, quternion));
	}
}

void ModelAnimationQuaternionKeyFrame::Write(BinaryWriter * w, ModelAnimationQuaternionKeyFrame* quaterkeyframe)
{
	w->Vector4((D3DXVECTOR4)quaterkeyframe->q);
	w->Vector3(quaterkeyframe->translation);
}

void ModelAnimationQuaternionKeyFrame::Read(BinaryReader * r, ModelAnimationQuaternionKeyFrame* quaterkeyframe)
{
	quaterkeyframe->q = (D3DXQUATERNION)r->Vector4();
	quaterkeyframe->translation = r->Vector3();
}

///////////////////////////////////////////////////////////////////////