#include "../stdafx.h"
#include "ModelGroup.h"

#include "./BinModel/Model.h"
#include "./BinModel/ModelMaterial.h"
#include "./BinModel/ModelSkeleton.h"
#include "./BinModel/ModelSkeletonBone.h"
#include "./BinModel/ModelAnimationController.h"

#include "./Actor/Player.h"
#include "./Actor/Boss.h"
#include "./Actor/EnemyManager.h"

#include "../Content/BinModel.h"
#include "../Content/FbxModel.h"
#include "../Content/Texture.h"

ModelGroup::ModelGroup():dot(D3DXVECTOR2(0,0)),a(0.0f)
{
	SaveFile="";
	attitude=NULL;
	another=NULL;
	model=NULL;
	skeletonList=NULL;

	player= new Player();
	enemy=new EnemyManager(player);
}

ModelGroup::~ModelGroup()
{
	SAFE_DELETE(attitude);
	SAFE_DELETE(another);
	SAFE_DELETE(model);
	for (size_t i = 0; i <models.size(); i++)
	SAFE_DELETE(models[i]);
}

void ModelGroup::PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	//모델 위치 및 생성
	if (Mouse::Get()->Down(0)&&OnModel==true)
	{
		if (another == NULL) 
		Check(origin, direction);
		else 
		{
			if(dot!=D3DXVECTOR2(-1,-1))
			{
				SetModel(another);
			}
		}
	}
	//현재 모델삭제
	if (Keyboard::Get()->Down(VK_ESCAPE))
	{
		SAFE_DELETE(another);
		another = NULL;
		model=NULL;
	}
	player->PreUpdate(origin,direction);
}
void ModelGroup::Update(int thread)
{
	for (size_t i = 0; i <models.size()-thread; i++)
	{
		models[i]->Update();
		for each(Weapon temp in weapons[models[i]])
		{
			temp.second->Update();
			temp.second->SetWorld(models[i]->GetWeaponWorld(temp.first));
		}
	}
	if (attitude != NULL)attitude->Update();
	if (another != NULL)
	{
		another->SetPosition(D3DXVECTOR3(dot.x, 0, dot.y));
		another->Update();
	}
	player->Update();
	enemy->Update();
}
void ModelGroup::Render(int thread)
{
	for (size_t i = 0; i <models.size() - thread; i++)
	{
		models[i]->Render();
		for each(Weapon temp in weapons[models[i]])
		{
			temp.second->Render();
		}
	}
	if (attitude != NULL)attitude->Render();
	if (another != NULL)another->Render();
	enemy->Render();
	player->Render();

}

