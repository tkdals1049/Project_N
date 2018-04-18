#include "../stdafx.h"
#include "MoAnimation.h"

MoAnimation::MoAnimation(string name, int keyFrame, float defaultFrameRate)
	: name(name), keyFrame(keyFrame), frameRate((float)keyFrame), defaultFrameRate(defaultFrameRate)
{

}

MoAnimation::~MoAnimation()
{
}


void MoAnimation::Write(BinaryWriter * bw)
{
	bw->String(name);

	bw->Int(keyFrame);
	bw->Float(frameRate);
	bw->Float(defaultFrameRate);
}