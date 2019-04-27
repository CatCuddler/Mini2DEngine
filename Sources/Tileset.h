#pragma once

#include <Kore/Graphics2/Graphics.h>

using namespace Kore;

namespace {
	const char* tileFile;
	Graphics4::Texture* image;
	
	int* source;
	
	const int tileWidth = 32;
	const int tileHeight = 32;
	
	const int rows = 10;
	const int columns = 10;
}

void loadCsv(const char* csvFile);

void initTiles(const char* csvFile, const char* tileFile);
void drawTiles(Graphics2::Graphics2* g2, float camX, float camY);

int getTileID(float px, float py);
int getTileIndex(float px, float py);

enum TileID {Water = 1};
