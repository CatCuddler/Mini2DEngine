#pragma once

#include <Kore/Graphics2/Graphics.h>

enum CharacterState {
	Standing, WalkingLeft, WalkingRight, JumpingLeft, JumpingRight
};

enum TileID {
	Ground0 = 0, Ground1 = 1, Ground2 = 2, Ground3 = 3, Ground4 = 4,
	Dollar = 5, Spikes = 6,
	Stand = 9,
	Walk0 = 10, Walk1 = 11, Walk2 = 12, Walk3 = 13, Walk4 = 14, Walk5 = 15, Walk6 = 16, Walk7 = 17,
	Jump0 = 20, Jump1 = 21, Jump2 = 22, Jump3 = 23, Jump4 = 24, Jump5 = 25, Jump6 = 26, Jump7 = 27
};

namespace {
	int* source;
}

void initTiles(const char* csvFile, const char* tileFile);
void getTiles(TileID ID, Kore::vec3* positions, int& tileCount);
void drawTiles(Kore::Graphics2::Graphics2* g2, Kore::vec3 cameraPosition);
void drawSingleTile(Kore::Graphics2::Graphics2* g2, Kore::vec3 cameraPosition, Kore::vec3 tilePosition, int tileID, bool reverse = false);

int getTileID(float px, float py);
int getTileIndex(float px, float py);
void getTilePosition(TileID tileID, int& posX, int& posY);
