#include "../../stdafx.h"
#include "ModelAnimation.h"

ModelAnimation::ModelAnimation() 
	: name(""), keyFrame(0), frameRate(0), defaultFrameRate(0)
{
}

ModelAnimation::ModelAnimation(string name, int frameCount, float defaultFrameRate)
	: name(name), keyFrame(frameCount), frameRate(defaultFrameRate), defaultFrameRate(defaultFrameRate)
{
	
}

ModelAnimation::~ModelAnimation()
{
}


void ModelAnimation::Write(BinaryWriter * bw,ModelAnimation* modelAnimation)
{
	bw->String(modelAnimation->name);

	bw->Int(modelAnimation->keyFrame);
	bw->Float(modelAnimation->frameRate);
	bw->Float(modelAnimation->defaultFrameRate);
}

void ModelAnimation::Read(BinaryReader * br, ModelAnimation* modelAnimation)
{
	modelAnimation->name = br->String();

	modelAnimation->keyFrame = br->Int();
	modelAnimation->frameRate = br->Float();
	modelAnimation->defaultFrameRate = br->Float();
}