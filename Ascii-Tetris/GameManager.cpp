#include "GameManager.h"
#include "ConsoleUtils.h"

int GameManager::score = 0;

bool GameManager::startScreen() {
	Renderer::renderStartScreen();

	return true;
}