#include "stdafx.h"
#include "Model.h"

#include "BoneBuffer.h"
#include "ModelMesh.h"
#include "ModelMaterial.h"
#include "ModelSkeleton.h"
#include "ModelSkeletonBone.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"
#include "ModelAnimationKeyFrames.h"
#include "ModelBoneWeights.h"

#include "../FbxModel/MoModelUtility.h"

Model::Model()
: position(D3DXVECTOR3(0,0,0)), rotate(D3DXVECTOR3(0, 0, 0)), scale(D3DXVECTOR3(1,1,1)), adjust(D3DXVECTOR3(0, 0, 0)),
minp(D3DXVECTOR3(0, 0, 0)), maxp(D3DXVECTOR3(0, 0, 0)), origin(D3DXVECTOR3(0, 0, 0)),skeleton(NULL),animationController(NULL)
{
	damageColor = 0;
	root=range=0;
	speed =1.0f;
	D3DXMatrixIdentity(&matGeometricOffset);
	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&weaponWorld);

	manager = FbxManager::Create();
	assert(manager != NULL);

	scene = FbxScene::Create(manager, "");
	assert(scene != NULL);

	boneBuffer = new BoneBuffer();
	colorBuffer = new ColorBuffer();
	animationController = new ModelAnimationController();
}

Model::~Model()
{
	for (ModelMaterial* material : materials)
		SAFE_DELETE(material);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);

	SAFE_DELETE(skeleton);
}

void Model::Update()
{
	if(damageColor!=0)	colorBuffer->Data.Color.r -= Time::Get()->Delta()*damageColor;
	if (colorBuffer->Data.Color.r <= 0) damageColor =0;

	SetWorld();
	AnotherUpdate();	


	for (ModelMesh* mesh : meshes)
		mesh->Update();
}

void Model::Render()
{
	colorBuffer->SetPSBuffer(2);
	boneBuffer->SetInvworld(&(matGeometricOffset*world));
	boneBuffer->SetVSBuffer(4);

	for (ModelMesh* mesh : meshes)
 		mesh->Render();
}

void Model::AnotherUpdate()
{
	UINT isAnimated = 0;

	if (animationController != NULL)
	{
		isAnimated = animationController->GetAnimationCount() > 1;
		
		if (isAnimated !=0)
		{
		
			animationController->Update(speed);
		
			if (skeleton != NULL)
			{
				skeleton->UpdateAnimation(animationController,root,range);
			}
		}
	}

	if (skeleton != NULL)
	{
		D3DXMATRIX* bonesArray = skeleton->GetSkinTransforms();
		if (bonesArray != NULL)
		{
			UINT boneCount = skeleton->GetBoneCount();
			boneCount = boneCount < ModelBoneWeights::MaxBonesPerMax ? boneCount : ModelBoneWeights::MaxBonesPerMax;
			boneBuffer->SetBoneArray(bonesArray, boneCount);
		}
	}
	boneBuffer->SetSkinning(isAnimated==0?false:true);
	
}

void Model::AddAni(string file,string mode)
{
	//SDK의 버전을 얻어온다.
	int sdkMajor, sdkMinor, sdkRevision;
	FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);

	FbxIOSettings * ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	//임포터 생성
	importer = FbxImporter::Create(manager, "");
	bool status = importer->Initialize(file.c_str(), -1, ios);
	assert(status == true);

	status = importer->Import(scene);
	assert(status == true);

	ProcessAnimations(mode);

	importer->Destroy();

	animationController->SetCurrentAnimation(animationController->GetAnimationCount() - 1);
	animationController->Play();
}

void Model::AniChange(int num)
{
	if(num>0&&num<= (int)animationController->GetAnimationCount())
	{
		animationController->SetCurrentAnimation(num-1);
		animationController->Play();
	}
}
void Model::AniChange(string mode)
{
	animationController->SetCurrentAnimation(mode);
	animationController->Play();
}


