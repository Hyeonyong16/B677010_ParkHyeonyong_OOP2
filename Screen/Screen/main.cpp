// Screen.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include "Utils.h"

using namespace std;

class Screen {
	int width;
	int height;
	char* canvas;

	static Screen* instance;
	Screen(int width = 70, int height = 25) 
		: width(width), height(height),
		canvas(new char[(width+1)*height])

	{
		Borland::initialize();
	}
public:
	static Screen& getInstance() {
		if (instance == nullptr) {
			instance = new Screen();
		}
		return *instance;
	}

	~Screen() {
		if (instance) {
			delete[] canvas;
			instance = nullptr;
		}
	}

	void draw(const char* shape, int w, int h, const Position& pos)
	{
		if (!shape) return;
		for (int i = 0; i < h; i++)
		{
			strncpy(&canvas[pos.x + (pos.y+i)*(/*width+1*/w)], &shape[i*w], w);
		}
	}

	void render()
	{
		Borland::gotoxy(0, 0);
		cout << canvas;
	}

	void clear()
	{
		memset(canvas, ' ', (width + 1)*height);
		canvas[width + (height-1)*(width+1)] = '\0';
	}

};

Screen* Screen::instance = nullptr;

class GameObject {
	char* shape;
	int width;
	int height;
	Position pos;
	Screen& screen;

public:
	GameObject(const char* shape, int width, int height) 
		: height(height), width(width), shape(nullptr), pos(0, 0),
		screen(Screen::getInstance())
	{
		if (!shape || strlen(shape) == 0 || width == 0 || height == 0)
		{
			this->shape = new char[1];
			this->shape[0] = 'x';
			width = 1;
			height = 1;
		} else {
			this->shape = new char[(width+1)*height];
			strncpy(this->shape, shape, (width+1)*height);
		}
		this->width = width;
		this->height = height;
	}
	virtual ~GameObject() {
		if (shape) { delete[] shape; }
		width = 0, height = 0;
	}

	void setMine() {
		srand((unsigned)time(NULL));

		for (int i = 0; i < (width + 1) * height; i++) {
				shape[i] = '0';
				if (rand() % 10 == 0) 
					shape[i] = '*';
		}

		for (int i = 0; i < height; i++) {
			shape[(width + 1) * i + width] = '\n';
		}

		shape[(width + 1) * (height - 1) + width] = '\0';

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int pos = (width + 1) * i + j;
				if (shape[pos] != '*') {
					int mineNum = 0;	//주변 지뢰숫자를 저장할 변수
					int x, y, xMax, yMax;
					//각 모서리일때 예외처리용 변수(상하좌우)
					if (i == 0) y = 0;	
					else y = -1;
					if (j == 0) x = 0;
					else x = -1;
					if (i == height - 1) yMax = 1;
					else yMax = 2;
					if (j == width - 1) xMax = 1;
					else xMax = 2;
					//주변 8칸 확인해서 지뢰가 있을경우 mineNum 1씩 증가
					//								  -1          0         1
					//width * y + x + y		====-1 -width-2   -width-1   -width
					//주변 8칸 정보			==== 0	  -1	      0		    1
					//						==== 1   width     width+1   width+2
					for (int h = y; h < yMax; h++) {
						for (int w = x; w < xMax; w++) {
							if (shape[pos + (width * h) + w + h] == '*') mineNum++;
						}
					}

					shape[pos] = '0' + mineNum;
				}
			}
		}
	}

	void setBoard() {
		for (int i = 0; i < (width + 1) * height; i++) {
			shape[i] = '+';
		}

		for (int i = 0; i < height; i++) {
			shape[(width + 1) * i + width] = '\n';
		}

		shape[(width + 1) * (height - 1) + width] = '\0';
	}

	char* getShape() {
		return shape;
	}

	int checkGameover(const char* mine) {
		bool checkAll = true;
		for (int i = 0; i < (width + 1) * height; i++) {
			if (shape[i] == '*')
				//지뢰가 있으면
				return 1;
			if (shape[i] == '+' && mine[i] != '*')
				checkAll = false;
		}

		//다 찾았을시
		if (checkAll == true)
			return 2;

		//지뢰가 없으면
		return 0;
	}

	void draw() {
		screen.draw(shape, width+1, height, pos);
	}

	virtual void update(Position& pos, char* mine, bool leftClick) {
		if (leftClick == true) {
			//width * pos.y + pos.x 배열에서 위치
			if (mine[(width + 1) * pos.y + pos.x] == '*') {
				shape[(width + 1) * pos.y + pos.x] = mine[(width + 1) * pos.y + pos.x];
			}
			else if (mine[(width + 1) * pos.y + pos.x] > '0' && mine[(width + 1) * pos.y + pos.x] < '9') {
				shape[(width + 1) * pos.y + pos.x] = mine[(width + 1) * pos.y + pos.x];
			}
			else if (mine[(width + 1) * pos.y + pos.x] == '0') {
				shape[(width + 1) * pos.y + pos.x] = mine[(width + 1) * pos.y + pos.x];
				//width*(pos.y-1)+(pos.x-1)   width*(pos.y-1)+pox.x   width*(pos.y-1)+(pox.x+1)  
				//width * pos.y + (pos.x-1)   width * pos.y + pos.x   width * pos.y + (pos.x+1)
				//width*(pos.y+1)+(pos.x-1)   width*(pos.y-1)+pos.x   width*(pos.y+1)+(pox.x+1)
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						if (pos.y + i >= 0 && pos.y + i < height && pos.x + j >= 0 && pos.x + j < width) {
							Position nextPos(pos.x + j, pos.y + i);
							if (shape[(width + 1) * nextPos.y + nextPos.x] == '+')
								update(nextPos, mine, true);
						}
					}
				}
			}
		}

		//깃발
		if (leftClick == false) {
			shape[(width + 1) * pos.y + pos.x] = '=';
		}
	}
};

int main()
{
	char mine[110];
	char board[110];
	int gameover = 0;
	GameObject playerBoard(board, 10, 10);
	GameObject mineBoard(mine, 10, 10);
	Screen&	 screen = Screen::getInstance();
	INPUT_RECORD InputRecord;
	DWORD Events;

	playerBoard.setBoard();
	mineBoard.setMine();

	while (true)
	{
		playerBoard.draw();
		screen.render();	

		printf("\n LeftClick: check, RightClick: flag");
		gameover = playerBoard.checkGameover(mineBoard.getShape());
		if (gameover == 1)
		{
			printf("\n===GameOver!===\n");
			break;
		}

		else if (gameover == 2)
		{
			printf("\n===Clear!===\n");
			break;
		}

		Sleep(30);
		
		ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &InputRecord, 1, &Events);
		if (InputRecord.EventType == MOUSE_EVENT) {
			if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
				Position pos;
				pos.x = InputRecord.Event.MouseEvent.dwMousePosition.X;
				pos.y = InputRecord.Event.MouseEvent.dwMousePosition.Y;

				playerBoard.update(pos, mineBoard.getShape(), true);
			}
			if (InputRecord.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
				Position pos;
				pos.x = InputRecord.Event.MouseEvent.dwMousePosition.X;
				pos.y = InputRecord.Event.MouseEvent.dwMousePosition.Y;

				playerBoard.update(pos, mineBoard.getShape(), false);
			}
		}
		screen.clear();
	}

	return 0;
}