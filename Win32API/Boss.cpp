#include "stdafx.h"
#include "Boss.h"
#include "CollideRect.h"
#include "MotionTrail.h"
#include "Player.h"

CBoss::CBoss(float x, float y)
	: m_isHit(false), m_isSkill(false), m_isAttack(false), m_isDeadAnimation(false), m_actionTime(GetTickCount()), m_actionSpeed(1500), m_hitTime(GetTickCount())
{
	m_info.xPos = x;
	m_info.yPos = y;

	Init();
}


CBoss::~CBoss()
{
	Release();
}

void CBoss::Init()
{
	m_info.xSize = 70;
	m_info.ySize = 100;

	m_hp = 200;
	m_atk = 10;

	m_speed = 5.f;

	ExecuteScene(BOSS_SCENE::IDLE);

	target = CObjManager::GetInstance()->GetPlayer();

	UpdateRect();
}

int CBoss::Update()
{
	if (m_isDeadAnimation)
	{
		bool isEnd = UpdateFrame();

		if (isEnd)
		{
			m_isDead = true;

			SpawnDeadTrail();
		}
	}
	else
	{
		if (m_isHit)
		{
			if (m_hitTime + 300 < GetTickCount())
			{
				m_isHit = false;
				ChangeScene(BOSS_SCENE::IDLE);
				m_actionTime = GetTickCount();
				m_actionSpeed = 800;
			}
		}
		else if (m_isSkill)
		{
			PortalProcess();
		}
		else
		{
			if (m_isAttack) AttackProcess();

			UpdateFrame();

			if (nullptr != target) CalcDistance();
		}
	}

	if (m_isDead) return DEAD_EVENT;

	return NO_EVENT;
}

void CBoss::LateUpdate()
{
	if (m_hp <= 0)
	{
		m_isDeadAnimation = true;
		ChangeAction(EX_DIR::DOWN, BOSS_SCENE::DIE);
	}

	UpdateRect();
}

void CBoss::Render(HDC hDC)
{
	float x = m_info.xPos - 110.f;
	float y = m_info.yPos - 110.f;

	if (m_isHit)
	{
		if (GetTickCount() & 1)
		{
			HDC memDC = GetDCByDirection();

			TransparentBlt(hDC, x - CCamera::GetX(), y - CCamera::GetY(), 220, 200, memDC, m_frame.startFrame * 220, m_frame.sceneFrame * 220, 220, 220, RGB(64, 170, 226));
		}
	}
	else
	{
		HDC memDC = GetDCByDirection();

		TransparentBlt(hDC, x - CCamera::GetX(), y - CCamera::GetY(), 220, 200, memDC, m_frame.startFrame * 220, m_frame.sceneFrame * 220, 220, 220, RGB(64, 170, 226));
	}

	HDC UIDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_UI"));
	HDC HPDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_HP"));

	float xPos = (WINCX / 2) - 115.f;
	float yPos = 0;

	float hpPercent = 159 * m_hp / 200.f;

	TransparentBlt(hDC, xPos, yPos, 229, 46, UIDC, 0, 0, 229, 46, RGB(255,0,255));
	BitBlt(hDC, xPos + 65, yPos + 21, hpPercent, 15, HPDC, 0, 0, SRCCOPY);
}

void CBoss::Release()
{
}

void CBoss::Damaged(int damage)
{
	if (!m_isHit)
	{
		if (nullptr == target) return;
		INFO targetInfo = target->GetInfo();

		m_hp -= damage;
		m_isHit = true;
		m_isAttack = false;
		m_hitTime = GetTickCount();

		LookAtTarget();

		CDamageFontManager::CreateDamageFont(m_info.xPos, m_info.yPos, damage);
	}
}

HDC CBoss::GetDCByDirection()
{
	HDC returnDC = nullptr;

	switch (m_direction)
	{
	case EX_DIR::LEFT: returnDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_Left")); break;
	case EX_DIR::LD: returnDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_LD")); break;
	case EX_DIR::DOWN: returnDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_Down")); break;
	case EX_DIR::RD: returnDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_RD")); break;
	case EX_DIR::RIGHT: returnDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_Right")); break;
	case EX_DIR::RU: returnDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_RU")); break;
	case EX_DIR::LU: returnDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_LU")); break;
	case EX_DIR::UP: returnDC = CBitmapManager::GetInstance()->GetDC(__T("Boss_Up")); break;
	}

	return returnDC;
}

void CBoss::CalcDistance()
{
	if (nullptr == target) return;
	INFO targetInfo = target->GetInfo();

	float xDist = targetInfo.xPos - m_info.xPos;
	float yDist = targetInfo.yPos - m_info.yPos;
	float dist = sqrtf(xDist * xDist + yDist * yDist);

	if (m_actionTime + m_actionSpeed < GetTickCount())
	{
		LookAtTarget();

		if (!m_isAttack)
		{
			if (dist < 100) Attack();
			else if (dist < 200) WalkToTarget();
			else if (dist < 500) Portal();
		}
	}
}

void CBoss::Attack()
{
	if (!m_isAttack)
	{
		m_isAttack = true;
		ExecuteScene(BOSS_SCENE::ATTACK);
		m_actionSpeed = 1500;
	}
}

void CBoss::Portal()
{
	if (!m_isSkill)
	{
		m_isSkill = true;
		ExecuteScene(BOSS_SCENE::PORTAL);
		m_actionSpeed = 1500;
	}
}

