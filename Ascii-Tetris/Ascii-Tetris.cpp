#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <windows.h>
#include <conio.h>
#include <random>

using namespace std;
using namespace std::chrono;


class gameManager {
	public:
		bool running = true;
		static const int targetFPS = 60;
		static const int targetFrameTime = 1000 / targetFPS; // in milliseconds

		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		
		
		gameManager() {

			CONSOLE_CURSOR_INFO cursorInfo;
			cursorInfo.dwSize = 1;
			cursorInfo.bVisible = FALSE;
			SetConsoleCursorInfo(consoleHandle, &cursorInfo);
		}



		auto getElapsedTime(steady_clock::time_point startTime, steady_clock::time_point endTime) {
			return duration_cast<milliseconds>(endTime - startTime).count();
		}
};
	
class boardManager {

public:


	COORD piecePosition = { 6, 2 };
	COORD pieceStartingCoords = { 6, 2 };
	// Board dimensions (add 2 more for board walls)
	static const int boardWidth = 12;
	static const int boardHeight = 22;

	// Gameboard declaration (X and Y flipped)
	int gameboard[boardHeight][boardWidth]{};



	bool pieceCollided = false;

	//         z  y  z
	int pieces[7][5][5]{
		{
			//O piece (doesnt rotate)
			{ 2, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 1, 1, 0, 0 },
			{ 0, 1, 1, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			//I piece 
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 1, 1, 1, 1, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			//S Piece
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 1, 1, 0 },
			{ 0, 1, 1, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			//Z Piece
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 1, 1, 0, 0 },
			{ 0, 0, 1, 1, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			//L Piece
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 1, 1, 1, 0 },
			{ 0, 1, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			//J Piece
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 1, 1, 1, 0 },
			{ 0, 0, 0, 1, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			//T Piece
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 1, 1, 1, 0 },
			{ 0, 0, 1, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		}
	};

	int currentPiece[5][5] = {
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
	};

	static enum pixelType {
		empty = 0,
		piece = 1,
		staticPiece = 2,
		wall = 3
	};

	boardManager() {
		// Set the roofs of the board to wall
		for (int x = 0; x < boardWidth; x++) {
			gameboard[0][x] = pixelType::wall;
			gameboard[boardHeight - 1][x] = pixelType::wall;
		}
		// Set the walls of the board to wall
		for (int y = 0; y < boardHeight; y++) {
			gameboard[y][0] = pixelType::wall;
			gameboard[y][boardWidth - 1] = pixelType::wall;
		}
	}

	//Piece Functions
	void createPiece() {
		piecePosition = pieceStartingCoords;
		randomizeCurrentPiece();
		buildPiece();
	}

	void moveDownPiece() {
		piecePosition.Y = piecePosition.Y + 1;
	}

	void moveRightPiece() {
		if (gameboard[piecePosition.Y][piecePosition.X + 1] == 0) {
			piecePosition.X = piecePosition.X + 1;
		}
	}

	void moveLeftPiece() {
		if (gameboard[piecePosition.Y][piecePosition.X - 1] == 0) {
			piecePosition.X = piecePosition.X - 1;
		}

	}

	void randomizeCurrentPiece() {
		srand(static_cast<unsigned int>(time(0)));
		int randomInt = rand() % 7;
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				currentPiece[y][x] = pieces[randomInt][y][x];
			}
		}
	}

	void buildPiece() {
		int offsetX = 0;
		int offsetY = 0;
		for (int x = 0; x < 5; ++x) {
			for (int y = 0; y < 5; ++y) {
				if (currentPiece[y][x] == 1) {
					offsetX = x - 3;
					offsetY = y - 3;
					if (pieceCollided == true) {
						gameboard[offsetY + piecePosition.Y][offsetX + piecePosition.X] = pixelType::staticPiece;
					}
					else {
						gameboard[offsetY + piecePosition.Y][offsetX + piecePosition.X] = pixelType::piece;
						
					}
				}
			};

		}
	};

	void boardClear() {
		for (int y = 0; y < boardHeight; y++) {
			for (int x = 0; x < boardWidth; x++) {
				if (gameboard[y][x] == pixelType::piece)
					gameboard[y][x] = pixelType::empty;
			}
		}
	}

	bool checkForDown() {
		bool allUnderEmpty = 1;
		int offsetX = 0;
		int offsetY = 0;
		for (int x = 0; x < 5; ++x) {
			for (int y = 0; y < 5; ++y) {
				if(currentPiece[y][x] == 1)
				{
					offsetX = x - 3;
					offsetY = y - 3;
					auto checkTile = gameboard[offsetY + piecePosition.Y + 1][offsetX + piecePosition.X];
					if (checkTile == pixelType::wall || checkTile == pixelType::staticPiece) {
						allUnderEmpty = false;
						break;
					}
				}
				
			}
		}
		return allUnderEmpty;
	}
};


void update(int frameCount, boardManager& boardManager) {
	auto piececheck = boardManager.gameboard[boardManager.piecePosition.Y + 1][boardManager.piecePosition.X];
	cout << boardManager.checkForDown();
	if (frameCount % 10 == 0) {
		if(boardManager.checkForDown()){
			boardManager.moveDownPiece();
			boardManager.boardClear();
			boardManager.buildPiece();
		}
		else {
			boardManager.pieceCollided = true;
			boardManager.boardClear();
			boardManager.buildPiece();
			boardManager.pieceCollided = false;
			boardManager.createPiece();
		}
	}

}

void playerInput(boardManager& boardManager) {
	if (_kbhit()) {
		char key = _getch();
		switch (key) {
			case 'd':
				boardManager.moveRightPiece();
				break;
			case 'a':
				boardManager.moveLeftPiece();
				break;
			default:
				break;
		}
	}
}

void render(boardManager boardManager, gameManager gameManager, string boardString) {
	SetConsoleCursorPosition(gameManager.consoleHandle, { 0, 0 });
	boardString = "";
	for (int row = 0; row < boardManager.boardHeight; row++) {
		for (int collumn = 0; collumn < boardManager.boardWidth; collumn++) {
			switch (boardManager.gameboard[row][collumn]) {

			case boardManager::pixelType::wall:
				boardString = boardString + "#" + " ";
				break;
			case boardManager::pixelType::empty:
				boardString = boardString + " " + " ";
				break;
			case boardManager::pixelType::piece:
				boardString = boardString + "O" + " ";
				break;
			case boardManager::pixelType::staticPiece:
				boardString = boardString + "O" + " ";
				break;
			default:
				break;
			}
		}
		boardString = boardString + "\n";
	}
	cout << boardString;
}

int main() {
	int frameCounter = 0;
	string boardString = "";
	gameManager gameManager;
	boardManager boardManager;
	boardManager.createPiece();
	while (gameManager.running)
	{
		++frameCounter;

		//Starts Clock
		const auto startTime = steady_clock::now();

		//Game loop
		playerInput(boardManager);
		update(frameCounter, boardManager);
		render(boardManager, gameManager,boardString);

		//Ends Clock and handles FPS
		const auto endTime = steady_clock::now();
		const auto elapsedMilliseconds = gameManager.getElapsedTime(startTime, endTime);
		this_thread::sleep_for(milliseconds(gameManager.targetFrameTime-elapsedMilliseconds));
		
	}
}
