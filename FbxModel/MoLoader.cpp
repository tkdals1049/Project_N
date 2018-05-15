#include "../stdafx.h"
#include "MoLoader.h"
#include "MoModel.h"
#include "MoMaterial.h"
#include "MoModelUtility.h"
#include "MoKeyFrame.h"
#include "MoSkeleton.h"
#include "MoSkeletonBone.h"
#include "MoAnimation.h"
#include "MoAnimationController.h"
#include "MoBoneWeights.h"
#include "MoModelUtility.h"

void MoLoader::Convert(string fbxFile, string saveFile)
{
	MoModel* model = NULL;
	MoLoader* loader = new MoLoader();

	model = loader->Load(fbxFile);
	SAFE_DELETE(loader);

	model->Write(saveFile);
	SAFE_DELETE(model);
}

void MoLoader::LoadBinary(string binFile, Model ** model)
{
	MoModel::Read(binFile, model);
}

void MoLoader::WriteBinary(string binFile, Model * model)
{
	MoModel::Write(binFile, model);
}

MoLoader::MoLoader()
{
	manager = FbxManager::Create();
	assert(manager != NULL);

	scene = FbxScene::Create(manager, "");
	assert(scene != NULL);
}

MoLoader::~MoLoader()
{
	scene->Destroy();
	manager->Destroy();
}

MoModel * MoLoader::Load(string file)
{
	int major, minor, revision;
	FbxManager::GetFileFormatVersion(major, minor, revision);

	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	importer = FbxImporter::Create(manager, "");
	bool check = importer->Initialize(file.c_str(), -1, ios);
	assert(check == true);



	importer->Import(scene);
	converter = new FbxGeometryConverter(manager);
	{
		model = new MoModel(file);

		ProcessMaterial();

		ProcessNode(scene->GetRootNode(), FbxNodeAttribute::eSkeleton);
		ProcessNode(scene->GetRootNode(), FbxNodeAttribute::eMesh);

		ProcessAnimations();
	}
	SAFE_DELETE(converter);

	ios->Destroy();
	importer->Destroy();

	return model;
}

void MoLoader::ProcessMaterial()
{
	for (int i = 0; i < scene->GetMaterialCount(); i++)
	{
		FbxSurfaceMaterial* temp = scene->GetMaterial(i);
		materials.push_back(temp);

		MoMaterial* material = new MoMaterial(i, temp);
		model->PushMaterial(material);
	}
}

void MoLoader::ProcessNode(FbxNode * node, FbxNodeAttribute::EType type)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		if (nodeType == type)
		{
			switch (nodeType)
			{
				case FbxNodeAttribute::eSkeleton:
					ProcessSkeleton(node);
					break;

				case FbxNodeAttribute::eMesh:
					ProcessMesh(node);
					break;

				case FbxNodeAttribute::eMarker:
				case FbxNodeAttribute::eNurbs:
				case FbxNodeAttribute::ePatch:
				case FbxNodeAttribute::eCamera:
				case FbxNodeAttribute::eLight:
				case FbxNodeAttribute::eNull:
					break;
			}//switch(nodeType)
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ProcessNode(node->GetChild(i), type);
}

void MoLoader::ProcessSkeleton(FbxNode * node)
{

	FbxSkeleton* fbxSkeleton = node->GetSkeleton();
	if (fbxSkeleton == NULL)return;

	if (model->skeleton == NULL)
		model->skeleton = new MoSkeleton();

	int parentBoneIndex = -1;
	FbxNode* parentNode = node->GetParent();
	if (parentNode != NULL)
	{
		parentBoneIndex = model->skeleton->FindBoneIndex(parentNode->GetName());
	}

	MoSkeletonBone* skeletonBone = new MoSkeletonBone(node->GetName(), parentBoneIndex);
	model->skeleton->AddSkeletonBone(skeletonBone);

	
}

