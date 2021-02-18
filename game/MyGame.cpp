#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void)
{
	
}

CMyGame::~CMyGame(void)
{

}

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();
	for (CSprite* balls : ballList)
	{
		// -10 is just feels too slow
		balls->Accelerate(0, -25);

		AttractMouse(*balls);

		ScreenCollider(*balls);

		MovingPlatformCollider(*balls);

		StaticRectangle(*balls);

		BallXBallCollision(*balls);
		
		StaticCircle(*balls);

		balls->Update(t);
	}

	MovingPlatform();

	CircleSize();

	CubeSize();

	for (CSprite* obstacles : obstacleList) obstacles->Update(t);
	for (CSprite* circles : circularObsacleList) circles->Update(t);

	ballList.delete_if(deleted);
	circularObsacleList.delete_if(deleted);
	obstacleList.delete_if(deleted);
}

void CMyGame::OnDraw(CGraphics* g)
{	
	for (CSprite* balls : ballList) balls->Draw(g);
	for (CSprite* obstacles : obstacleList) obstacles->Draw(g);
	for (CSprite* circles : circularObsacleList) circles->Draw(g);

	if (!informatioBool)
	{
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 90) << "Press [F1] for keymap!";
	}
	else
	{
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 90) << "Press [F2] for moving floor";
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 130) << "Press [F3] for changing mode (random size / small size ball)";
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 170) << "Press [Q] to reset the scene";
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 210) << "Press [E] to spawn bouncy balls";
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 250) << "Press [R] to spawn static circle / Press [F] to delete it";
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 290) << "Press [T] to spawn static rectangle / Press [G] to delete it";
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 330) << "Hold [RMB] on any static object to change size";
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 370) << "press [MMB] to attract the balls (probably the way you (Jarek) meant in the tasks)";
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 410) << "Hold [LMB] to pull/attract the balls fully to the mouse position";
		*g << font("ChavaRegular.ttf", 25) << color(CColor::White()) << xy(20, GetHeight() - 450) << "Press [ALT] + [F4] to exit";
	}

	*g << font("ChavaRegular.ttf", 30) << color(CColor(212, 176, 61, 255)) << xy(20, GetHeight() - 50) << "Ball Number: " << ballNumber;

	//FPS counter from you (Jarek)
	static int prevT = GetTime();
	if (GetTime() > prevT)
		*g << left << xy(GetWidth()-170, GetHeight() - 50)<< font("ChavaRegular.ttf", 30) << color(CColor::White()) << "fps = " << (int)(1000 / (GetTime() - prevT));
	prevT = GetTime();
}

void CMyGame::OnInitialize()
{
	MovingPlatformInit();
}

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();

	if (sym == SDLK_F1)
	{
		if (!informatioBool) informatioBool = true;
		else informatioBool = false;
	}

	if (sym == SDLK_F2)
	{
		if (!platformBool) platformBool = true;
		else platformBool = false;
	}

	if (sym == SDLK_F3)
	{
		if (!modeBool) modeBool = true;
		else modeBool = false;
	}

	if (sym == SDLK_q)
	{
		ballList.delete_all();
		obstacleList.delete_all();
		circularObsacleList.delete_all();
		ballNumber = 0;

		MovingPlatformInit();
	}

	if (sym == SDLK_e)
	{
		int ballSize = 0;
		int spawnNumber = 0;

		if (!modeBool)
		{
			ballSize = rand() % 20 + 50;
			spawnNumber = 1;
		}
		else
		{
			ballSize = 10;
			spawnNumber = 50;
		}

		for (size_t i = 0; i < spawnNumber; i++)
		{
			if (ballList.size() < 750) 
			{
				CColor randomColor = CColor(rand() % 255, rand() % 255, rand() % 255);
				CSprite* ball;
				ballList.push_back(ball = new CSpriteOval(GetMouseCoords().m_x, GetMouseCoords().m_y, ballSize, ballSize, randomColor, GetTime()));
				ball->SetVelocity(rand() % 200 - 100, rand() % 200 - 100);
				ballNumber++;
			}
		}
	}

	CColor randomColor = CColor(rand() % 255, rand() % 255, rand() % 255);

	if (sym == SDLK_r)
	{
		CSprite* circle;
		circularObsacleList.push_back(circle = new CSpriteOval(GetMouseCoords().m_x, GetMouseCoords().m_y, GetHeight()/4, GetHeight() / 4, randomColor, GetTime()));
	}

	if (sym == SDLK_t)
	{
		float size = GetHeight() / 4;
		CSprite* obstacle;
		obstacleList.push_back(obstacle = new CSpriteRect(GetMouseCoords().m_x, GetMouseCoords().m_y, size, size, randomColor, GetTime()));
		obstacle->SetRotation(0);
		obstacle->SetState(2);
	}
}
void CMyGame::OnMButtonDown(Uint16 x, Uint16 y)
{
	for (CSprite* balls : ballList)
	{
		CVector mousePos = CVector(GetMouseCoords().m_x, GetMouseCoords().m_y);
		CVector position = CVector(mousePos - balls->GetPosition());

		balls->SetVelocity(position * 2);
	}
}

