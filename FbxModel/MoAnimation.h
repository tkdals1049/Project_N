#pragma once

class MoAnimation
{
public:
	MoAnimation(string name, int keyFrame, float defaultFrameRate);
	~MoAnimation();

	void Write(BinaryWriter * bw);
	string GetName() { return name; }
private:
	string name;

	int keyFrame;
	float frameRate;
	float defaultFrameRate;
};
