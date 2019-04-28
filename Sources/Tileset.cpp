#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Log.h>
#include <Kore/Graphics1/Image.h>
#include <Kore/TextureImpl.h>

#include "Tileset.h"
#include "Settings.h"

namespace{
	Graphics4::Texture* image;
	
	int animIndex = 0;
	int animCount = 0;
	
	int tilesetRows = -1;
	int tilesetColumns = -1;
	
	int lastPlayerPosX;
	int lastPlayerPosY;
}

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

void initTiles(const char* csvFilePath, const char* tileFilePath) {
	image = new Graphics4::Texture(tileFilePath);
	
	tilesetColumns = image->texWidth / tileWidth;
	tilesetRows = image->texHeight / tileHeight;
	
	loadCsv(csvFilePath);
}

void drawTiles(Graphics2::Graphics2* g2, int camX, int camY) {
	for (int y = 0; y < mapRows; ++y) {
		for (int x = 0; x < mapColumns; ++x) {
			int index = source[y * mapColumns + x];
			
			int row = (int)(index / mapColumns);
			int column = index % mapColumns;
			
			g2->drawScaledSubImage(image, column * tileWidth, row * tileHeight, tileWidth, tileHeight, x * tileWidth - camX, y * tileHeight - camY, tileWidth, tileHeight);
		}
	}
}

void animate(Status playerStatus, Graphics2::Graphics2* g2, int camX, int camY, int posX, int posY) {
	int column = animIndex;
	
	switch (playerStatus) {
		case WalkingRight: {
			//log(LogLevel::Info, "Walk right");
			int row = (int)(Walk0 / tilesetRows);
			g2->drawScaledSubImage(image, column * tileWidth, row * tileHeight, tileWidth, tileHeight, posX - camX, posY - camY, tileWidth, tileHeight);
			break;
		}
		case WalkingLeft: {
			//log(LogLevel::Info, "Walk left");
			int row = (int)(Walk0 / tilesetRows);
			g2->drawScaledSubImage(image, (column + 1)  * tileWidth, row * tileHeight, -tileWidth, tileHeight, posX - camX, posY - camY, tileWidth, tileHeight);
			break;
		}
		case JumpingRight: {
			//log(LogLevel::Info, "Jump right");
			int row = (int)(Jump0 / tilesetRows);
			g2->drawScaledSubImage(image, column * tileWidth, row * tileHeight, tileWidth, tileHeight, posX - camX, posY - camY, tileWidth, tileHeight);
			break;
		}
		case JumpingLeft: {
			//log(LogLevel::Info, "Jump left");
			int row = (int)(Jump0 / tilesetRows);
			g2->drawScaledSubImage(image, (column + 1) * tileWidth, row * tileHeight, -tileWidth, tileHeight, posX - camX, posY - camY, tileWidth, tileHeight);
			break;
		}
		case Standing: {
			//log(LogLevel::Info, "Standing");
			int row = (int)(Stand / tilesetRows);
			g2->drawScaledSubImage(image, Stand * tileWidth, row * tileHeight, tileWidth, tileHeight, posX - camX, posY - camY, tileWidth, tileHeight);
			break;
		}
		default:
			break;
	}
	
	++animCount;
	
	if (animCount > 20) {
		animCount = 0;
		++animIndex;
		if (animIndex >= 8)
			animIndex = 0;
	}
		
	lastPlayerPosX = posX;
	lastPlayerPosY = posY;
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