void ModelGroup::PostRender(bool& isUse)
{
	//모델 위치 저장
		if (another != NULL)
		{	
			if(model!=another)
			{
				model = another;
				D3DXVECTOR3 temp1=model->GetPosition();
				D3DXVECTOR3 temp2=model->GetRotate();
				D3DXVECTOR3 temp3=model->GetScale();
				c_temp1[0] = temp1.x;c_temp1[1] = temp1.y;c_temp1[2] = temp1.z;
				c_temp2[0] = temp2.x;c_temp2[1] = temp2.y;c_temp2[2] = temp2.z;
				c_temp3[0] = temp3.x;c_temp3[1] = temp3.y;c_temp3[2] = temp3.z;
			}
		}
		else if (attitude != NULL)
		{
			if (model != attitude)
			
			{
				model = attitude;
				D3DXVECTOR3 temp1 = model->GetPosition();
				D3DXVECTOR3 temp2 = model->GetRotate();
				D3DXVECTOR3 temp3 = model->GetScale();
				c_temp1[0] = temp1.x;c_temp1[1] = temp1.y;c_temp1[2] = temp1.z;
				c_temp2[0] = temp2.x;c_temp2[1] = temp2.y;c_temp2[2] = temp2.z;
				c_temp3[0] = temp3.x;c_temp3[1] = temp3.y;c_temp3[2] = temp3.z;
			}
		}
	//모델 관련 설정
	ImGui::Begin("Model",&isUse);
	OnModel = ImGui::CollapsingHeader("Model");
	if (OnModel)
	{
		static int curModel = 0, preModel = 0;
		if (curModel != preModel)
		{
			MoLoader::LoadBinary(BinModel::Get()->GetBinModelPath(curModel), &another);
			another->Reset();
			preModel=curModel;
		}

		string str="";
		if(model!=NULL)str=model->GetFile();
		ImGui::InputText("Model Name",(char*)str.c_str(),str.size());

		ImGui::ListBox("", &curModel, BinModel::Get()->GetBinmodelList(), BinModel::Get()->GetBinmodelPathSize(), 4);
		if (ImGui::Button("Refresh", ImVec2(60, 20)))
		{
			BinModel::Get()->Refresh();
		}

		ImGui::Separator();

		if(model!=NULL)
		{
			ImGui::PushItemWidth(300);

			ImGui::LabelText("", "x");ImGui::SameLine(110);
			ImGui::LabelText("", "y");ImGui::SameLine(210);
			ImGui::LabelText("", "z");

			ImGui::InputFloat3("Position",c_temp1,1);
			ImGui::InputFloat3("Rotate", c_temp2, 1);
			ImGui::InputFloat3("Scale", c_temp3,  1);

			ImGui::PopItemWidth();

			if (ImGui::Button("Delete", ImVec2(50, 20)))
			{
				for(size_t i=0; i<models.size();i++)
				if(attitude==models[i])
				{
					Model* back=models[i];
					for each(Weapon temp in weapons[models[i]])	SAFE_DELETE(temp.second);
					weapons[models[i]].clear();
					weapons.erase(weapons.find(models[i]));
					models.erase(models.begin()+i);

					SAFE_DELETE(back);
					attitude=NULL;
					model=NULL;
				}
			}ImGui::SameLine();
			if (ImGui::Button("Enter", ImVec2(50, 20)))
			{
				model->SetPosition(D3DXVECTOR3(c_temp1));
				model->SetRotate(D3DXVECTOR3(c_temp2));
				model->SetScale(D3DXVECTOR3(c_temp3));
			}

			static D3DXVECTOR3 temp(0, 0.78f, 0);
			static D3DXVECTOR3 temp2(1,0, 1.5f);
			ImGui::InputFloat3("adjust", temp, 1);
			ImGui::InputFloat3("aniset", temp2, 1);
			if (ImGui::Button("SetPlayer", ImVec2(80, 20)))
			{
				for (size_t i = 0; i<models.size();i++)
					if (attitude == models[i])
					{
						Model* back = models[i];
						player->SetModel(back);
						player->SetAdjust(temp);
						player->SetAniPlay(temp2);
						player->AniChange("idle");
						for each(Weapon temp in weapons[models[i]])	player->AddWeapon(temp.second,back->GetScale());
						weapons[models[i]].clear();
			  			weapons.erase(weapons.find(models[i]));
						models.erase(models.begin() + i);
						
						attitude = NULL;
						model = NULL;
						temp=D3DXVECTOR3(0,0,0);
						temp2 = D3DXVECTOR3(0,0, 1);
					}
			}
			
		}
		else 
		{
			if (ImGui::Button("PlayerDelete", ImVec2(80, 20)))
			{
				Model* back = player->GetModel();
				SAFE_DELETE(back);
				player->SetModel((Model*)NULL);
				player->ClearWeapon();
			}
		}
	}

	//모델에 집어넣을 마테리얼
	static bool OnMaterial=false;
	OnMaterial = ImGui::CollapsingHeader("Material");
	if (OnMaterial)
	{
		if(model!=NULL)
		{
			int num=model->GetMaterialCount();
			ImGui::Columns(2, "mycolumns"); // 4-ways, with border
			ImGui::Separator();
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Path"); ImGui::NextColumn();
			ImGui::Separator();

			for (int i = 0; i < num; i++)
			{
				ModelMaterial* material=model->GetMaterial(i);
				ImGui::Text(material->GetName().c_str()); ImGui::NextColumn();
				ImGui::Text(material->GetDiffuseFile().c_str()); ImGui::NextColumn();
			}
			ImGui::Columns(1);
			ImGui::Separator();

			static int curTexture = 0, textureNum = 0;

			ImGui::InputInt("Texture Number", (int *)&textureNum);
			ImGui::ListBox("Texture List", &curTexture, Texture::Get()->GetTextureList(), Texture::Get()->GetTexturePathSize(), 4);
			ImGui::SameLine();
			if (ImGui::Button("Change"))
			{
				if (textureNum >= 0 && textureNum < num)
				{
					model->MakeMaterial(textureNum, Texture::Get()->GetTexturePath(curTexture));

				}
			}		
			if (ImGui::Button("Refresh", ImVec2(60, 20)))
			{
				Texture::Get()->Refresh();
			}

		}
	}

	//모델 장비
	static bool OnSkeleton = false;
	OnSkeleton = ImGui::CollapsingHeader("Skeleton");
	if (OnSkeleton)
	{
			static int curModel = 0, curSkeleton = 0;
		
			ImGui::PushItemWidth(200);
			ImGui::ListBox("1", &curModel, BinModel::Get()->GetBinmodelList(), BinModel::Get()->GetBinmodelPathSize(), 8);ImGui::SameLine();
			if (model != NULL&&another==NULL)if(attitude->GetSkeleton()->GetBoneCount()>=1)ImGui::ListBox("2", &curSkeleton, skeletonList, attitude->GetSkeleton()->GetBoneCount(), 8);
			ImGui::PopItemWidth();

			if (ImGui::Button("equip"))
			{
				Model* temp=NULL;
				MoLoader::LoadBinary(BinModel::Get()->GetBinModelPath(curModel),&temp);
				temp->Reset();
				if (model != NULL&&another == NULL)AddWeaponVector(model,skeletonList[curSkeleton],temp);
			}
	}

	//모델 애니메이션
	static bool OnAni = false;
	OnAni = ImGui::CollapsingHeader("Animation");
	if (OnAni)
	{
			static int curModel = 0, curSkeleton = 0;

			ImGui::PushItemWidth(200);
			ImGui::ListBox("Ani List", &curModel, FbxModel::Get()->GetFbxModelList(), FbxModel::Get()->GetFbxModelSize(), 8);
			if (ImGui::Button("Refresh", ImVec2(60, 20)))
			{
				FbxModel::Get()->Refresh();
			}

			ImGui::Separator();
			static char aniName[100] = "";
			ImGui::InputText("Ani Name", aniName, IM_ARRAYSIZE(aniName));ImGui::SameLine();
			static float speed=1.0f;
			ImGui::InputFloat("Speed",&speed,0.1f,1.0f);
			static int root=0;
			ImGui::InputInt("Root",&root);

			ImGui::PopItemWidth();

			if (model != NULL&&another == NULL)
			{
				if(model->GetAnimationController()->GetAnimationCount()>0)
				{
					if (ImGui::Button("SetAni "))model->AddAni(FbxModel::Get()->GetFbxModelPath(curModel),aniName);ImGui::SameLine();
					if (ImGui::Button("AniPlay "))model->GetAnimationController()->Play();ImGui::SameLine();
					if (ImGui::Button("AniStop "))model->GetAnimationController()->Stop();ImGui::SameLine();
					if (ImGui::Button("AniPause "))model->GetAnimationController()->Pause();
					
					static int aniCount = 0;
					int curCount=model->GetAnimationController()->GetAnimationCount();
					ImGui::InputInt("AniCount", &aniCount);ImGui::SameLine();
					ImGui::LabelText("/CurCount",(char*)to_string(curCount).c_str());
					if (ImGui::Button("AniChange "))model->AniChange(aniCount);
					static int lines = 0, size=0;
					size=model->GetAnimationController()->GetCurrentAnimationCount();
					lines=model->GetAnimationController()->GetCurrentKeyFrame();
					ImGui::SliderInt("Number of lines", &lines, 0, size);
					model->GetAnimationController()->SetCurrentKeyFrame(lines);
				}
			}
	}
	ImGui::End();
}

