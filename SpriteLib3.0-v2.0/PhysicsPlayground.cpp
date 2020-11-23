#include "PhysicsPlayground.h"
#include "Utilities.h"

#include <random>
#include <cmath>

PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{
	//No gravity this is a top down scene
	m_gravity = b2Vec2(0.f, -98.f);
	m_physicsWorld->SetGravity(m_gravity);

	m_physicsWorld->SetContactListener(&listener);
}

void PhysicsPlayground::InitScene(float windowWidth, float windowHeight)
{
	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	///////////////////////////////////Player, Camera, Misc////////////////////////////////////////////////
	{
		//Setup MainCamera Entity
		{
			/*Scene::CreateCamera(m_sceneReg, vec4(-75.f, 75.f, -75.f, 75.f), -100.f, 100.f, windowWidth, windowHeight, true, true);*/

			//Creates Camera entity
			auto entity = ECS::CreateEntity();
			ECS::SetIsMainCamera(entity, true);

			//Creates new orthographic camera
			ECS::AttachComponent<Camera>(entity);
			ECS::AttachComponent<HorizontalScroll>(entity);
			ECS::AttachComponent<VerticalScroll>(entity);

			vec4 temp = vec4(-75.f, 75.f, -75.f, 75.f);
			ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
			ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
			ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

			//Attaches the camera to vert and horiz scrolls
			ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
			ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		}

		//Player entity
		{

			auto entity = ECS::CreateEntity();
			ECS::SetIsMainPlayer(entity, true);

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<CanJump>(entity);

			//Sets up the components
			std::string fileName = "WhiteCircle.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 30, 30);
			ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 10.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(-20.f), float32(0.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			//tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
			tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight() - shrinkY) / 2.f), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);

			tempPhsBody.SetRotationAngleDeg(0.f);
			tempPhsBody.SetFixedRotation(true);
			tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
			tempPhsBody.SetGravityScale(1.f);
		}

		//Setup controls sign
		{
			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);

			//Set up the components
			std::string fileName = "Controls.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 128, 128);
			ECS::GetComponent<Sprite>(entity).SetTransparency(1.0f);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(-60.f, 15.f, 0.f));


			/*Scene::CreateSprite(m_sceneReg, "HelloWorld.png", 100, 60, 0.5f, vec3(0.f, 0.f, 0.f));*/
		}

		//Setup rotate sign
		{
			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);

			//Set up the components
			std::string fileName = "RotateOnly.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 80, 30);
			ECS::GetComponent<Sprite>(entity).SetTransparency(1.0f);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(250.f, -75.f, -1.f));


			/*Scene::CreateSprite(m_sceneReg, "HelloWorld.png", 100, 60, 0.5f, vec3(0.f, 0.f, 0.f));*/
		}
	}

	/////////////////////////////////////World & Platforms//////////////////////////////////////////////////
	{
		//Setup Background
		{
			//Create entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);

			//Set up component
			float backgroundScale = 20;
			std::string fileName = "LightestGraySquare.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 128 * backgroundScale, 128 * backgroundScale);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(-150.f, 40.f, -10.f));
		}


		//Initial area, player scale puzzle.
		{
			//Setup static Top Platform
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 150, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(30.f), float32(-10.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
					float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

			}

			//Upper roof 1
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 70, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -10.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(-10.f), float32(35.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
					float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

			}

			//Upper roof 2
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 25, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -10.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(37.f), float32(25.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
					float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

			}

			//Upper roof 3
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 65, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -10.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(82.f), float32(16.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
					float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

			}

			//Setup static RAMP
			{
			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//Sets up components
			std::string fileName = "GraySquare.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 80, 10);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(137.1f), float32(-29.3f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
				float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY, 0.8f);
			tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
			tempPhsBody.SetRotationAngleDeg(-30.f);
			}

			//Ramp roof
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 80, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(142.5f), float32(0.0f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
					float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY, 0.8f);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
				tempPhsBody.SetRotationAngleDeg(-30.f);
			}

			//Setup static low Platform
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 150, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(240.f), float32(-50.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
					float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
			}

			//Lower roof
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 150, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, 10.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(250.f), float32(-20.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
					float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
			}

			//Setup static Wall lower
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 110, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(325.f), float32(-70.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, ENVIRONMENT, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
				tempPhsBody.SetRotationAngleDeg(90.f);
			}

			//Upper wall
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 40, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(-40.f), float32(10.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, ENVIRONMENT, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
				tempPhsBody.SetRotationAngleDeg(90.f);
			}
		}

		//Second area, translation puzzle. 
		{ 
			//Landing platform
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 50, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -10.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(325.f), float32(-120.f)); //300,-120

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
					float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

			}

			//Outer ball containing wall
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 110, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(350.f), float32(-70.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, ENVIRONMENT, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
				tempPhsBody.SetRotationAngleDeg(90.f);
			}

			//Player containing wall
			{
				//Creates entity
				auto entity = ECS::CreateEntity();

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 50, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(300.f), float32(-70.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, ENVIRONMENT, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
				tempPhsBody.SetRotationAngleDeg(90.f);
			}

			//Player containing door
			{
				//Creates entity
				auto entity = ECS::CreateEntity();
				rotatePuzzleDoor = entity;

				//Add components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				//Sets up components
				std::string fileName = "GraySquare.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 50, 10);
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;
				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(300.f), float32(-100.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, ENVIRONMENT, PLAYER | ENEMY);
				tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
				tempPhsBody.SetRotationAngleDeg(90.f);
			}

		}
	}


	/////////////////////////////////////Interactables/////////////////////////////////////////////////////
	{
		//Scale puzzle Ball
		{
					auto entity = ECS::CreateEntity();
					ball = entity;
					//Add components
					ECS::AttachComponent<Sprite>(entity);
					ECS::AttachComponent<Transform>(entity);
					ECS::AttachComponent<PhysicsBody>(entity);

					//Sets up the components
					std::string fileName = "RedCircle.png";
					ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 20);
					ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
					ECS::GetComponent<Transform>(entity).SetPosition(vec3(45.f, -8.f, 3.f));

					auto& tempSpr = ECS::GetComponent<Sprite>(entity);
					auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

					float shrinkX = 0.f;
					float shrinkY = 0.f;

					b2Body* tempBody;
					b2BodyDef tempDef;
					tempDef.type = b2_dynamicBody;
					tempDef.position.Set(float32(45.f), float32(8.f));//45 //-8 
					
					tempBody = m_physicsWorld->CreateBody(&tempDef);
					tempBody->SetAwake(true);
					tempBody->SetSleepingAllowed(false);

					//tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false);
					tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth() - shrinkY) / 2.f), vec2(0.f, 0.f), false, OBJECTS, GROUND | ENVIRONMENT | PLAYER | TRIGGER, 0.3f);

					tempPhsBody.SetColor(vec4(25.f, 0.f, 1.f, 1.0f));}

		//Rotate puzzle Ball
		{
			auto entity = ECS::CreateEntity();
			ball = entity;
			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//Sets up the components
			std::string fileName = "RedCircle.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 10);
			ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(45.f, -8.f, 3.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(335.0f), float32(8.f));//335 //8 

			tempBody = m_physicsWorld->CreateBody(&tempDef);
			tempBody->SetAwake(true);
			tempBody->SetSleepingAllowed(false);

			//tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false);
			tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth() - shrinkY) / 2.f), vec2(0.f, 0.f), false, OBJECTS, GROUND | ENVIRONMENT | PLAYER | TRIGGER, 0.3f);

			tempPhsBody.SetColor(vec4(25.f, 0.f, 1.f, 1.0f)); }

		//Platform rotate 1
		{
			//Creates entity
			auto entity = ECS::CreateEntity();
			rotatePlate1 = entity;

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//Sets up components
			std::string fileName = "RedSquare.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -10.f, 2.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(345.f), float32(-50.f)); //300,-120

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
				float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
			tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

		}

		//Platform rotate 2
		{
			//Creates entity
			auto entity = ECS::CreateEntity();
			rotatePlate2 = entity;

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//Sets up components
			std::string fileName = "RedSquare.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -10.f, 2.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(345.f), float32(-70.f)); //300,-120

			tempBody = m_physicsWorld->CreateBody(&tempDef);


			tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
				float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
			tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

			tempPhsBody.SetRotationAngleDeg(90.0f);

		}

		//Platform rotate 3
		{
			//Creates entity
			auto entity = ECS::CreateEntity();
			rotatePlate3 = entity;

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//Sets up components
			std::string fileName = "RedSquare.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -10.f, 2.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(345.f), float32(-90.f)); //300,-120

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
				float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
			tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

		}

		//Setup trigger rotate puzzle
		{
			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Trigger*>(entity);

			//Sets up components
			std::string fileName = "YellowSquare.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 15, 2.5);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 80.f));
			ECS::GetComponent<Trigger*>(entity) = new DestroyTrigger();
			ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
			ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(rotatePuzzleDoor);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(337.7f), float32(-112.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
			tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
		}
	}
	


	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void PhysicsPlayground::Update()
{
	
}


