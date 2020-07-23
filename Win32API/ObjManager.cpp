#include "stdafx.h"
#include "ObjManager.h"
#include "Obj.h"
#include "CollisionManager.h"

CObjManager* CObjManager::m_instance = nullptr;

CObjManager::CObjManager()
{
}


CObjManager::~CObjManager()
{
	Release();
}

CObjManager * CObjManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new CObjManager;
	}

	return m_instance;
}

void CObjManager::DestroyInstance()
{
	SafeDelete(m_instance);
}

void CObjManager::Update()
{
	for (int i = 0; i < OBJ::END; ++i)
	{
		auto endIter = m_objectList[i].end();
		for (auto iter = m_objectList[i].begin(); iter != endIter;)
		{
			if (DEAD_EVENT == (*iter)->Update())
			{
				SafeDelete(*iter);
				iter = m_objectList[i].erase(iter);
			}
			else ++iter;
		}
	}
}

void CObjManager::LateUpdate()
{
	for (int i = 0; i < OBJ::END; ++i)
	{
		for (const auto& elem : m_objectList[i])
		{
			elem->LateUpdate();
			if (m_objectList[i].empty()) break;
		}
	}
}

void CObjManager::Render(const HDC& hDC)
{
	for (int i = 0; i < OBJ::END; ++i)
	{
		for (const auto& elem : m_objectList[i])
		{
			elem->Render(hDC);
		}
	}
}

void CObjManager::Release()
{
	for (int i = 0; i < OBJ::END; ++i)
	{
		for (auto& elem : m_objectList[i])
		{
			SafeDelete(elem);
		}
		m_objectList[i].clear();
	}
}

void CObjManager::AddObject(CObj * obj, OBJ::TAG tag)
{
	m_objectList[tag].emplace_back(obj);
}

void CObjManager::RemoveObject(OBJ::TAG tag)
{
	for (auto& elem : m_objectList[tag])
	{
		SafeDelete(elem);
	}
	m_objectList[tag].clear();
}

CObj * CObjManager::GetPlayer()
{
	if (m_objectList[OBJ::PLAYER].empty()) return nullptr;

	return (m_objectList[OBJ::PLAYER]).front();
}