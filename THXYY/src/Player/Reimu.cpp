#include "Reimu.h"
#include "ReimuBullet.h"
#include "ReimuBomb1.h"

Reimu::Reimu()
{
	auto stgResources = STGResources::GetInstance();

	frame = 0;
	frame_fire = 0;

	SetTexture(stgResources->texReimu);

	SetHiSpeed(4.5f);
	SetLowSpeed(2.0f);
	SetRadius(3.0f);
}

Reimu::~Reimu()
{

}

void Reimu::Update()
{
	Player::Update();

	frame++;

	if (motionState == STATIC || motionState == UP || motionState == DOWN)
	{
		if (frame >= 8 * ANIM_INTERVAL)
		{
			frame = 0;
		}
		int left = 32 * (frame / ANIM_INTERVAL);
		SetTexRect(Rect(left, left + 32, 0, 48));
	}
	else
	{
		if (motionState == LEFT || motionState == LEFT_UP || motionState == LEFT_DOWN)
		{
			if (prevState == STATIC || prevState == UP || prevState == DOWN)
			{
				frame = 0;
			}
			if (frame >= 8 * ANIM_INTERVAL)
			{
				frame = 4 * ANIM_INTERVAL;
			}
			int left = 32 * (frame / ANIM_INTERVAL);
			SetTexRect(Rect(left, left + 32, 48, 96));
		}
		else if (motionState == RIGHT || motionState == RIGHT_UP || motionState == RIGHT_DOWN)
		{
			if (prevState == STATIC || prevState == UP || prevState == DOWN)
			{
				frame = 0;
			}
			if (frame >= 4 * ANIM_INTERVAL + 4 * TURN_INTERVAL)
			{
				frame = 4 * TURN_INTERVAL;
			}

			int left;

			if (frame < 4 * TURN_INTERVAL)
			{
				left = 32 * (frame / TURN_INTERVAL);
			}
			else
			{
				left = 32 * ((frame - 4 * TURN_INTERVAL) / ANIM_INTERVAL) + 128;
			}
			SetTexRect(Rect(left, left + 32, 96, 144));
		}
	}
	prevState = motionState;
}

void Reimu::Fire()
{
	auto engine = STGEngine::GetInstance();

	if (frame_fire == 0)
	{
		STGResources::GetInstance()->soundPlayerShoot->Play();

		ReimuBullet* bullet1 = new ReimuBullet();
		bullet1->SetPosition(position.x - 7.0f, position.y);
		engine->ShootPlayerBullet(bullet1);

		ReimuBullet* bullet2 = new ReimuBullet();
		bullet2->SetPosition(position.x + 7.0f, position.y);
		engine->ShootPlayerBullet(bullet2);
	}

	frame_fire++;
	if (frame_fire == 3)
	{
		frame_fire = 0;
	}
}

void Reimu::Bomb()
{
	auto stgResources = STGResources::GetInstance();

	EnterBombState(210);
	SetInvincible(250);

	auto engine = STGEngine::GetInstance();

	for (int i = 0; i < 8; i++)
	{
		ReimuBomb1* bomb = new ReimuBomb1();
		bomb->SetInitialAngle(-45 * i + 7.5);
		bomb->SetAttackFrame(10 * i + 90);
		engine->AddObject(bomb);
	}

	stgResources->soundBomb->Play();
	stgResources->soundExplode->Play();
}