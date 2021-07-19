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
	const int scale = 5;
	
	CharacterState playerStatus = CharacterState::Standing;
	bool left, right, jump;
	
	int animIndex = 0;
	int animCount = 0;
	
	Graphics2::Graphics2* g2;
	
	PhysicsWorld* physics;
	BoxPhysicsObject* player;
	SpherePhysicsObject** coins;
	int coinNum = 0;
	int cointCollected = 0;
	
	vec3 playerPosition = vec3(0, 0, 0);
	vec3 cameraPosition = vec3(0, 0, 0);
	vec3 inventoryPosition = vec3(0, tileHeight, 0);
	
	Kravur* font14;
	Kravur* font24;
	Kravur* font34;
	Kravur* font44;
	
	double startTime;
	double lastTime;
	
	bool collectCoins() {
		for (int i = 0; i < coinNum; i++) {
			if (coins[i]->collider->IntersectsWith(*player->collider)) {
				//log(LogLevel::Info, "Collect coin at position %f %f", coins[i]->position.x(), coins[i]->position.y());
				coins[i]->position = inventoryPosition + vec3(cointCollected * 2, 0, 0);
				coins[i]->collider->center = coins[i]->position;
				++cointCollected;
				return true;
			}
		}
		return false;
	}
	
	bool feetOnTheGround() {
		BoxPhysicsObject** current = &physics->boxPhysicsObjects[0];
		while (*current != nullptr) {
			if((*current)->collider->IntersectsWith(*player->collider)) {
				//log(LogLevel::Info, "On the ground %f %f", (*current)->collider->position.x(), (*current)->collider->position.y());
				return true;
			}
			++current;
		}
		return false;
	}
	
	void drawGUI(const char* text) {
		if (text != nullptr) {
			g2->drawString(text, 10, 10);
		}
		
		g2->setColor(Graphics1::Color::White);
	}
	
	// Return true if animation cycle is complete
	bool updateCharacter() {
		switch (playerStatus) {
			case WalkingRight: {
				//log(LogLevel::Info, "Walk right");
				drawSingleTile(g2, cameraPosition, playerPosition, Walk0 + animIndex);
				break;
			}
			case WalkingLeft: {
				//log(LogLevel::Info, "Walk left");
				drawSingleTile(g2, cameraPosition, playerPosition, Walk0 + animIndex, true);
				break;
			}
			case JumpingRight: {
				//log(LogLevel::Info, "Jump right");
				drawSingleTile(g2, cameraPosition, playerPosition, Jump2 /*+ animIndex*/);
				break;
			}
			case JumpingLeft: {
				//log(LogLevel::Info, "Jump left");
				drawSingleTile(g2, cameraPosition, playerPosition, Jump2 /*+ animIndex*/, true);
				break;
			}
			case Standing: {
				//log(LogLevel::Info, "Standing");
				drawSingleTile(g2, cameraPosition, playerPosition, Stand);
				break;
			}
			default:
				break;
		}
		
		++animCount;
		
		if (animCount > 20) {
			animCount = 0;
			++animIndex;
			if (animIndex >= 8) {
				animIndex = 0;
				return true;
			}
		}
		return false;
	}
	
	void update() {
		double t = System::time() - startTime;
		double deltaT = t - lastTime;
		//Kore::log(Info, "%f", deltaT);
		lastTime = t;
		
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag);
		
		playerPosition = player->position;
		
		const float impulse = 10;
		
		// Move player only if the feet are on the ground
		playerStatus = Standing;
		//if (feetOnTheGround()) {
			if (left && playerPosition.x() > 0) {
				playerStatus = WalkingLeft;
				player->ApplyImpulse(vec3(-impulse, 0, 0));
			} else if (right && playerPosition.x() < mapColumns * tileWidth - tileWidth) {
				playerStatus = WalkingRight;
				player->ApplyImpulse(vec3(impulse, 0, 0));
			}
			
			if (jump) {
				const float jumpImpulse = 100;
				if (left) {
					playerStatus = JumpingLeft;
				} else if (right) {
					playerStatus = JumpingRight;
				}
				player->ApplyImpulse(vec3(0, -jumpImpulse, 0));
				
				jump = false;
			}
		//}
		
		// Collect a coin if the player is near enough
		collectCoins();
		
		// Update the physics and render
		physics->Update(deltaT);
		
		g2->begin();
		
		// Draw background
		drawTiles(g2, cameraPosition);
		
		// Draw coins
		for (int i = 0; i < coinNum; i++) {
			vec3 pos = coins[i]->position;
			drawSingleTile(g2, cameraPosition, pos, TileID::Dollar);
		}
		
		// Draw the player
		updateCharacter();
		
		if (debug) {
			// Debug: show bounding box
			physics->DrawBoundingBox(g2);
			physics->DrawBoundingSphere(g2);
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
		
		player = new BoxPhysicsObject();
		player->position = playerPosition;
		player->collider->position = playerPosition;
		player->velocity = vec3(0, 0, 0);
		player->Mass = 10;
	}
	
	void spawnCoins() {
		Kore::vec3 coinPositions[mapRows * mapColumns];
		coinNum = 0;
		getTiles(TileID::Dollar, coinPositions, coinNum);
		
		coins = new SpherePhysicsObject*[coinNum];
		for (int i = 0; i < coinNum; i++) {
			coins[i] = new SpherePhysicsObject();
			coins[i]->position = coinPositions[i];
			coins[i]->collider->center = vec3(coins[i]->position.x()+tileWidth/2, coins[i]->position.y()+tileHeight/2, 0);
			physics->AddObject(coins[i]);
		}
	}
	
	void initBoxColliders() {
		int size = 0;
		Kore::vec3 boxColliders[mapRows * mapColumns];
		getTiles(TileID::Ground0, boxColliders, size);
		getTiles(TileID::Ground1, boxColliders, size);
		getTiles(TileID::Ground2, boxColliders, size);
		getTiles(TileID::Ground3, boxColliders, size);
		getTiles(TileID::Ground4, boxColliders, size);
		
		for (int i = 0; i < size; ++i) {
			BoxPhysicsObject* boxCollider = new BoxPhysicsObject();
			boxCollider->position = boxColliders[i];
			boxCollider->collider->position = boxColliders[i];
			physics->AddObject(boxCollider);
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

int kickstart(int argc, char** argv) {
	Kore::System::init("Mini2DEngine", width * scale, height * scale);
	Kore::System::setCallback(update);
	
	startTime = System::time();
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	
	g2 = new Graphics2::Graphics2(width, height);
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
	physics = new PhysicsWorld(player);
	spawnCoins();
	initBoxColliders();
	
	Kore::System::start();
	
	return 0;
}
