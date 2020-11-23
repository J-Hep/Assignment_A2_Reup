#pragma once

#include "Scene.h"
#include "PhysicsPlaygroundListener.h"

class PhysicsPlayground : public Scene
{
public:
	PhysicsPlayground(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

protected:
	PhysicsPlaygroundListener listener;

	int ball = 0;
	int rotatePlate1 = 0;
	int rotatePlate2 = 0;
	int rotatePlate3 = 0;
	int rotatePuzzleDoor = 0;
	int movingPlate = 0;
	int movingPlateWall1 = 0;
	int movingPlateWall2 = 0;
	int winCover = 0;

};