float CMyGame::LookAt(float targetX, float targetY, float sourceX, float sourceY)
{
	float X = targetX - sourceX;
	float Y = targetY - sourceY;

	float angle = atan2(X, Y) * 180.0f / 3.14f;

	return angle - 90;
}

float CMyGame::Distance(float targetX, float targetY, float sourceX, float sourceY)
{
	float distance = sqrt(pow(targetX - sourceX, 2) + pow(targetY - sourceY, 2));

	return distance;
}

float CMyGame::DistanceF(float target, float source)
{
	float distance = target - source;

	return fabsf(distance);
}

float CMyGame::BallDistance(float target, float source)
{
	float distance = (target / 2) + (source / 2);

	return distance;
}

void CMyGame::MovingPlatformInit()
{
	CSprite* obstacle;
	obstacleList.push_back(obstacle = new CSpriteRect(GetWidth() / 2, -160, GetWidth() + 200, 130, CColor::DarkCyan(), GetTime()));
	obstacle->SetRotation(0);
	obstacle->SetOmega(10);
	obstacle->SetState(1);
	rotateBool = false;
}

void CMyGame::AttractMouse(CSprite& csprite)
{
	if (IsLButtonDown())
	{
		float lookAt = LookAt(GetMouseCoords().m_x, GetMouseCoords().m_y, csprite.GetX(), csprite.GetY()) + 90;
		float distance = Distance(GetMouseCoords().m_x, GetMouseCoords().m_y, csprite.GetX(), csprite.GetY());

		// Straight line attraction V1
		
		csprite.SetDirection(lookAt);
		csprite.SetSpeed(distance * 10);
		
		// Straight line attraction V2 (similar effect to the first one, but different solution)
		/*
		CVector mousePos = CVector(GetMouseCoords().m_x, GetMouseCoords().m_y);
		CVector position = CVector(mousePos - csprite.GetPosition());

		csprite.SetVelocity(position * 10);
		*/
		// Arched line attraction (found out by accident, but looks cool)
		// Have to comment out the Straight line attraction to make it work
		/*
		float xN = sin(DEG2RAD(lookAt));
		float yN = cos(DEG2RAD(lookAt));
		CVector n = CVector(xN, yN);
		csprite.SetVelocity(n);
		csprite.Accelerate(n * 1500);
		*/
	}
}

void CMyGame::ScreenCollider(CSprite& csprite)
{
	CVector vector = csprite.GetVelocity();
	CVector vectorN = Normalise(csprite.GetVelocity());
	CVector n = CVector(0, 1);
	if (csprite.GetY() < 0 + csprite.GetHeight() / 2)
	{
		if ((Dot(vectorN, n) < 0))
		{
			vector = 0.75 * Reflect(vector, n);
			csprite.SetY(csprite.GetH() / 2 - Normalise(vector).m_y);
		}
	}
	n = CVector(0, 1);
	if (csprite.GetY() > GetHeight() - csprite.GetHeight() / 2)
	{
		if ((Dot(vectorN, n) > 0))
		{
			vector = 0.75 * Reflect(vector, n);
			csprite.SetPosition(csprite.GetPosition() - vectorN + Normalise(csprite.GetVelocity()) * 2);
		}
	}
	n = CVector(1, 0);
	if (csprite.GetX() < 0 + csprite.GetHeight() / 2)
	{
		if ((Dot(vectorN, n) < 0))
		{
			vector = 0.75 * Reflect(vector, n);
			csprite.SetPosition(csprite.GetPosition() - vectorN + Normalise(csprite.GetVelocity()) * 2);
		}
	}
	n = CVector(1, 0);
	if (csprite.GetX() > GetWidth() - csprite.GetHeight() / 2)
	{
		if ((Dot(vectorN, n) > 0))
		{
			vector = 0.75 * Reflect(vector, n);
			csprite.SetPosition(csprite.GetPosition() - vectorN + Normalise(csprite.GetVelocity()) * 2);
		}
	}
	csprite.SetVelocity(vector);
}

