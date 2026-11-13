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
		// True if game loop is running
		bool running = true;

		// Frames per second (target)
		static const int targetFPS = 60;
		static const int targetFrameTime = 1000 / targetFPS; // in milliseconds

		//Honestly no idea
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		
		int score;
		
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

	// Current position of middle pixel of piece
	COORD piecePosition = { 6, 2 };
	COORD pieceStartingCoords = { 6, 2 };

	// Board dimensions (add 2 more for board walls)
	static const int boardWidth = 12;
	static const int boardHeight = 22;

	// Gameboard declaration (X and Y flipped)
	int gameboard[boardHeight][boardWidth]{};

	bool pieceCollided = false;

	// 3D array containing all tetronimo types
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

	// Current piece type eg. I, T, O
	int currentPiece[5][5] = {
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
	};

	// Stores all pixel types of the board
	enum pixelType {
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

	// Piece Functions
	void createPiece() {
		piecePosition = pieceStartingCoords;
		randomizeCurrentPiece();
		buildPiece();
	}

	// Checks move is valid then moves piece
	void movePiece(int dirX, int dirY) {
		if (checkValidMove(dirX, dirY)) {
			piecePosition.X = piecePosition.X + dirX;
			piecePosition.Y = piecePosition.Y + dirY;
		}
	}

	// Generates a random piece and sets current piece array to the type of piece
	void randomizeCurrentPiece() {
		srand(static_cast<unsigned int>(time(0)));
		int randomInt = rand() % 7;
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				currentPiece[y][x] = pieces[randomInt][y][x];
			}
		}
	}

	// Clears non static piece pixels
	void buildPiece() {
		int offsetX = 0;
		int offsetY = 0;

		int lineCheck = 0;
		
		//Clears board
		for (int y = 0; y < boardHeight; y++) {
			lineCheck = 0;
			for (int x = 0; x < boardWidth; x++) {
				if (gameboard[y][x] == pixelType::piece){
					gameboard[y][x] = pixelType::empty;
				}
				lineCheck += gameboard[y][x];
			}
		}
		//Builds piece
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

	// Checks if current piece position + the offset is free (returns true if free)
	bool checkValidMove(int checkOffsetX, int checkOffsetY) {
		bool allCheckedEmpty = 1;
		int pieceOffsetX = 0;
		int pieceOffsetY = 0;
		for (int x = 0; x < 5; ++x) {
			for (int y = 0; y < 5; ++y) {
				if (currentPiece[y][x] == 1) {
					pieceOffsetX = x - 3;
					pieceOffsetY = y - 3;
					auto checkTile = gameboard[pieceOffsetY + piecePosition.Y + checkOffsetY][pieceOffsetX + piecePosition.X + checkOffsetX];
					if (checkTile == pixelType::wall || checkTile == pixelType::staticPiece) {
						allCheckedEmpty = false;
						break;
					}
				}
				
			}
		}
		return allCheckedEmpty;
	}
};


void update(int frameCount, boardManager& boardManager, gameManager& gameManager) {
	if (frameCount % 5 == 0) {
		if(boardManager.checkValidMove(0,1)){
			boardManager.movePiece(0,1);
			boardManager.buildPiece();
		}
		else {
			boardManager.pieceCollided = true;
			boardManager.buildPiece();
			boardManager.pieceCollided = false;
			boardManager.createPiece();
			gameManager.score++;
		}
	}
	else {
		boardManager.buildPiece();
	}
}

void playerInput(boardManager& boardManager) {
	if (_kbhit()) {
		char key = _getch();
		switch (key) {
			case 'd':
				boardManager.movePiece(1,0);
				break;
			case 'a':
				boardManager.movePiece(-1,0);
				break;
			default:
				break;
		}
	}
}

// Renders gameboard
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
	cout << "Score: " << gameManager.score;

}

int main() {
	int frameCounter = 0;
	string boardString = "";
	gameManager gameManager;
	boardManager boardManager;

	gameManager.score = 0;


	boardManager.createPiece();
	while (gameManager.running)
	{
		++frameCounter;

		//Starts Clock
		const auto startTime = steady_clock::now();

		//Game loop

		update(frameCounter, boardManager, gameManager);
		playerInput(boardManager);
		render(boardManager, gameManager,boardString);

		//Ends Clock and handles FPS
		const auto endTime = steady_clock::now();
		const auto elapsedMilliseconds = gameManager.getElapsedTime(startTime, endTime);
		this_thread::sleep_for(milliseconds(gameManager.targetFrameTime-elapsedMilliseconds));
		
	}
}