void MoLoader::ProcessMesh(FbxNode * node)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	converter->Triangulate(attribute, true, true);

	FbxMesh* mesh = node->GetMesh();
	assert(mesh != NULL);

	int vertexCount = mesh->GetControlPointsCount();
	assert(vertexCount > 0);

	vector<MoBoneWeights> boneWeights(vertexCount, MoBoneWeights());
	ProcessBoneWeights(mesh, boneWeights);

	FbxVector4* controlPoints = mesh->GetControlPoints();
	for (int i = 0; i < mesh->GetPolygonCount(); i++)
	{
		MoMaterial* material = NULL;

		for (int vi = 0; vi < 3; vi++)
		{
			int vertexIndex = mesh->GetPolygonVertex(i, vi);
			if (vertexIndex < 0 || vertexIndex >= vertexCount)
				continue;

			if (material == NULL)
				material = LinkMaterialWithPolygon(mesh, 0, i, 0, vertexIndex);

			FbxVector4 fbxPos = controlPoints[vertexIndex];
			FbxVector4 fbxNormal;
			mesh->GetPolygonVertexNormal(i, vi, fbxNormal);
			fbxNormal.Normalize();

			D3DXVECTOR3 vecPos = MoModelUtility::ToVector3(fbxPos);
			D3DXVECTOR3 vecNormal = MoModelUtility::ToVector3(fbxNormal);

			D3DXVECTOR2 vecUv = GetUV(mesh, 0, i, vi, vertexIndex);
			model->PushVertex(material, vecPos, vecNormal, vecUv, boneWeights[vertexIndex]);
		}//for(vi)
	}//for(i)

	model->geometricOffset = GetGeometricOffset(node);
}
void MoLoader::ProcessAnimations()
{
	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == NULL) return;

	string test = rootNode->GetName();
	float frameRate = (float)FbxTime::GetFrameRate(scene->GetGlobalSettings().GetTimeMode());

	FbxArray<FbxString *> takeArray;
	FbxDocument* document = dynamic_cast<FbxDocument *>(scene);
	if (document != NULL)
		document->FillAnimStackNameArray(takeArray);

	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
		string takeName = (takeInfo->mName.Buffer());
		takeName += to_string(model->animationController->GetAnimationCount());

		FbxTime start = FbxTime(FbxLongLong(0x7fffffffffffffff));
		FbxTime stop = FbxTime(FbxLongLong(-0x7fffffffffffffff));

		FbxTimeSpan span = takeInfo->mLocalTimeSpan;
		double tempStart = span.GetStart().GetSecondDouble();
		double tempStop = span.GetStop().GetSecondDouble();

		if (tempStart < tempStop)
		{
			int keyFrames = (int)((tempStop - tempStart) * (double)frameRate);

			MoAnimation* animation = new MoAnimation(takeName, keyFrames, frameRate);
			model->animationController->AddAnimation(animation);

			string test2 = rootNode->GetName();
			ProcessAnimation(rootNode, takeName, frameRate, (float)tempStart, (float)tempStop);
		}
	}
	takeArray.Clear();
	//TODO: defualt 스택 세팅
}

void MoLoader::ProcessAnimation(FbxNode * node, string takeName, float frameRate, float start, float stop)
{
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute != NULL)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			if (model->skeleton != NULL)
			{
				string test = node->GetName();
				MoSkeletonBone* bone = model->skeleton->FindBone(node->GetName());

				if (bone != NULL)
				{
					MoKeyFrame* animationKeyFrames = new MoKeyFrame(takeName);

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

			if (model != NULL)
			{
				MoKeyFrame* animationKeyFrames = new MoKeyFrame(takeName);

				double time = 0;
				while (time <= (double)stop)
				{
					FbxTime takeTime;
					takeTime.SetSecondDouble(time);

					D3DXMATRIX matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node, takeTime);
					animationKeyFrames->AddKeyFrame(matAbsoluteTransform);

					time += 1.0f / frameRate;
				}
				model->AddAnimationKeyFrames(animationKeyFrames);
			}
		}//if(nodeAttribute->GetAttributeType())
	}//if(nodeAttribute)

	for (int i = 0; i < node->GetChildCount(); ++i)
		ProcessAnimation(node->GetChild(i), takeName, frameRate, start, stop);
}


