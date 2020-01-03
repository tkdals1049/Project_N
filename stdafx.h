#pragma once

#include <Windows.h>
#include <assert.h>

#include <io.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#pragma comment(lib, "winmm.lib")

#include <string>
#include <vector>
#include <list>
#include <map>
#include <thread>
#include <functional>
#include <iostream>
using namespace std;

#include <atlbase.h>
#include <atlconv.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>////쉐이더 리플렉션을 사용하기 위해 자동 생성 시켜줌
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")//쉐이더 리플렉션을 사용하기 위해 자동 생성 시켜줌

#include <dsound.h>
#pragma comment(lib, "dsound.lib")
#include <MMSystem.h>
#pragma comment (lib, "winmm.lib")

#include "include/vorbis/codec.h"
#include "include/vorbis/vorbisfile.h"
#ifdef _DEBUG    
#	pragma comment(lib, "lib/libogg_static_d.lib")
#	pragma comment(lib, "lib/libvorbis_static_d.lib")
#	pragma comment(lib, "lib/libvorbisenc_static_d.lib")
#	pragma comment(lib, "lib/libvorbisfile_static_d.lib")
#else
#	pragma comment(lib, "lib/libogg_static.lib")
#	pragma comment(lib, "lib/libvorbis_static.lib")
#	pragma comment(lib, "lib/libvorbisenc_static.lib")
#	pragma comment(lib, "lib/libvorbisfile_static.lib")
#endif

#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
#pragma comment(lib, "libfbxsdk-md.lib")
using namespace fbxsdk;

#include <imgui.h>
#include <imguiDx11.h>
#pragma comment(lib, "imgui.lib")

#define SAFE_RELEASE(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p){ if(p){ delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p); (p) = NULL; } }
#define IS_IN_RANGE(value,r0,r1) (( ((r0) <= (value)) && ((value) <= (r1)) ) ? 1 : 0)
#define UINT unsigned int
#define radian (float)(D3DX_PI / 180)

const wstring Contents = L"../_Contents/";
const wstring BinModels = L"../_Contents/BinModels/";
const wstring FbxModels = L"../_Contents/FbxModels/";
const wstring Shaders = L"../_Shaders/";
const string Sounds = "../_Sound/";

#include "./Systems/D3D.h"
#include "./Systems/Keyboard.h"
#include "./Systems/Mouse.h"
#include "./Systems/Time.h"
#include "./Systems/MusicPlayer.h"

#include "./Renders/RenderTarget.h"
#include "./Renders/VertexLayouts.h"
#include "./Renders/States.h"
#include "./Renders/Shader.h"
#include "./Renders/ShaderBuffer.h"
#include "./Renders/GlobalBuffers.h"
#include "./Renders/LightManager.h"
#include "./Renders/ShaderManager.h"

#include "./Viewer/Camera.h"
#include "./Viewer/CameraManager.h"

#include "./Utilities/BinaryFile.h"
#include "./Utilities/Path.h"
#include "./Utilities/String.h"
#include "./Utilities/ZFrustum.h"
#include "./Utilities/ZQuadTree.h"

#include "./Environment/Sky.h"
#include "./Environment/Plane.h"
#include "./Ui/Ui.h"

#include "./Model/BinModel/Model.h"
#include "./Model/BinModel/ModelMaterial.h"
#include "./Model/BinModel/ModelSkeletonBone.h"
#include "./Model/BinModel/ModelSkeleton.h"
#include "./Model/BinModel/ModelAnimation.h"
#include "./Model/BinModel/ModelAnimationController.h"
#include "./Model/FbxModel/MoLoader.h"

#include "./Model/Actor/Actor.h"

#include "./Content/Texture.h"
#include "./Content/BinModel.h"
#include "./Content/FbxModel.h"
#include "./Content/Test.h"