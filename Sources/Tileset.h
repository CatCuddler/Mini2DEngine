#pragma once

#include <Kore/Graphics2/Graphics.h>

using namespace Kore;

namespace {
	int* source;
	
	const int tileWidth = 32;
	const int tileHeight = 32;
}

void loadCsv(const char* csvFile);

void initTiles(const char* csvFile, const char* tileFile);
void drawTiles(Graphics2::Graphics2* g2, int camX, int camY);
void animate(Graphics2::Graphics2* g2, int camX, int camY, int posX, int posY);

int getTileID(float px, float py);
int getTileIndex(float px, float py);

enum TileID {Walk0 = 20, Walk1 = 21, Walk2 = 22, Walk3 = 23, Walk4 = 24, Walk5 = 25, Walk6 = 26, Walk7 = 27};
