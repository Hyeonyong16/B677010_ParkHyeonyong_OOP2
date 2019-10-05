// Screen.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <ctime>
#include "Utils.h"

using namespace std;

class GameObject {
	char*		shape;
	int			width;
	int			height;
	Position	pos;
	Screen&		screen;

	bool			 isMove;		//움직일 블럭인지 아닌지 체크

	vector<GameObject *> children;


public:
	GameObject(const char* shape, int width, int height, const Position& pos = Position{ 0, 0 })
		: height(height), width(width),
		shape(nullptr), pos(pos), isMove(true),
		screen(Screen::getInstance()) {
		if (!shape || strlen(shape) == 0 || width == 0 || height == 0)
		{
			this->shape = new char[1];
			this->shape[0] = 'x';
			width = 1;
			height = 1;
		}
		else {
			this->shape = new char[width*height];
			strncpy(this->shape, shape, width*height);
		}
		this->width = width;
		this->height = height;
	}

	virtual ~GameObject() {
		if (shape) { delete[] shape; }
		width = 0, height = 0;
	}

	void add(GameObject* child) {
		if (!child) return;
		children.push_back(child);
	}

	void setShape(const char* shape) {
		if (!shape) return;
		strncpy(this->shape, shape, width*height);
	}

	void setPos(int x, int y) { this->pos.x = x; this->pos.y = y; }

	Position& getPos() { return pos; }

	bool& getIsMove() { return isMove; };

	virtual void draw()
	{
		screen.draw(shape, width, height, pos);

		for (auto child : children) child->draw();
	}

	void internalUpdate() {
		update();
		for (auto child : children)
			child->update();
	}

	virtual void update()
	{
	}

	vector<GameObject *>& getChildren() { return children; }
};

class Block : public GameObject {

	vector<string> sprites;
	int current;

public:
	Block(vector<string>& sprites, int w, int h,
		const Position& pos = Position{ 0,0 })
		: sprites(sprites), current(0),
		GameObject(sprites[current].c_str(), w, h, pos) {}

	void update() {
		WORD keyCode;
		if (getPos().y >= Screen::getInstance().getHeight() - 5) {
			getIsMove() = false;
			return;
		}
		if (getPos().x > 24) {
			getIsMove() = false;
			return;
		}

		if (getIsMove() == true) {
			if (Input::GetKeyEvent(keyCode)) {
				switch (keyCode) {
				case VK_RIGHT:
					if (getPos().x >= 24) break;
					getPos().x++;
					break;

				case VK_LEFT:
					if (getPos().x <= 0) break;
					getPos().x--;
					break;

				case VK_UP:
					current = (current + 1) % sprites.size();
					setShape(sprites[current].c_str());
					break;

				case VK_DOWN:
					getPos().y = Screen::getInstance().getHeight() - 6;
					break;

				case 0x41: //'a'
					current = (current + 1) % sprites.size();
					setShape(sprites[current].c_str());
					break;

				case 0x44: //'d'
					current = (current + 3) % sprites.size();
					setShape(sprites[current].c_str());
					break;
				}
			}
			getPos().y = getPos().y + 1;
		}
	}
};

int main()
{
	//ㄴ자 블럭
	vector<string> sprites1{ "\x78\x78  \x78  \x78 ",
		"   \x78\x78\x78  \x78",
		" \x78  \x78 \x78\x78 ",
		"   \x78  \x78\x78\x78"
	};
	//ㅁ자 블럭
	vector<string> sprites2{ "   \x78\x78 \x78\x78 " };
	//역 ㄴ자 블럭
	vector<string> sprites3{ "     \x78\x78\x78\x78",
	"\x78\x78  \x78  \x78 ",
	"   \x78\x78\x78\x78  ",
	"\x78  \x78  \x78\x78 "
	};
	//l자 블럭
	vector<string> sprites4{ "\x78  \x78  \x78  ",
	"      \x78\x78\x78"
	};

	Screen&	 screen = Screen::getInstance();
	vector<GameObject *> gameObjects;

	srand(time(nullptr));

	string mode{ "mode con cols="
		+ to_string(screen.getWidth() + 4)
		+ " lines=" + to_string(screen.getHeight() + 5) };
	system(mode.c_str());
	system("chcp 437");

	auto parent = new Block{ sprites1, 3,3, Position{7, 0} };		//움직일 블럭
	auto child = new Block{ sprites2, 3,3, Position{30, 3} };		//다음으로 보여줄 블럭

	parent->add(child);
	gameObjects.push_back(parent);

	screen.clear(); screen.render();

	while (true)
	{
		screen.clear();
		for (auto obj : gameObjects) obj->internalUpdate();

		for (auto it = gameObjects.cbegin();
			it != gameObjects.cend(); it++)
			(*it)->draw();

		screen.render();
		Sleep(150);

		Input::EndOfFrame();
	}

	return 0;
}