void MoLoader::ProcessBoneWeights(FbxMesh * mesh, vector<MoBoneWeights>& meshBoneWeights)
{
	if (model->skeleton == NULL)
		return;

	// Deformer는 주로 Scene에서 하나만 존재
	for (int i = 0; i < mesh->GetDeformerCount(); ++i)
	{
		FbxDeformer* deformer = mesh->GetDeformer(i);

		if (deformer == NULL)
			continue;

		// Deformer의 타입이 Skin일 경우, 즉 Skinned Mesh일 경우
		if (deformer->GetDeformerType() == FbxDeformer::eSkin)
		{

			FbxSkin* skin = (FbxSkin *)deformer;

			if (skin == NULL)
				continue;

			ProcessBoneWeights(skin, meshBoneWeights);
		}
	}
}

void MoLoader::ProcessBoneWeights(FbxSkin * skin, vector<MoBoneWeights>& meshBoneWeights)
{
	FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize;

	int clusterCount = skin->GetClusterCount();
	vector<MoBoneWeights> skinBoneWeights(meshBoneWeights.size(), MoBoneWeights());
	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* cluster = skin->GetCluster(i);
		if (cluster == NULL)
			continue;

		linkMode = cluster->GetLinkMode();
		FbxNode* pLinkNode = cluster->GetLink();

		if (pLinkNode == NULL)
			continue;

		int nBoneIndex = model->skeleton->FindBoneIndex(pLinkNode->GetName());
		if (nBoneIndex < 0)
			continue;

		MoSkeletonBone* skeletonBone = model->skeleton->GetSkeletonBone(nBoneIndex);

		FbxAMatrix matClusterTransformMatrix;
		FbxAMatrix matClusterLinkTransformMatrix;
		cluster->GetTransformMatrix(matClusterTransformMatrix); // 매쉬의 Transform Matrix
		cluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix); // 본의 Transform Matrix

																		//TODO: 체크
		skeletonBone->SetBindPoseTransform(MoModelUtility::ToMatrix(matClusterLinkTransformMatrix)); // 본->루트
		skeletonBone->SetBoneReferenceTransform(MoModelUtility::ToMatrix(matClusterTransformMatrix)); // 매쉬->루트

		int* indices = cluster->GetControlPointIndices(); //해당 Bone에 영향을 받는 모든 Vertex Index값의 배열
		double* weights = cluster->GetControlPointWeights(); // 각 Vertex Index가 갖는 Weight값의 배열

															 // indices[j]의 Index를 갖는 Vertex에 영향을 주는 Bone의 Idx와, 그 수치인 Weights를
															 // MoBoneWeights Class로 형식으로 추가
		for (int j = 0; j < cluster->GetControlPointIndicesCount(); ++j)
			skinBoneWeights[indices[j]].AddBoneWeight(nBoneIndex, (float)weights[j]);
	}

	// Weight의 모드
	switch (linkMode)
	{
		case FbxCluster::eNormalize: // 모든 Weight의 합이 1 로 정규화
		{
			for (int i = 0; i < (int)skinBoneWeights.size(); ++i)
				skinBoneWeights[i].Normalize();
		}
		break;

		case FbxCluster::eAdditive: // 그대로 합산
			break;

		case FbxCluster::eTotalOne: // 모든 Weight의 합이 1 이어야 함(정규화 되지 않음)
			break;
	}

	for (size_t i = 0; i < meshBoneWeights.size(); i++)
		meshBoneWeights[i].AddBoneWeights(skinBoneWeights[i]);
}

