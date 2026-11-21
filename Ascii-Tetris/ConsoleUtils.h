#include <windows.h>
#ifndef CONSOLEUTILS_H
#define CONSOLEUTILS_H
class ConsoleUtils {
private:
	static HANDLE consoleHandle;
	static CONSOLE_CURSOR_INFO cursorInfo;
public:
	static void resetCursorPosition(COORD offset);
	static void setCursorInvisible();
	static void setCursorVisible();
	static void clearConsole();

};
#endif