#include <windows.h>
#include "InputHandler.h"

void InputHandler::getInputs(){
	if (GetAsyncKeyState('d') & 0X8000) {
		dKeyPressed();
	}
	else if (GetAsyncKeyState('s') & 0X8000) {
		sKeyPressed();
	}
	else if (GetAsyncKeyState('a') & 0X8000){
		aKeyPressed();
	}
	else if(GetAsyncKeyState(VK_SPACE) & 0X8000) {
		spaceKeyPressed();
	}
}

void InputHandler::dKeyPressed(){
}

void InputHandler::sKeyPressed(){
}

void InputHandler::aKeyPressed(){
}

void InputHandler::spaceKeyPressed(){
}
