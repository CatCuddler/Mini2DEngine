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
	PhysicsObject** coins;
	int coinNum = 0;
	int cointCollected = 0;
	
	const int velocity = 50;
	
	vec3 playerPosition = vec3(0, 0, 0);
	vec3 cameraPosition = vec3(0, 0, 0);
	vec3 inventoryPosition = vec3(0, 0, 0);
	
	Kravur* font14;
	Kravur* font24;
	Kravur* font34;
	Kravur* font44;
	
	double startTime;
	double lastTime;
	
	bool collectCoins(vec3 playerPosition, vec3 coinPosition) {
		if ((playerPosition - coinPosition).getLength() < 20) {
			++cointCollected;
			return true;
		}
		return false;
	}
	
	bool feetOnTheGround() {
		// Calculate max distance of the tiles when they are beside each other
		vec3 pos1 = vec3(0, 0, 0);
		vec3 pos2 = vec3(tileWidth/2, tileHeight, 0);
		float dist = (pos1 - pos2).getLength();
		
		BoxCollider** bc = &physics.boxColliders[0];
		while (*bc != nullptr) {
			vec3 pos = (*bc)->position;
			if ((playerPosition - pos).getLength() <= dist) {
				log(LogLevel::Info, "On the ground");
				return true;
			}
			++bc;
		}
		return false;
	}
	
	void drawGUI(const char* text) {
		if (text != nullptr) {
			g2->drawString(text, 10, tileHeight);
		}
		
		g2->setColor(Graphics1::Color::White);
	}
	
	void update() {
		double t = System::time() - startTime;
		double deltaT = t - lastTime;
		//Kore::log(Info, "%f", deltaT);
		lastTime = t;
		
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag);
		
		playerPosition = player->GetPosition();
		
		// Move player only if the feet are on the ground
		if (feetOnTheGround()) {
			playerStatus = Standing;
			if (left && playerPosition.x() > 0) {
				playerStatus = WalkingLeft;
				player->ApplyImpulse(vec3(-velocity, 0, 0));
			} else if (right && playerPosition.x() < mapColumns * tileWidth - tileWidth) {
				playerStatus = WalkingRight;
				player->ApplyImpulse(vec3(velocity, 0, 0));
			}
			
			if (jump) {
				if (left) {
					playerStatus = JumpingLeft;
					player->ApplyImpulse(vec3(-velocity, 0, 0));
				} else if (right) {
					playerStatus = JumpingRight;
					player->ApplyImpulse(vec3(velocity, 0, 0));
				}
				player->ApplyImpulse(vec3(0, -700, 0));
				
				jump = false;
			}
		}
		
		// Update the physics and render
		physics.Update(deltaT);
		
		g2->begin();
		
		// Draw background
		drawTiles(g2, cameraPosition);
		
		// Draw coins
		for (int i = 0; i < coinNum; i++) {
			vec3 pos = coins[i]->GetPosition();
			drawSingleTile(g2, cameraPosition, pos, TileID::Dollar);
			
			if (collectCoins(playerPosition, pos)) {
				coins[i]->SetPosition(inventoryPosition + vec3(cointCollected * 2, 0, 0));
			}
		}
		
		animate(playerStatus, g2, cameraPosition, playerPosition);
		
		if (debug) {
			// Debug: show bounding box
			physics.DrawBoundingBox(g2);
		}
		
		// Draw GUI
		if (cointCollected > 0) {
			char coinText[42];
			sprintf(coinText, "Coins: %i", cointCollected);
			drawGUI(coinText);
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
		player->SetPosition(playerPosition);
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
			//physics.AddObject(coin);
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
				jump = false;
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
	
	font14 = Kravur::load("Fonts/arial", FontStyle(), 14);
	font24 = Kravur::load("Fonts/arial", FontStyle(), 24);
	font34 = Kravur::load("Fonts/arial", FontStyle(), 34);
	font44 = Kravur::load("Fonts/arial", FontStyle(), 44);
	
	g2->setFont(font14);
	g2->setFontColor(Graphics1::Color::White);
	g2->setFontSize(14);
	
	initTiles("tileset/map2.csv", "tileset/tileset2.png");
	spawnPlayer();
	spawnCoins();
	initBoxColliders();
	
	Kore::System::start();
	
	return 0;
}