void Model::ProcessAnimations(string mode="")
{
	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == NULL) return;
	float frameRate = (float)FbxTime::GetFrameRate(scene->GetGlobalSettings().GetTimeMode());

	FbxArray<FbxString *> takeArray;
	FbxDocument* document = dynamic_cast<FbxDocument *>(scene);
	if (document != NULL)
		document->FillAnimStackNameArray(takeArray);

	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
		string takeName = (takeInfo->mName.Buffer());
		takeName += to_string(animationController->GetAnimationCount());
		if(mode!="")takeName=mode;
		FbxTime start = FbxTime(FbxLongLong(0x7fffffffffffffff));
		FbxTime stop = FbxTime(FbxLongLong(-0x7fffffffffffffff));

		FbxTimeSpan span = takeInfo->mLocalTimeSpan;
		double tempStart = span.GetStart().GetSecondDouble();
		double tempStop = span.GetStop().GetSecondDouble();

		if (tempStart < tempStop)
		{
			int keyFrames = (int)((tempStop - tempStart) * (double)frameRate);

			ModelAnimation* animation = new ModelAnimation(takeName, keyFrames, frameRate);
			animationController->AddAnimation(animation);

			string test2 = rootNode->GetName();
			ProcessAnimation(rootNode, takeName, frameRate, (float)tempStart, (float)tempStop);
		}
	}
	takeArray.Clear();
	//TODO: defualt 스택 세팅
}

void Model::ProcessAnimation(FbxNode * node, string takeName, float frameRate, float start, float stop)
{
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute != NULL)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			if (skeleton != NULL)
			{
				ModelSkeletonBone* bone = skeleton->FindBone(node->GetName());

				if (bone != NULL)
				{
					ModelAnimationKeyFrames* animationKeyFrames = new ModelAnimationKeyFrames(takeName);

					double time = 0;
					while (time <= (double)stop)
					{
						FbxTime takeTime;
						takeTime.SetSecondDouble(time);

						D3DXMATRIX matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node, takeTime);
						D3DXMATRIX matParentAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node->GetParent(), takeTime);

						D3DXMATRIX matInvParentAbsoluteTransform;
						D3DXMatrixInverse(&matInvParentAbsoluteTransform, NULL, &matParentAbsoluteTransform);

						D3DXMATRIX matTransform = matAbsoluteTransform * matInvParentAbsoluteTransform;
						animationKeyFrames->AddKeyFrame(matTransform);

						time += 1.0f / frameRate;
					}

					bone->AddAnimationKeyFrames(animationKeyFrames);
				}//if(bone)
			}//if(skeleton)
		}
		else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
				ModelAnimationKeyFrames* animationKeyFrames = new ModelAnimationKeyFrames(takeName);

				double time = 0;
				while (time <= (double)stop)
				{
					FbxTime takeTime;
					takeTime.SetSecondDouble(time);

					D3DXMATRIX matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node, takeTime);
					animationKeyFrames->AddKeyFrame(matAbsoluteTransform);

					time += 1.0f / frameRate;
				}
				AddAnimationKeyFrames(animationKeyFrames);
		}//if(nodeAttribute->GetAttributeType())
	}//if(nodeAttribute)

	for (int i = 0; i < node->GetChildCount(); ++i)
		ProcessAnimation(node->GetChild(i), takeName, frameRate, start, stop);
}


/////////////////////////////////////////////////////////////////////////
bool Model::Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	bool check=false;
	for (ModelMesh* mesh : meshes)
	{
		if(mesh->ClickCheck(matGeometricOffset,origin,direction))
		return true;
	}
	return false;
}

void Model::Reset()
{
	D3DXVec3TransformCoord(&origin,&origin,&matGeometricOffset);
	for (ModelMesh* mesh : meshes)
	{
		mesh->Reset();
	}
	for (size_t i = 0; i <meshes.size(); i++)
	{
		if (i==0) meshes[i]->GetMinMax(minp, maxp);
		else
		{
			D3DXVECTOR3 minb, maxb;
			meshes[i]->GetMinMax(minb, maxb);
			if (minp.x > minb.x) minp.x = minb.x;
			if (maxp.x < maxb.x) maxp.x = maxb.x;
			if (minp.y > minb.y) minp.y = minb.y;
			if (maxp.y < maxb.y) maxp.y = maxb.y;
			if (minp.z > minb.z) minp.z = minb.z;
			if (maxp.z < maxb.z) maxp.z = maxb.z;
		}
	}
}

