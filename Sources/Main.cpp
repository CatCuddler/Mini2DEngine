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

using namespace Kore;

namespace {
	const int w = tileWidth * mapColumns;
	const int h = tileHeight * mapRows;
	const int scale = 2;
	
	Status playerStatus = Status::Standing;
	bool left, right, jump;
	int jumpAnimaCount = 0;
	
	Graphics2::Graphics2* g2;
	
	const int moveDistance = 2;
	int playerPosX = 0;
	int playerPosY = 0;

	void update() {
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag);
		
		// Move character
		if (left && playerPosX > 0 && !jump) {
			playerStatus = WalkingLeft;
			playerPosX -= moveDistance;
		} else if (right && playerPosX < mapColumns * tileWidth - tileWidth && !jump) {
			playerStatus = WalkingRight;
			playerPosX += moveDistance;
		} else if (jump) {
			/*playerStatus = JumpingLeft;
			
			if (jumpAnimaCount > 10 * 3) {
				playerPosY -= moveDistance;
				playerPosX += moveDistance;
			}
			
			if (jumpAnimaCount > 10 * 8) {
				jumpAnimaCount = 0;
				jump = false;
			}
			++jumpAnimaCount;*/
		} else {
			playerStatus = Standing;
		}

		g2->begin();
		drawTiles(g2, 0, 0);
		animate(playerStatus, g2, 0, 0, playerPosX, playerPosY);
		g2->end();

		Graphics4::end();
		Graphics4::swapBuffers();
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
			case KeySpace:
				jump = true;
				break;
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
	Kore::System::init("Mini2DEngine", w * scale, h * scale);
	Kore::System::setCallback(update);
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	
	g2 = new Graphics2::Graphics2(w, h, false);
	//g2->setImageScaleQuality(Graphics2::Low);
	
	initTiles("tileset/map2.csv", "tileset/tileset2.png");
	getTilePosition(TileID::Stand, playerPosX, playerPosY);

	Kore::System::start();

	return 0;
}
