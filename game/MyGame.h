#pragma once

class CMyGame : public CGame
{
	// Define sprites and other instance variables here
	CSpriteList ballList;
	CSpriteList obstacleList;
	CSpriteList circularObsacleList;
	
	bool rotateBool = false;
	bool platformBool = false;
	bool modeBool = false;
	bool informatioBool = false;
	int ballNumber = 0;

public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnMButtonDown(Uint16 x, Uint16 y);

	float LookAt(float targetX, float targetY, float sourceX, float sourceY);
	float Distance(float targetX, float targetY, float sourceX, float sourceY);
	float DistanceF(float target, float source);
	float BallDistance(float target, float source);

	void MovingPlatformInit();

	void AttractMouse(CSprite& csprite);
	void ScreenCollider(CSprite& csprite);
	void MovingPlatformCollider(CSprite& csprite);
	void MovingPlatform();
	void StaticRectangle(CSprite& csprite);
	void CubeSize();
	void StaticCircle(CSprite& csprite);
	void CircleSize();
	void BallXBallCollision(CSprite& csprite);
};