void ModelGroup::SetModel(string file)
{
	Model* model = NULL;
	MoLoader::LoadBinary(file, &model);
	model->Reset();

	models.push_back(model);
}

void ModelGroup::SetModel(Model* another)
{
	Model* model = NULL;
	MoLoader::LoadBinary(another->GetFile(), &model);
	model->SetPosition(another->GetPosition());
	model->SetRotate(another->GetRotate());
	model->SetScale(another->GetScale());
	model->Reset();

	for(size_t i=0;i<another->GetMaterialCount();i++)
	{
		model->MakeMaterial(i,another->GetMaterial(i)->GetDiffuseFile());
	}

	models.push_back(model);
}

void ModelGroup::SetModel()
{
	Model* model = NULL;
	MoLoader::LoadBinary(attitude->GetFile(), &model);
	model->SetPosition(attitude->GetPosition());
	model->SetRotate(attitude->GetRotate());
	model->SetScale(attitude->GetScale());
	model->Reset();

	for (size_t i = 0;i<attitude->GetMaterialCount();i++)
	{
		model->MakeMaterial(i, attitude->GetMaterial(i)->GetDiffuseFile());
	}

	models.push_back(model);
}

Model * ModelGroup::GetModel() 
{ 
	return models[models.size() - 1]; 
}

void ModelGroup::AddWeaponVector(Model * model, string weaponName, Model * weaponFile)
{	
	weapons[model].push_back(Weapon(weaponName,weaponFile));
}

void ModelGroup::SetAttitude(Model * model)
{
	attitude=model;
}

Model * ModelGroup::GetAttitude()
{
	return attitude;
}

void ModelGroup::Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	for (size_t i = 0; i <models.size(); i++)
	{
		if(models[i]->Check(origin,direction))
		{
		if(attitude!=models[i])
		{
		SetAttitude(models[i]);
		if(attitude->GetSkeleton()!=NULL)
		{
			int num=attitude->GetSkeleton()->GetBoneCount();
			skeletonList = new const char*[num];
			for (size_t i = 0; (int)i < num; i++)
			{
				ModelSkeletonBone* bone= attitude->GetSkeleton()->GetSkeletonBone(i);
				skeletonList[i] = new char[bone->GetName().size() + 1];
				strcpy_s((char*)skeletonList[i], bone->GetName().size() + 1, bone->GetName().c_str());
			}
		}
		}
		return;
		}
	}

}