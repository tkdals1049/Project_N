#include "../stdafx.h"
#include "MoKeyFrame.h"
#include "../Model/ModelAnimationKeyFrames.h"

MoKeyFrame::MoKeyFrame(string animationName)
	: animationName(animationName)
{

}

MoKeyFrame::~MoKeyFrame()
{

}

void MoKeyFrame::AddKeyFrame(const D3DXMATRIX & transform)
{
	keyFrames.push_back(make_pair(transform, MoAnimationQuaternionKeyFrame(transform)));
}

string MoKeyFrame::GetAnimationName()
{
	return animationName;
}

const D3DXMATRIX & MoKeyFrame::GetKeyFrameTransform(int keyFrame) const
{
	return keyFrames[keyFrame].first;
}

const MoAnimationQuaternionKeyFrame & MoKeyFrame::GetQuaternionKeyFrame(int keyFrame) const
{
	return keyFrames[keyFrame].second;
}

void MoKeyFrame::Write(BinaryWriter * w)
{
	w->String(animationName);

	UINT keyFramesCount = keyFrames.size();
	w->UInt(keyFramesCount);
	for (size_t i = 0; i < keyFramesCount; i++)
	{
		w->Matrix(keyFrames[i].first);
		keyFrames[i].second.Write(w);
	}
}

void MoAnimationQuaternionKeyFrame::Write(BinaryWriter * w)
{
	w->Vector4((D3DXVECTOR4)q);
	w->Vector3(translation);
}
