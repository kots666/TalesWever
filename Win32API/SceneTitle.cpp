#include "stdafx.h"
#include "SceneTitle.h"
#include "MyBitmap.h"
#include "WidthButton.h"

CSceneTitle::CSceneTitle()
{
	Init();
}

CSceneTitle::~CSceneTitle()
{
	Release();
}

void CSceneTitle::Init()
{
	CObj* newButton = new CWidthButton(401.5f, 405.f, 115, 24, __T("Start"));
	CObjManager::GetInstance()->AddObject(newButton, OBJ::BUTTON);

	newButton = new CWidthButton(401.5f, 453.f, 115, 24, __T("Credit"));
	CObjManager::GetInstance()->AddObject(newButton, OBJ::BUTTON);

	newButton = new CWidthButton(401.5f, 477.f, 115, 24, __T("Exit"));
	CObjManager::GetInstance()->AddObject(newButton, OBJ::BUTTON);

	CTileManager::GetInstance();

	CSoundManager::GetInstance()->PlayBGM(__T("BGM_Title.mp3"));
	CSoundManager::GetInstance()->SetVolume(0.5f, CSoundManager::BGM);
}

void CSceneTitle::Update()
{
}

void CSceneTitle::LateUpdate()
{
}

void CSceneTitle::Render(HDC hDC)
{
	HDC memDC = CBitmapManager::GetInstance()->GetDC(__T("Title"));

	BitBlt(hDC, 0, 0, WINCX, WINCY, memDC, 0, 0, SRCCOPY);
}

void CSceneTitle::Release()
{
	CSoundManager::GetInstance()->StopSound(CSoundManager::BGM);
}