void CMyGame::MovingPlatformCollider(CSprite& csprite)
{
	for (CSprite* obstacles : obstacleList)
	{
		CVector vector = csprite.GetVelocity();
		CVector vectorN = Normalise(csprite.GetVelocity());
		float xN = sin(DEG2RAD(obstacles->GetRotation()));
		float yN = cos(DEG2RAD(obstacles->GetRotation()));
		CVector n = CVector(0, 0);

		n = CVector(xN, yN);

		if (csprite.HitTest(obstacles) && obstacles->GetState() == 1 && (Dot(vectorN, n) < 0))
		{
			vector = 0.75 * Reflect(vector, n);
			csprite.SetVelocity(vector);
			csprite.SetPosition(csprite.GetPosition() - vectorN + Normalise(csprite.GetVelocity()) * 10);
		}
	}
}

void CMyGame::MovingPlatform()
{
	for (CSprite* obstacles : obstacleList)
	{
		if (obstacles->GetState() == 1)
		{
			if (obstacles->GetRotation() > 5 && !rotateBool)
			{
				obstacles->SetOmega(-5);
				rotateBool = true;
			}
			if (obstacles->GetRotation() < -5 && rotateBool)
			{
				obstacles->SetOmega(5);
				rotateBool = false;
			}

			if (platformBool)
			{
				obstacles->SetVelocity(0, 100);
				if (obstacles->GetY() > 0)
				{
					obstacles->SetVelocity(0, 0);
				}
			}
			else
			{
				obstacles->SetVelocity(0, -100);
				if (obstacles->GetY() < -160)
				{
					obstacles->SetVelocity(0, 0);
				}
			}
		}
	}

}

void CMyGame::StaticRectangle(CSprite& csprite)
{
	for (CSprite* obstacles : obstacleList)
	{
		CVector vector = csprite.GetVelocity();
		CVector vectorN = Normalise(csprite.GetVelocity());
		float xN = sin(DEG2RAD(obstacles->GetRotation()));
		float yN = cos(DEG2RAD(obstacles->GetRotation()));
		CVector n = CVector(0, 0);

		CVector position = obstacles->GetPosition() - csprite.GetPosition();
		CVector nP = Normalize(position);

		if (csprite.HitTest(obstacles) && obstacles->GetState() == 2)
		{
			n = CVector(xN, yN);
			if (Dot(vectorN, n) < 0 && csprite.GetY() - vectorN.m_y > obstacles->GetY() &&
				csprite.GetX() - vectorN.m_x < obstacles->GetX() + obstacles->GetWidth() / 2 &&
				csprite.GetX() - vectorN.m_x > obstacles->GetX() - obstacles->GetWidth() / 2)
			{
				vector = 0.75 * Reflect(vector, n);
				csprite.SetPosition(csprite.GetPosition() - nP * 2);
			}
			n = CVector(xN, yN);
			if (Dot(vectorN, n) > 0 && csprite.GetY() - vectorN.m_y < obstacles->GetY() &&
				csprite.GetX() - vectorN.m_x < obstacles->GetX() + obstacles->GetWidth() / 2 &&
				csprite.GetX() - vectorN.m_x > obstacles->GetX() - obstacles->GetWidth() / 2)
			{
				vector = 0.75 * Reflect(vector, n);
				csprite.SetPosition(csprite.GetPosition() - nP * 2);
			}
			n = CVector(yN, xN);
			if (Dot(vectorN, n) < 0 && csprite.GetX() - vectorN.m_x > obstacles->GetX() &&
				csprite.GetY() - vectorN.m_y < obstacles->GetY() + obstacles->GetHeight() / 2 &&
				csprite.GetY() - vectorN.m_y > obstacles->GetY() - obstacles->GetHeight() / 2)
			{
				vector = 0.75 * Reflect(vector, n);
				csprite.SetPosition(csprite.GetPosition() - nP * 2);
			}
			n = CVector(yN, xN);
			if (Dot(vectorN, n) > 0 && csprite.GetX() - vectorN.m_x < obstacles->GetX() &&
				csprite.GetY() - vectorN.m_y < obstacles->GetY() + obstacles->GetHeight() / 2 &&
				csprite.GetY() - vectorN.m_y > obstacles->GetY() - obstacles->GetHeight() / 2)
			{
				vector = 0.75 * Reflect(vector, n);
				csprite.SetPosition(csprite.GetPosition() - nP * 2);
			}
			csprite.SetVelocity(vector);
		}
	}
}

