#pragma once

class ModelAnimationQuaternionKeyFrame;
class MoAnimationQuaternionKeyFrame
{
public:
	MoAnimationQuaternionKeyFrame(){};
	MoAnimationQuaternionKeyFrame(const D3DXMATRIX& transform)
	{
		D3DXQuaternionRotationMatrix(&q, &transform);
		translation = D3DXVECTOR3(transform._41, transform._42, transform._43);
	}

	const D3DXQUATERNION& GetQuaternion() const { return q; }
	const D3DXVECTOR3& GetTranslation() const { return translation; }

	void Write(BinaryWriter * w);
private:

	D3DXQUATERNION q;
	D3DXVECTOR3 translation;
};

class ModelAnimationKeyFrames;
class MoKeyFrame
{
public:
	MoKeyFrame(){};
	MoKeyFrame(string animationName);
	~MoKeyFrame();

	void AddKeyFrame(const D3DXMATRIX& transform);

	string GetAnimationName();
	const D3DXMATRIX& GetKeyFrameTransform(int keyFrame) const;
	const MoAnimationQuaternionKeyFrame& GetQuaternionKeyFrame(int keyFrame) const;

	void Write(BinaryWriter * w);

private:
	string animationName;

	typedef pair<D3DXMATRIX, MoAnimationQuaternionKeyFrame> Pair;
	vector<Pair> keyFrames;
};