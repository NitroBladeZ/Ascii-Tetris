#include "ConsoleUtils.h"

HANDLE ConsoleUtils::consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_CURSOR_INFO ConsoleUtils::cursorInfo;

void ConsoleUtils::resetCursorPosition(COORD offset){
	SetConsoleCursorPosition(consoleHandle, { offset.X, offset.Y });
}

void ConsoleUtils::setCursorInvisible(){
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void ConsoleUtils::setCursorVisible(){
	cursorInfo.dwSize = 25;
	cursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void ConsoleUtils::clearConsole(){
	system("cls");
}