void CBoss::AttackProcess()
{
	bool isEnd = UpdateFrame();

	if (BOSS_FRAME::ATTACK_SPAWNTIME == m_frame.startFrame && !m_isAttackSpawn)
	{
		if (nullptr == target) return;
		INFO targetInfo = target->GetInfo();

		CObj* newCollide = new CCollideRect(targetInfo.xPos, targetInfo.yPos, 50, 50, m_atk);
		CObjManager::GetInstance()->AddObject(newCollide, OBJ::ENEMY_ATTACK_ONE);

		m_isAttackSpawn = true;
	}

	if (isEnd && m_isAttack)
	{
		m_isAttack = false;
		m_isAttackSpawn = false;
		m_actionTime = GetTickCount();
		ChangeScene(BOSS_SCENE::IDLE);
	}
}

void CBoss::PortalProcess()
{
	bool isEnd = UpdateFrame();

	if (isEnd && m_isSkill)
	{
		m_isSkill = false;

		m_actionTime = GetTickCount();
		ChangeScene(BOSS_SCENE::IDLE);

		CPlayer* player = dynamic_cast<CPlayer*>(target);
		if (nullptr == player) return;

		EX_DIR::TAG bossDir = player->GetDirection();

		INFO targetInfo = target->GetInfo();

		float xPos = targetInfo.xPos;
		float yPos = targetInfo.yPos;

		float distance = 100.f;
		float shortDistance = distance / sqrtf(2.f);

		switch (bossDir)
		{
		case EX_DIR::LEFT:
			xPos += distance;
			break;
		case EX_DIR::LD:
			xPos += shortDistance;
			yPos -= shortDistance;
			break;
		case EX_DIR::DOWN:
			yPos -= distance;
			break;
		case EX_DIR::RD:
			xPos -= shortDistance;
			yPos -= shortDistance;
			break;
		case EX_DIR::RIGHT: 
			xPos -= distance;
			break;
		case EX_DIR::RU: 
			xPos -= shortDistance;
			yPos += shortDistance;
			break;
		case EX_DIR::LU: 
			xPos += shortDistance;
			yPos += shortDistance;
			break;
		case EX_DIR::UP:
			yPos += distance;
			break;
		}

		m_info.xPos = xPos;
		m_info.yPos = yPos;
		m_direction = bossDir;

		CObj* newCollide = new CCollideRect(targetInfo.xPos, targetInfo.yPos, 50, 50, m_atk);
		CObjManager::GetInstance()->AddObject(newCollide, OBJ::ENEMY_ATTACK_ONE);
	}
}

void CBoss::LookAtTarget()
{
	if (nullptr == target) return;
	INFO targetInfo = target->GetInfo();

	float xDist = targetInfo.xPos - m_info.xPos;
	float yDist = targetInfo.yPos - m_info.yPos;
	float dist = sqrtf(xDist * xDist + yDist * yDist);

	if (0 > xDist && 0 > yDist) m_direction = EX_DIR::LU;
	if (0 <= xDist && 0 > yDist) m_direction = EX_DIR::RU;
	if (0 > xDist && 0 <= yDist) m_direction = EX_DIR::LD;
	if (0 <= xDist && 0 <= yDist) m_direction = EX_DIR::RD;

	float radian = acosf(xDist / dist); // ���� ��

	if (0.f < yDist) radian *= -1;

	float degree = RADIAN_TO_DEGREE(radian);

	float offset = 15.f;

	if (0.f - offset <= degree && degree <= 0.f + offset) m_direction = EX_DIR::RIGHT;
	else if (90.f - offset <= degree && degree <= 90.f + offset) m_direction = EX_DIR::UP;
	else if (180.f - offset <= degree || degree <= -180.f + offset) m_direction = EX_DIR::LEFT;
	else if (-90.f - offset <= degree && degree <= -90.f + offset) m_direction = EX_DIR::DOWN;
}

void CBoss::WalkToTarget()
{
	if (nullptr == target) return;
	INFO targetInfo = target->GetInfo();

	float xDist = targetInfo.xPos - m_info.xPos;
	float yDist = targetInfo.yPos - m_info.yPos;
	float dist = sqrtf(xDist * xDist + yDist * yDist);

	ChangeScene(BOSS_SCENE::WALK);
	float radian = acosf(xDist / dist); // ���� ��

	if (0.f < yDist) radian *= -1;

	m_info.xPos += cosf(radian) * m_speed;
	m_info.yPos -= sinf(radian) * m_speed;
}

void CBoss::SpawnDeadTrail()
{
	CObj* deadTrail = new CMotionTrail(m_info.xPos, m_info.yPos, 220, 220, 110, 110, m_frame.endFrame - 1, m_frame.sceneFrame, RGB(64, 170, 226), __T("Boss_Down"));
	CObjManager::GetInstance()->AddObject(deadTrail, OBJ::TRAIL);
}

void CBoss::ChangeAction(EX_DIR::TAG dir, int scene)
{
	if (dir != m_direction) m_direction = dir;
	ChangeScene(scene);
}

void CBoss::ChangeScene(int scene)
{
	if (m_frame.sceneFrame != scene) ExecuteScene(scene);
}

void CBoss::ExecuteScene(int scene)
{
	m_frame.sceneFrame = scene;
	m_frame.startFrame = BOSS_FRAME::START[scene];
	m_frame.endFrame = BOSS_FRAME::END[scene];
	m_frame.frameTime = GetTickCount();
	m_frame.frameSpeed = BOSS_FRAME::SPEED[scene];
}