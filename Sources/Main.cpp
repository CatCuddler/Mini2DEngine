#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Graphics4/PipelineState.h>
#include <Kore/Graphics4/Shader.h>
#include <Kore/Graphics1/Color.h>
#include <Kore/System.h>

#include "Tileset.h"

using namespace Kore;

namespace {
	const int w = 320;
	const int h = 320;
	const int scale = 2;
	
	Graphics2::Graphics2* g2;

	void update() {
		Graphics4::begin();
		//Graphics4::restoreRenderTarget();
		Graphics4::clear(Graphics4::ClearColorFlag);

		g2->begin();
		drawTiles(g2, 0, 0);
		g2->end();

		Graphics4::end();
		Graphics4::swapBuffers();
	}
}

int kore(int argc, char** argv) {
	Kore::System::init("Mini2DEngine", w * scale, h * scale);
	Kore::System::setCallback(update);
	
	g2 = new Graphics2::Graphics2(w, h, false);
	g2->setImageScaleQuality(Graphics2::Low);
	
	initTiles("tileset/tileset.csv", "tileset/tiles.png");

	Kore::System::start();

	return 0;
}