MoMaterial * MoLoader::LinkMaterialWithPolygon(FbxMesh * mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	if (layerIndex < 0 || layerIndex > mesh->GetLayerCount())
		return NULL;


	FbxNode* node = mesh->GetNode();
	if (node == NULL) return NULL;

	FbxLayerElementMaterial* fbxMaterial = mesh->GetLayer(layerIndex)->GetMaterials();
	if (fbxMaterial == NULL) return NULL;

	int mappingIndex = GetMappingIndex
	(
		fbxMaterial->GetMappingMode()
		, polygonIndex
		, 0
		, vertexIndex
	);
	if (mappingIndex < 0) return NULL;

	FbxLayerElement::EReferenceMode refMode = fbxMaterial->GetReferenceMode();
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
			return GetMaterial(node->GetMaterial(mappingIndex));
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArr = fbxMaterial->GetIndexArray();

		if (mappingIndex < indexArr.GetCount())
		{
			int tempIndex = indexArr.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
				return GetMaterial(node->GetMaterial(tempIndex));
		}//if(mappingIndex)
	}//if(refMode)

	return NULL;
}

int MoLoader::GetMappingIndex(FbxLayerElement::EMappingMode mode, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	switch (mode)
	{
		case FbxLayerElement::eAllSame:
			return 0;
		case FbxLayerElement::eByControlPoint:
			return vertexIndex;
		case FbxLayerElement::eByPolygonVertex:
			return polygonIndex * 3 + polygonVertexIndex;
		case FbxLayerElement::eByPolygon:
			return polygonIndex;
		default:
			return -1;
	}
}

D3DXVECTOR2 MoLoader::GetUV(FbxMesh * mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	int layerCount = mesh->GetLayerCount();
	if (layerIndex >= layerCount) return D3DXVECTOR2();

	FbxLayer* layer = mesh->GetLayer(layerIndex);
	if (layer == NULL) return D3DXVECTOR2();

	FbxLayerElementUV* uv = layer->GetUVs(FbxLayerElement::eTextureDiffuse);
	if (uv == NULL) return D3DXVECTOR2();


	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	const FbxLayerElementArrayTemplate<FbxVector2>& uvArray = uv->GetDirectArray();
	const FbxLayerElementArrayTemplate<int>& uvIndexArray = uv->GetIndexArray();

	int mappingIndex = 0;
	if (mappingMode == FbxLayerElement::eByControlPoint)
	{
		mappingIndex = vertexIndex;

		if (refMode == FbxLayerElement::eDirect)
		{
			if (mappingIndex < uvArray.GetCount())
				return MoModelUtility::ToUv(uvArray.GetAt(mappingIndex));
		}
		else if (refMode == FbxLayerElement::eIndexToDirect)
		{
			if (mappingIndex < uvArray.GetCount())
			{
				int tempIndex = uvIndexArray.GetAt(mappingIndex);

				if (tempIndex < uvArray.GetCount())
					return MoModelUtility::ToUv(uvArray.GetAt(tempIndex));
			}
		}//if(refMode)
	}
	else if (mappingMode == FbxLayerElement::eByPolygonVertex)
	{
		mappingIndex = mesh->GetTextureUVIndex
		(
			polygonIndex
			, polygonVertexIndex
			, FbxLayerElement::eTextureDiffuse
		);

		switch (refMode)
		{
			case FbxLayerElement::eDirect:
			case FbxLayerElement::eIndexToDirect:
				if (mappingIndex < uvArray.GetCount())
					return MoModelUtility::ToUv(uvArray.GetAt(mappingIndex));
		}//switch(refMode)
	}//if(mappingMode)

	return D3DXVECTOR2();
}

MoMaterial * MoLoader::GetMaterial(FbxSurfaceMaterial * fbxMaterial)
{
	for (size_t i = 0; i < materials.size(); i++)
	{
		if (materials[i] == fbxMaterial)
			return model->materials[i];			
	}

	return NULL;
}

D3DXMATRIX MoLoader::GetGeometricOffset(FbxNode * node)
{
	if (node == NULL)
	{
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);

		return matrix;
	}

	FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);

	FbxMatrix offset;
	offset.SetIdentity();
	offset.SetTRS(T, R, S);

	return MoModelUtility::ToMatrix(offset);
}

D3DXMATRIX MoLoader::GetAbsoluteTransformFromCurrentTake(FbxNode * node, FbxTime time)
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