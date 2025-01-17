// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#include <io.h>

#include "fmod.h"
#pragma comment(lib, "fmodex_vc.lib")

#pragma comment(lib,"msimg32.lib")

#ifdef _DEBUG
 #ifdef UNICODE
  #pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
 #else
  #pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ 헤더
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

using namespace std;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "Extern.h"
#include "Define.h"
#include "Enum.h"
#include "Struct.h"
#include "Function.h"
#include "Frames.h"

#include "AbstractFactory.h"
#include "CollisionManager.h"

#include "ObjManager.h"
#include "KeyManager.h"
#include "BitmapManager.h"
#include "SceneManager.h"
#include "TileManager.h"
#include "SoundManager.h"
#include "Camera.h"
#include "DamageFontManager.h"
#include "ComboManager.h"
#include "InvenManager.h"
#include "EquipManager.h"