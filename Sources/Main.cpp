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
	bool lastDirectionLeft = false;
	
	Graphics2::Graphics2* g2;
	
	PhysicsWorld physics;
	PhysicsObject* player;
	PhysicsObject** coins;
	int coinNum;
	
	const int velocity = 10;
	vec3 playerPosition = vec3(0, 0, 0);
	vec3 cameraPosition = vec3(0, 0, 0);
	
	double startTime;
	double lastTime;
	
	void update() {
		double t = System::time() - startTime;
		double deltaT = t - lastTime;
		//Kore::log(Info, "%f", deltaT);
		lastTime = t;
		
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag);
		
		playerPosition = player->GetPosition();
		
		// Move character
		if (left && playerPosition.x() > 0) {
			playerStatus = WalkingLeft;
			player->ApplyImpulse(vec3(-velocity, 0, 0));
			lastDirectionLeft = true;
		} else if (right && playerPosition.x() < mapColumns * tileWidth - tileWidth) {
			playerStatus = WalkingRight;
			player->ApplyImpulse(vec3(velocity, 0, 0));
			lastDirectionLeft = false;
		} else if (jump) {
			int jumpImpulse = 200;
			if (lastDirectionLeft) {
				playerStatus = JumpingLeft;
				player->ApplyImpulse(vec3(-jumpImpulse, -jumpImpulse, 0));
			} else {
				playerStatus = JumpingRight;
				player->ApplyImpulse(vec3(jumpImpulse, -jumpImpulse, 0));
			}
			jump = false;
		} else {
			playerStatus = Standing;
		}
		
		// Update the physics and render
		physics.Update(deltaT);

		g2->begin();
		drawTiles(g2, cameraPosition);
		bool cycleFinished = animate(playerStatus, g2, cameraPosition, playerPosition);
		if (cycleFinished) {
			jump = false;
		}
		
		// Draw coins
		PhysicsObject** coin = &coins[0];
		for (int i = 0; i < coinNum; i++) {
			vec3 pos = (*coin)->GetPosition();
			drawSingleTile(g2, cameraPosition, pos, TileID::Dollar);
			++coin;
		}
		
		if (debug) {
			// Debug: show bounding box
			physics.DrawBoundingBox(g2);
		}
		
		g2->end();

		Graphics4::end();
		Graphics4::swapBuffers();
	}
	
	void spawnPlayer() {
		int posX;
		int posY;
		getTilePosition(TileID::Stand, posX, posY);
		playerPosition = vec3(posX, posY, 0);
		
		
		player = new PhysicsObject();
		player->SetPosition(vec3(0, 0, 0));//(playerPosition);
		player->velocity = vec3(0, 0, 0);
		player->Mass = 5;
		physics.AddObject(player);
	}
	
	void spawnCoins() {
		Kore::vec3 coinPositions[mapRows * mapColumns];
		getTiles(TileID::Dollar, coinPositions, coinNum);
		
		coins = new PhysicsObject*[coinNum];
		for (int i = 0; i < coinNum; i++) {
			PhysicsObject* coin = new PhysicsObject();
			coin->SetPosition(coinPositions[i]);
			coin->Mass = 5;
			coins[i] = coin;
			physics.AddObject(coin);
		}
	}
	
	void initBoxColliders() {
		int size;
		Kore::vec3 boxColliders[mapRows * mapColumns];
		getBoxColliders(boxColliders, size);
		
		for (int i = 0; i < size; ++i) {
			BoxCollider* boxCollider = new BoxCollider();
			boxCollider->position = boxColliders[i];
			boxCollider->width = tileWidth;
			boxCollider->height = tileHeight;
			physics.AddObject(boxCollider);
		}
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
	spawnPlayer();
	spawnCoins();
	initBoxColliders();

	Kore::System::start();

	return 0;
}
