#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Graphics4/PipelineState.h>
#include <Kore/Graphics4/Shader.h>
#include <Kore/Graphics1/Color.h>
#include <Kore/System.h>
#include <Kore/Log.h>

#include "Tileset.h"
#include "Settings.h"
#include "Collision.h"
#include "PhysicsWorld.h"
#include "PhysicsObject.h"

using namespace Kore;

namespace {
	const int width = tileWidth * mapColumns;
	const int height = tileHeight * mapRows;
	const int scale = 2;
	
	Status playerStatus = Status::Standing;
	bool left, right, jump;
	
	Graphics2::Graphics2* g2;
	
	PhysicsWorld physics;
	PhysicsObject* player;
	PhysicsObject* coin;
	
	const int moveDistance = 10;
	vec3 playerPosition = vec3(0, 0, 0);
	vec3 cameraPosition = vec3(0, 0, 0);
	
	vec3 coinPosition = vec3(tileWidth, (mapRows - 2) * tileHeight, 0);
	
	double startTime;
	double lastTime;
	
	void printPosition() {
		vec3 playerPosition = player->GetPosition();
		log(LogLevel::Info, "playerPosition %f %f %f", playerPosition.x(), playerPosition.y(), playerPosition.z());
		
		log(LogLevel::Info, "coinPosition %f %f %f", coinPosition.x(), coinPosition.y(), coinPosition.z());
	}

	void update() {
		double t = System::time() - startTime;
		double deltaT = t - lastTime;
		//Kore::log(Info, "%f", deltaT);
		lastTime = t;
		
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag);
		
		playerPosition = player->GetPosition();
		coinPosition = coin->GetPosition();
		
		// Move character
		if (left && playerPosition.x() > 0) {
			playerStatus = WalkingLeft;
			player->ApplyImpulse(vec3(-moveDistance, 0, 0));
			jump = false;
		} else if (right && playerPosition.x() < mapColumns * tileWidth - tileWidth) {
			playerStatus = WalkingRight;
			player->ApplyImpulse(vec3(moveDistance, 0, 0));
			jump = false;
		} else if (jump) {
			playerStatus = JumpingLeft;
			player->ApplyImpulse(vec3(0, -moveDistance, 0));
		} else {
			playerStatus = Standing;
		}
		
		// Update the physics and render
		physics.Update(deltaT);

		g2->begin();
		drawTiles(g2, cameraPosition);
		animate(playerStatus, g2, cameraPosition, playerPosition);
		drawSingleTile(g2, cameraPosition, coinPosition, Dollar);
		
		
		// Debug: show bounding box
		PhysicsObject** currentP = &physics.physicsObjects[0];
		while (*currentP != nullptr) {
			(*currentP)->drawBoundingBox(g2);
			++currentP;
		}
		
		g2->end();

		Graphics4::end();
		Graphics4::swapBuffers();
	}
	
	void SpawnPlayer(vec3 position, vec3 velocity) {
		player = new PhysicsObject();
		player->SetPosition(position);
		player->velocity = velocity;
		player->sphereCollider.radius = tileWidth / 2;
		player->Mass = 5;
		player->ApplyImpulse(velocity);
		physics.AddObject(player);
	}
	
	void SpawnCoins(vec3 position) {
		coin = new PhysicsObject();
		coin->SetPosition(position);
		coin->sphereCollider.radius = tileWidth / 2;
		coin->Mass = 5;
		physics.AddObject(coin);
	}
	
	void keyDown(KeyCode code) {
		switch (code) {
			case KeyLeft:
			case KeyA:
				left = true;
				break;
			case KeyRight:
			case KeyD:
				right = true;
				break;
			case KeySpace: {
				jump = true;
				break;
			}
			case KeyEscape:
			case KeyQ:
			case KeyReturn:
				Kore::System::stop();
				break;
			default:
				break;
		}
	}
	
	void keyUp(KeyCode code) {
		switch (code) {
			case KeyLeft:
			case KeyA:
				left = false;
				break;
			case KeyRight:
			case KeyD:
				right = false;
				break;
			case KeySpace:
				//jump = false;
				break;
			default:
				break;
		}
	}
}

int kore(int argc, char** argv) {
	Kore::System::init("Mini2DEngine", width * scale, height * scale);
	Kore::System::setCallback(update);
	
	startTime = System::time();
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	
	g2 = new Graphics2::Graphics2(width, height, false);
	//g2->setImageScaleQuality(Graphics2::Low);
	
	initTiles("tileset/map2.csv", "tileset/tileset2.png");
	int posX;
	int posY;
	getTilePosition(TileID::Stand, posX, posY);
	playerPosition = vec3(posX, posY, 0);
	SpawnPlayer(playerPosition, vec3(0, 0, 0));
	
	SpawnCoins(coinPosition);

	Kore::System::start();

	return 0;
}