////////////////////////////////////Controls////////////////////////////////////
void PhysicsPlayground::KeyboardHold()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& plrSprite = ECS::GetComponent<Sprite>(MainEntities::MainPlayer());
	auto& plrTrans = ECS::GetComponent<Transform>(MainEntities::MainPlayer());
	auto& rPlatOne = ECS::GetComponent<PhysicsBody>(rotatePlate1);
	auto& rPlatTwo = ECS::GetComponent<PhysicsBody>(rotatePlate2);
	auto& rPlatThree = ECS::GetComponent<PhysicsBody>(rotatePlate3);
	auto& tPlatOne = ECS::GetComponent<Transform>(rotatePlate1);

	float speed = 1.f;
	b2Vec2 vel = b2Vec2(0.f, 0.f);

	/*if (Input::GetKey(Key::Shift)) No sprinting it makes players float
	{
		speed *= 5.f;
	}*/

	if (Input::GetKey(Key::A))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(-400000.f * speed, 0.f), true); //-400000
	}
	if (Input::GetKey(Key::D))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(400000.f * speed, 0.f), true);//400000
	}

	//Change physics body size for circle
	if (Input::GetKey(Key::E))
	{
		player.ScaleBody(1.3 * Timer::deltaTime, 0);
		plrSprite.SetHeight(player.GetHeight());
		plrSprite.SetWidth(player.GetWidth());
	}
	else if (Input::GetKey(Key::Q))
	{
		player.ScaleBody(-1.3 * Timer::deltaTime, 0);
		plrSprite.SetHeight(player.GetHeight());
		plrSprite.SetWidth(player.GetWidth());
	}

	else if (Input::GetKey(Key::R))
	{
		rPlatOne.SetRotationAngleDeg(rPlatOne.GetRotationAngleDeg() + 20 * Timer::deltaTime);
		rPlatTwo.SetRotationAngleDeg(rPlatTwo.GetRotationAngleDeg() + 20 * Timer::deltaTime);
		rPlatThree.SetRotationAngleDeg(rPlatThree.GetRotationAngleDeg() + 20 * Timer::deltaTime);
	}

	else if (Input::GetKey(Key::T))
	{
		/*
		rPlatOne.SetRotationAngleDeg(rPlatOne.GetRotationAngleDeg() - 20 * Timer::deltaTime);
		rPlatTwo.SetRotationAngleDeg(rPlatTwo.GetRotationAngleDeg() - 20 * Timer::deltaTime);
		rPlatThree.SetRotationAngleDeg(rPlatThree.GetRotationAngleDeg() - 20 * Timer::deltaTime);
		*/
	}



}

void PhysicsPlayground::KeyboardDown()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());

	/*if (Input::GetKeyDown(Key::T))  The p key does this automatically
	{
		PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	}*/
	if (canJump.m_canJump)
	{
		if (Input::GetKeyDown(Key::Space))
		{
			player.GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.f, 160000.f), true);
			canJump.m_canJump = false;
		}
	}
}

void PhysicsPlayground::KeyboardUp()
{
	

}
