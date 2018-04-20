#pragma once

#include <Windows.h>
#include <assert.h>

#include <io.h>
#include <conio.h>
#include <time.h>
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

#define UINT unsigned int

const wstring Contents = L"../_Contents/";
const wstring BinModels = L"../_Contents/BinModels/";
const wstring FbxModels = L"../_Contents/FbxModels/";
const wstring Shaders = L"../_Shaders/";
#include "./Systems/D3D.h"
#include "./Systems/Keyboard.h"
#include "./Systems/Mouse.h"
#include "./Systems/Time.h"

#include "./Renders/VertexLayouts.h"
#include "./Renders/States.h"
#include "./Renders/Shader.h"
#include "./Renders/ShaderBuffer.h"
#include "./Renders/GlobalBuffers.h"

#include "./Viewer/Camera.h"

#include "./Utilities/BinaryFile.h"
#include "./Utilities/Path.h"
#include "./Utilities/String.h"

#include "./Model/Model.h"
#include "./FbxModel/MoLoader.h"