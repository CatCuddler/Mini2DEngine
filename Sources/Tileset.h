#pragma once

#include <Kore/Graphics2/Graphics.h>

enum Status {
	Standing, WalkingLeft, WalkingRight, JumpingLeft, JumpingRight
};

enum TileID {
	Stand = 3, Dollar = 4,
	Walk0 = 10, Walk1 = 11, Walk2 = 12, Walk3 = 13, Walk4 = 14, Walk5 = 15, Walk6 = 16, Walk7 = 17,
	Jump0 = 20, Jump1 = 21, Jump2 = 22, Jump3 = 23, Jump4 = 24, Jump5 = 25, Jump6 = 26, Jump7 = 27
};

namespace {
	int* source;
}

void loadCsv(const char* csvFile);

void initTiles(const char* csvFile, const char* tileFile);
void drawTiles(Kore::Graphics2::Graphics2* g2, Kore::vec3 cameraPosition);
void animate(Status playerStatus, Kore::Graphics2::Graphics2* g2, Kore::vec3 cameraPosition, Kore::vec3 characterPosition);

int getTileID(float px, float py);
int getTileIndex(float px, float py);
void getTilePosition(TileID tileID, int& posX, int& posY);