void CMyGame::CubeSize()
{
	for (CSprite* obstacles : obstacleList)
	{
		if (obstacles->GetState() == 2)
		{
			float distanceX = 0;
			float distanceY = 0;
			if (IsRButtonDown() && obstacles->HitTest(GetMouseCoords().m_x, GetMouseCoords().m_y))
			{
				distanceX = DistanceF(GetMouseCoords().m_x, obstacles->GetX()) * 4;
				distanceY = DistanceF(GetMouseCoords().m_y, obstacles->GetY()) * 4;
				if (distanceX < 50) distanceX = 50;
				if (distanceY < 50) distanceY = 50;
				obstacles->SetSize(distanceX, distanceY);
			}
			if (IsKeyDown(SDLK_g) && obstacles->HitTest(GetMouseCoords().m_x, GetMouseCoords().m_y)) obstacles->Delete();
		}
	}
}

void CMyGame::StaticCircle(CSprite& csprite)
{
	for (CSprite* circles : circularObsacleList)
	{
		CVector vA = csprite.GetVelocity();
		CVector vB = circles->GetVelocity();

		CVector position = circles->GetPosition() - csprite.GetPosition();

		float distance = Distance(csprite.GetX(), csprite.GetY(), circles->GetX(), circles->GetY());
		float ballDistance = BallDistance(csprite.GetW(), circles->GetW());

		if (distance < ballDistance && (Dot(vB - vA, position) < 0))
		{
			CVector n = Normalize(position);

			CVector u = Dot(vA - vB, n) * n;
			csprite.Accelerate(-u);
			csprite.SetVelocity(-u * 0.75);
			csprite.SetPosition(csprite.GetPosition() - n * 2);
		}
	}
}

void CMyGame::CircleSize()
{
	for (CSprite* circles : circularObsacleList)
	{
		float distance = 0;
		if (IsRButtonDown() && circles->HitTest(GetMouseCoords().m_x, GetMouseCoords().m_y))
		{
			distance = DistanceF(GetMouseCoords().m_x, circles->GetX()) * 4;
			if (distance < 50) distance = 50;
			circles->SetSize(distance, distance);
		}
		if (IsKeyDown(SDLK_f) && circles->HitTest(GetMouseCoords().m_x, GetMouseCoords().m_y)) circles->Delete();
	}
}

void CMyGame::BallXBallCollision(CSprite& csprite)
{
	for (CSprite* balls : ballList)
	{
		CVector vA = csprite.GetVelocity();
		CVector vB = balls->GetVelocity();

		CVector position = balls->GetPosition() - csprite.GetPosition();

		float distance = Distance(csprite.GetX(), csprite.GetY(), balls->GetX(), balls->GetY());
		float ballDistance = BallDistance(csprite.GetW(), balls->GetW());

		if (distance < ballDistance && (Dot(vB - vA, position) < 0))
		{
			CVector n = Normalize(position);

			CVector u = Dot(vA - vB, n) * n;
			csprite.Accelerate(-u);
			balls->Accelerate(u);
			csprite.SetPosition(csprite.GetPosition() - n);
			balls->SetPosition(balls->GetPosition() + n);
		}
	}
}