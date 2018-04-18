#pragma once

class ModelAnimation 
{
public:
	ModelAnimation();
	ModelAnimation(string name, int keyFrameCount, float defaultFrameRate);
	~ModelAnimation();

	static void Write(BinaryWriter * bw,ModelAnimation* modelAnimation);
	static void Read(BinaryReader * br ,ModelAnimation* modelAnimation);

	void SetName(string name) { this->name = name; }

	string GetName() const { return name; }
	int GetKeyFrames() const { return keyFrame; }

	float GetFrameRate() const { return frameRate; }
	void SetFrameRate(float frameRate) { this->frameRate = frameRate; }

	float GetDefaultFrameRate() const { return defaultFrameRate; }

	void ResetFrameRate() { frameRate = defaultFrameRate; }

private:
	string name;

	int keyFrame;
	float frameRate;
	float defaultFrameRate;
};
