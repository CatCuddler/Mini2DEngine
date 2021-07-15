#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Log.h>
#include <Kore/Graphics1/Image.h>
//#include <Kore/TextureImpl.h>

#include "Tileset.h"
#include "Settings.h"

using namespace Kore;

namespace{
	Graphics4::Texture* image;
	
	int tilesetRows = -1;
	int tilesetColumns = -1;
	
	CharacterState lastPlayerStatus;
	
	void loadCsv(const char* csvFile) {
		FileReader file(csvFile);
		
		void* data = file.readAll();
		int length = file.size();
		
		char* cpyData = new char[length + 1];
		for (int i = 0; i < length; ++i) {
			cpyData[i] = ((char*)data)[i];
		}
		cpyData[length] = 0;
		
		source = new int[mapRows * mapColumns];
		int i = 0;
		
		char delimiter[] = ",;\n";
		char* ptr = std::strtok(cpyData, delimiter);
		while (ptr != nullptr) {
			assert(i < mapRows * mapColumns);
			int num = atoi(ptr);
			//log(Info, "%i -> %i", i, num);
			source[i] = num;
			ptr = std::strtok(nullptr, delimiter);
			i++;
		}
	}
}

void initTiles(const char* csvFilePath, const char* tileFilePath) {
	image = new Graphics4::Texture(tileFilePath);
	
	tilesetColumns = image->texWidth / tileWidth;
	tilesetRows = image->texHeight / tileHeight;
	
	loadCsv(csvFilePath);
}

void drawTiles(Graphics2::Graphics2* g2, vec3 cameraPosition) {
	for (int y = 0; y < mapRows; ++y) {
		for (int x = 0; x < mapColumns; ++x) {
			int index = source[y * mapColumns + x];
			if (index != -1 && index != TileID::Dollar) {
				drawSingleTile(g2, cameraPosition, vec3(x * tileWidth, y * tileHeight, 0), index);
			}
		}
	}
}

void getBoxColliders(Kore::vec3* positions, int& count) {
	int boxCounter = 0;
	for (int y = 0; y < mapRows; ++y) {
		for (int x = 0; x < mapColumns; ++x) {
			int index = source[y * mapColumns + x];
			if (index >= Ground0 && index <= Ground4) {
				positions[boxCounter] = vec3(x * tileWidth, y * tileHeight, 0);
				++boxCounter;
			}
		}
	}
	count = boxCounter;
}

void getTiles(TileID ID, Kore::vec3* positions, int& size) {
	int tileCounter = 0;
	for (int y = 0; y < mapRows; ++y) {
		for (int x = 0; x < mapColumns; ++x) {
			int index = source[y * mapColumns + x];
			if (index == ID) {
				positions[tileCounter] = vec3(x * tileWidth, y * tileHeight, 0);
				++tileCounter;
			}
		}
	}
	size = tileCounter;
}

void drawSingleTile(Kore::Graphics2::Graphics2* g2, Kore::vec3 cameraPosition, Kore::vec3 tilePosition, int tileID, bool reverse) {
	int row = (int)(tileID / tilesetRows);
	int column = tileID % tilesetColumns;
	
	if (!reverse)
		g2->drawScaledSubImage(image, column * tileWidth, row * tileHeight, tileWidth, tileHeight, tilePosition.x() - cameraPosition.x(), tilePosition.y() - cameraPosition.y(), tileWidth, tileHeight);
	else
		g2->drawScaledSubImage(image, (column + 1)  * tileWidth, row * tileHeight, -tileWidth, tileHeight, tilePosition.x() - cameraPosition.x(), tilePosition.y() - cameraPosition.y(), tileWidth, tileHeight);
}

int getTileID(float px, float py) {
	int index = getTileIndex(px, py);
	return source[index];
}

int getTileIndex(float px, float py) {
	int x = px / tileWidth;
	int y = py / tileHeight;
	return y * mapColumns + x;
}

void getTilePosition(TileID tileID, int& posX, int& posY) {
	for (int y = 0; y < mapRows; ++y) {
		for (int x = 0; x < mapColumns; ++x) {
			int index = source[y * mapColumns + x];
			if (index == tileID) {
				posX = x * tileWidth;
				posY = y * tileHeight;
			}
		}
	}
}