void Model::SetWorld()
{
		D3DXVECTOR3 size = (maxp - minp)/2;

		D3DXMATRIX matS;
		D3DXMatrixScaling(&matS, scale.x, scale.y, scale.z);

		D3DXMATRIX matR, matX, matY, matZ;
		D3DXMatrixRotationX(&matX, rotate.x*(float)D3DX_PI/180);
		D3DXMatrixRotationY(&matY, rotate.y*(float)D3DX_PI/180);
		D3DXMatrixRotationZ(&matZ, rotate.z*(float)D3DX_PI/180);
		matR = matX*matY*matZ;

		D3DXMATRIX matT, matRT;
		D3DXMatrixTranslation(&matRT, size.x*adjust.x, size.y*adjust.y, size.z*adjust.z);
		D3DXMatrixTranslation(&matT, position.x, position.y, position.z);
		matGeometricOffset = matS*matR*matRT*matT;
}

ModelMaterial * Model::GetMatchMaterial(UINT number)
{
	for (ModelMaterial* material : materials)
	{
		if (material->GetNumber() == number)
			return material;
	}

	return NULL;
}

void Model::MakeMaterial(int num, string diffusePath)
{
	materials[num]->SetDiffuseTexture(diffusePath);
}

void Model::MeshMaterial()
{
	for (ModelMesh* mesh : meshes)
	{
		mesh->material = GetMatchMaterial(mesh->material->GetNumber());
	}
}

void Model::SetMaterial(ModelMaterial * material)
{
	materials.push_back(material);
}

void Model::ClearMaterial()
{
	materials.clear();
}

D3DXMATRIX Model::GetWeaponWorld(string weapon)
{
	return skeleton->GetWeapon(weapon)*matGeometricOffset*world;
}

float Model::GetMinBone()
{
	return skeleton->GetMin();
}

void Model::SetWorld(D3DXMATRIX& world)
{
	this->world=world;
}

void Model::SetGeometricOffset(D3DXMATRIX & world)
{
	matGeometricOffset=world;
}

void Model::AddAnimationKeyFrames(ModelAnimationKeyFrames * animationKeyFrames)
{
	this->animationKeyFrames.insert(make_pair(animationKeyFrames->GetAnimationName(), animationKeyFrames));
}
void Model::GetFileName(string file)
{
	 file=this->file;
}

void Model::SetPosition(D3DXVECTOR3 & position)
{
	this->position = position;
}

D3DXVECTOR3 Model::GetPosition()
{
	return position;
}

void Model::SetRotate(D3DXVECTOR3 & rotate)
{
	this->rotate = rotate;
}

D3DXVECTOR3 Model::GetRotate()
{
	return rotate;
}

void Model::SetScale(D3DXVECTOR3 & scale)
{
	this->scale = scale;
}

D3DXVECTOR3 Model::GetScale()
{
	return scale;
}

void Model::SetAdjust(D3DXVECTOR3 & adjust)
{
	this->adjust = adjust;
}

D3DXVECTOR3 Model::GetAdjust()
{
	return adjust;
}

void Model::SetAniPlay(int root, int range,float speed)
{
	this->root=(UINT)root;
	this->range = (UINT)range;
	this->speed=speed;
}

D3DXMATRIX Model::GetAbsoluteTransformFromCurrentTake(FbxNode * node, FbxTime time)
{
	if (node == NULL)
	{
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);

		return mat;
	}

	/*FbxAnimEvaluator* e = node->GetAnimationEvaluator();
	FbxAMatrix matrix = e->GetNodeGlobalTransform(node, time, FbxNode::eSourcePivot, false);*/
	FbxAMatrix matrix = node->EvaluateGlobalTransform(time);

	return MoModelUtility::ToMatrix(matrix);
}

void Model::isDamage()
{
	if (damageColor == 0) damageColor = 1;
	colorBuffer->Data.Color.r = 1;
}
