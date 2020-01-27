#include <Windows.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <unordered_map>

using namespace std;

class Point {
	int _x, _y;

public:
	Point() {
		_x = 0;
		_y = 0;
	}

	Point(int x, int y) {
		_x = x;
		_y = y;
	}

	int getX() { return _x; }
	int getY() { return _y; }

};

// Отсюда вы можете брать цвета и здесь же создавать свои
class ConsoleColor {
public:
	static COLORREF red() { return RGB(255, 0, 0); }
	static COLORREF blue() { return RGB(0, 0, 255); }
	static COLORREF gray() { return RGB(200, 200, 200); }
	static COLORREF white() { return RGB(255, 255, 255); }
	static COLORREF green() { return RGB(0, 255, 0); }
	static COLORREF black() { return RGB(0, 0, 0); }
	static COLORREF brown() { return RGB(80, 20, 25); }
	static COLORREF yellow() { return RGB(255, 255, 0); }
};

// Это класс, который нужен для рисования. В нем все готово. Ничего менять не нужно. Нужно только пользоваться
class ConsoleDrawer {
	HANDLE _conHandle;
	HWND _hwnd;
	HDC _hdc;
	PAINTSTRUCT _ps;
	unordered_map<COLORREF, HGDIOBJ> _bm;
	unordered_map<COLORREF, HGDIOBJ> _pm;
	HGDIOBJ _curentBrush;
	HGDIOBJ _curentPen;

	int _width;
	int _height;
	bool _isWork = true;

	void selectBrush(COLORREF color, bool filled = false) {
		if (!filled) {
			SelectObject(_hdc, CreateSolidBrush(NULL_BRUSH));
			return;
		}
		if (_bm.find(color) == _bm.end())
			_bm.insert({ color, CreateSolidBrush(color) });

		if (_curentBrush != _bm[color]) {
			_curentBrush = _bm[color];
			SelectObject(_hdc, _curentBrush);
		}
	}

	void selectPen(COLORREF color) {
		if (_pm.find(color) == _pm.end())
			_pm.insert({ color, CreatePen(PS_SOLID, 1, color) });
		if (_curentPen != _pm[color]) {
			_curentPen = _pm[color];
			SelectObject(_hdc, _curentPen);
		}
	}

public:
	ConsoleDrawer() {
		setlocale(LC_ALL, "");
		_conHandle = GetStdHandle(STD_INPUT_HANDLE);
		int t = sizeof(HGDIOBJ);
		_hwnd = GetConsoleWindow();
		RECT rc;
		GetClientRect(_hwnd, &rc);
		_width = rc.right;
		_height = rc.bottom;
		_hdc = GetDC(_hwnd);
	}

	void clearScreen() {
		system("cls");
	}

	int getWidth() { return _width; }
	int getHeight() { return _height; }

	HANDLE get_conHandle() {
		return _conHandle;
	}

	bool isWork() {

		return _isWork;
	}

	void stop() {
		_isWork = false;
	}

	void drawBackground(COLORREF color = ConsoleColor::gray()) {
		RECT rc;
		GetClientRect(_hwnd, &rc);
		drawRect(Point(), Point(_width, _height), color, true);
	}

	void drawPoint(Point p, COLORREF color) {
		SetPixel(_hdc, p.getX(), p.getY(), color);
	}

	void drawLine(Point p1, Point p2, COLORREF color) {
		selectPen(color);
		//SelectObject(_hdc, CreatePen(PS_SOLID, 1, color));
		//SetDCPenColor(_hdc, color);
		MoveToEx(_hdc, p1.getX(), p1.getY(), NULL);
		LineTo(_hdc, p2.getX(), p2.getY());
	}

	// Рисует круг или окружность по центру и радиусу
	// filled - нужно ли закрашивать внутренность
	void drawCircle(Point c, int radius, COLORREF color, bool filled = false) {
		selectPen(color);
		selectBrush(color, filled);
		//SetDCBrushColor(_hdc, color);
		Ellipse(_hdc, c.getX() - radius, c.getY() - radius, c.getX() + radius, c.getY() + radius);

	}

	// Рисует прямоугольник по двум точкам
	// p1 - нижний левый угол
	// p2 - верхний правый угол
	void drawRect(Point p1, Point p2, COLORREF color, bool filled = false) {
		selectPen(color);
		selectBrush(color, filled);
		Rectangle(_hdc, p1.getX(), p1.getY(), p2.getX(), p2.getY());
	}

	// Рисует треугольник по трем точкам
	void drawTriangle(Point p1, Point p2, Point p3, COLORREF color, bool filled = false) {
		selectPen(color);
		selectBrush(color, filled);
		POINT apt[3];
		apt[0].x = p1.getX(); apt[0].y = p1.getY();
		apt[1].x = p2.getX(); apt[1].y = p2.getY();
		apt[2].x = p3.getX(); apt[2].y = p3.getY();
		Polygon(_hdc, apt, 3);
	}
};

// Это самый базовый класс, от которого наследуются все объекты, которые появляются на поле
class ParkObject {
protected:
	int _cellX, _cellY, _cellSize;

public:
	ParkObject(int cellX, int cellY, int cellSize) {
		_cellX = cellX;
		_cellY = cellY;
		_cellSize = cellSize;
	}

	bool isInside(int cellX, int cellY) {
		return (_cellX == cellX) && (_cellY == cellY);
	}

	int getCellX() {
		return _cellX;
	}

	int getCellY() {
		return _cellY;
	}

	int getX() {
		return _cellX * _cellSize;
	}

	int getY() {
		return _cellY * _cellSize;
	}

};

class SnowMan : public ParkObject {

public:
	SnowMan(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) { }

	void draw(ConsoleDrawer& cd) {
		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();

		double d = _cellSize / 100.0;
		cd.drawCircle(Point(x + _cellSize / 2, y + _cellSize - 20 * d), 20 * d, ConsoleColor::white(), true);
		cd.drawCircle(Point(x + _cellSize / 2, y + _cellSize - 55 * d), 15 * d, ConsoleColor::white(), true);
		cd.drawCircle(Point(x + _cellSize / 2, y + _cellSize - 80 * d), 10 * d, ConsoleColor::white(), true);
		cd.drawRect(Point(x + _cellSize / 2 - 5 * d, y + 10 * d), Point(x + _cellSize / 2 + 5 * d, y + d), ConsoleColor::brown(), true);
	}
};

class Stone : public ParkObject {

public:
	Stone(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) { }

	void draw(ConsoleDrawer& cd) {
		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();

		double d = _cellSize / 100.0;
		cd.drawCircle(Point(x + _cellSize / 2, y + _cellSize - 55 * d), 15 * d, ConsoleColor::black(), true);
	}
};

class Present : public ParkObject {

public:
	Present(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) {
	}

	void draw(ConsoleDrawer& cd) {
		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();

		double d = _cellSize / 100.0;
		cd.drawRect(Point(x + 20 * d, y + _cellSize), Point(x + 80 * d, y + 40 * d), ConsoleColor::red(), true);
		cd.drawRect(Point(x + 40 * d, y + _cellSize), Point(x + 60 * d, y + 40 * d), ConsoleColor::yellow(), true);
		cd.drawCircle(Point(x + 35 * d, y + 20 * d), 10 * d, ConsoleColor::yellow(), true);
		cd.drawCircle(Point(x + 65 * d, y + 20 * d), 10 * d, ConsoleColor::yellow(), true);
		cd.drawTriangle(Point(x + 50 * d, y + 40 * d), Point(x + 35 * d, y + 30 * d), Point(x + 45 * d, y + 20 * d), ConsoleColor::yellow(), true);
		cd.drawTriangle(Point(x + 50 * d, y + 40 * d), Point(x + 65 * d, y + 30 * d), Point(x + 55 * d, y + 20 * d), ConsoleColor::yellow(), true);
	}
};

class RedTree : public ParkObject {

public:
	RedTree(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) {}

	void draw(ConsoleDrawer& cd) {
		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();

		int d = _cellSize / 10;
		cd.drawRect(Point(x + 4 * d, y + _cellSize), Point(x + 6 * d, y + _cellSize - 2 * d), ConsoleColor::brown(), true);
		cd.drawTriangle(Point(x + 2 * d, y + _cellSize - 2 * d), Point(x + _cellSize / 2, y + d), Point(x + _cellSize - 2 * d, y + _cellSize - 2 * d), ConsoleColor::red(), true);
	}
};

class Tree : public ParkObject {

public:
	Tree(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) {}

	void draw(ConsoleDrawer& cd) {
		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();

		int d = _cellSize / 10;
		cd.drawRect(Point(x + 4 * d, y + _cellSize), Point(x + 6 * d, y + _cellSize - 2 * d), ConsoleColor::brown(), true);
		cd.drawTriangle(Point(x + 2 * d, y + _cellSize - 2 * d), Point(x + _cellSize / 2, y + d), Point(x + _cellSize - 2 * d, y + _cellSize - 2 * d), ConsoleColor::green(), true);
	}
};

class Dino : public ParkObject {
public:
	Dino(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) {}

	void step(int direction) {
		if (direction == 0)
			_cellX -= 1;
		if (direction == 1)
			_cellY -= 1;
		if (direction == 2)
			_cellX += 1;
		if (direction == 3)
			_cellY += 1;
	}

	void draw_c(ConsoleDrawer& cd, COLORREF color) {

		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();

		double d = _cellSize / 125.0;
		cd.drawRect(Point(x + d, y + 63 * d), Point(x + 24 * d, y + 53 * d), color, true);
		cd.drawRect(Point(x + 75 * d, y + 63 * d), Point(x + 98 * d, y + 53 * d), color, true);
		cd.drawRect(Point(x + 27 * d, y + 93 * d), Point(x + 72 * d, y + 35 * d), color, true);//body
		cd.drawRect(Point(x + 27 * d, y + 124 * d), Point(x + 42 * d, y + 94 * d), color, true);
		cd.drawRect(Point(x + 57 * d, y + 124 * d), Point(x + 72 * d, y + 94 * d), color, true);
		cd.drawRect(Point(x + 27 * d, y + 32 * d), Point(x + 88 * d, y + 2 * d), color, true);//head
		cd.drawCircle(Point(x + 42 * d, y + 12 * d), 8 * d, ConsoleColor::gray(), true);
		cd.drawTriangle(Point(x + 27 * d, y), Point(x + 49 * d, y - 45 * d), Point(x + 72 * d, y), ConsoleColor::red(), true);
		cd.drawCircle(Point(x + 49 * d, y - 50 * d), 10 * d, ConsoleColor::white(), true);

	}
};

class HappyDino : public Dino {

public:
	HappyDino(int cellX, int cellY, int cellSize) : Dino(cellX, cellY, cellSize) {}
	void draw(ConsoleDrawer& cd) {
		draw_c(cd, ConsoleColor::green());

	}
};

class AngryDino : public Dino {

public:
	AngryDino(int cellX, int cellY, int cellSize) : Dino(cellX, cellY, cellSize) {}

	void draw(ConsoleDrawer& cd) {
		draw_c(cd, ConsoleColor::red());

	}
};

class DinoPark {
	int _cellsXCount;
	int _cellsYCount;
	int _cellSize;
	int _score = 0;
	int _score_counter = 0;
	vector<SnowMan> _snowmen;
	vector<Stone> _stones;
	vector<Tree> _trees;
	vector<RedTree> _redtrees;
	vector<Present> _presents;
	vector<HappyDino> _hDino;
	vector<AngryDino> _aDino;

public:
	DinoPark(int width, int height, int cellSize) {
		_cellsXCount = width;
		_cellsYCount = height;
		_cellSize = cellSize;
	}

	// Этот метод проверяет, что находится в клетке:
	// -1 - снеговик
	//  0 - пусто
	//  1 - дерево
	//  2 - динозавр
	//  3 - подарок
	int find(int x, int y) {
		for (int i = 0; i < _snowmen.size(); i++) {
			if (_snowmen[i].isInside(x, y))
				return -1;
		}
		for (int i = 0; i < _stones.size(); i++) {
			if (_stones[i].isInside(x, y))
				return -1;
		}
		for (int i = 0; i < _trees.size(); i++) {
			if (_trees[i].isInside(x, y))
				return 1;
		}
		for (int i = 0; i < _redtrees.size(); i++) {
			if (_redtrees[i].isInside(x, y))
				return 1;
		}
		for (int i = 0; i < _hDino.size(); i++) {
			if (_hDino[i].isInside(x, y))
				return 2;
		}
		for (int i = 0; i < _aDino.size(); i++) {
			if (_aDino[i].isInside(x, y))
				return 2;
		}
		for (int i = 0; i < _presents.size(); ++i) {
			if (_presents[i].isInside(x, y))
				return 3;
		}
		return 0;
	}

	// Метод для отрисовки всего поля
	void draw(ConsoleDrawer& cd) {

		// Рисуем сетку
		for (int i = 0; i <= _cellsXCount; i++) {
			int x = i * _cellSize;
			int y = _cellsYCount * _cellSize;
			cd.drawLine(Point(x, 0), Point(x, y), ConsoleColor::black());
		}
		for (int i = 0; i <= _cellsYCount; i++) {
			int x = _cellsXCount * _cellSize;
			int y = i * _cellSize;
			cd.drawLine(Point(0, y), Point(x, y), ConsoleColor::black());
		}

		// Рисуем снеговиков
		for (int i = 0; i < _snowmen.size(); i++) {
			_snowmen[i].draw(cd);
		}
		
		// Рисуем камни
		for (int i = 0; i < _stones.size(); i++) {
			_stones[i].draw(cd);
		}

		// Рисуем деревья
		for (int i = 0; i < _trees.size(); i++) {
			_trees[i].draw(cd);
		}
		
		// Рисуем красные деревья
		for (int i = 0; i < _redtrees.size(); i++) {
			_redtrees[i].draw(cd);
		}

		// Рисуем подарки
		for (int i = 0; i < _presents.size(); ++i) {
			_presents[i].draw(cd);
		}

		// Рисуем динозавра
		for (int i = 0; i < _hDino.size(); i++) {
			_hDino[i].draw(cd);
		}

		// И злого
		for (int i = 0; i < _aDino.size(); i++) {
			_aDino[i].draw(cd);
		}
	}

	// Метод обработки вашего хода
	void step(ConsoleDrawer& cd) {

		// Пока ходит только счастливый динозавр

		// Ловим нажатие на клавиатуру
		KEY_EVENT_RECORD key;
		INPUT_RECORD irec[100];
		DWORD cc;

		ReadConsoleInput(cd.get_conHandle(), irec, 100, &cc);
		for (DWORD j = 0; j < cc; ++j) {
			if (irec[j].EventType == KEY_EVENT && irec[j].Event.KeyEvent.bKeyDown) {

				for (int i = 0; i < _hDino.size(); i++) {
					// Смотрим по сторонам
					// -1 - снеговик
					//  0 - пусто
					//  1 - дерево
					//  2 - динозавр
					//  3 - подарок
					vector<int> res = {
						lookLeft(_hDino[i].getCellX(), _hDino[i].getCellY()),
						lookUp(_hDino[i].getCellX(), _hDino[i].getCellY()),
						lookRight(_hDino[i].getCellX(), _hDino[i].getCellY()),
						lookDown(_hDino[i].getCellX(), _hDino[i].getCellY())
					};
					vector<int> resa = {
						lookLeft(_aDino[i].getCellX(), _aDino[i].getCellY()),
						lookUp(_aDino[i].getCellX(), _aDino[i].getCellY()),
						lookRight(_aDino[i].getCellX(), _aDino[i].getCellY()),
						lookDown(_aDino[i].getCellX(), _aDino[i].getCellY())
					};
					// Проверяем, какая именно кнопка была нажата
					int x = rand() % 4;
					switch (irec[j].Event.KeyEvent.wVirtualKeyCode) {
					case VK_LEFT:
						// если путь свободен, идем
						if (res[0] != -1) {
							_hDino[i].step(0);
							if (resa[x] != -1)
								_aDino[i].step(x);
							_score -= 30;
							_score_counter++;
							if (_score_counter == 3) {
								_score -= 50;
								_score_counter = 0;
							}
							// если мы на что-то наступили (пока что только дерево), обновляем картину
							if (res[0] != 0) {
								_score += 100;
								refresh(cd);
							}
							if (resa[x] != 0) {
								refresh(cd);
							}
						}
						break;
					case VK_UP:
						if (res[1] != -1) {
							_hDino[i].step(1);
							if (resa[x] != -1)
								_aDino[i].step(x);
							_score -= 30;
							_score_counter++;
							if (_score_counter == 3) {
								_score -= 50;
								_score_counter = 0;
							}
							if (res[1] != 0) {
								_score += 100;
								refresh(cd);
							}
							if (resa[x] != 0) {
								refresh(cd);
							}
						}
						break;
					case VK_RIGHT:
						if (res[2] != -1) {
							_hDino[i].step(2);
							if (resa[x] != -1)
								_aDino[i].step(x);
							_score -= 30;
							_score_counter++;
							if (_score_counter == 3) {
								_score -= 50;
								_score_counter = 0;
							}
							if (res[2] != 0) {
								_score += 100;
								refresh(cd);
							}
							if (resa[x] != 0) {
								refresh(cd);
							}
						}
						break;
					case VK_DOWN:
						if (res[3] != -1) {
							_hDino[i].step(3);
							if (resa[x] != -1)
								_aDino[i].step(x);
							_score -= 30;
							_score_counter++;
							if (_score_counter == 3) {
								_score -= 50;
								_score_counter = 0;
							}
							if (res[3] != 0) {
								_score += 100;
								refresh(cd);
							}
							if (resa[x] != 0) {
								refresh(cd);
							}
						}
						break;
					case VK_ESCAPE:
						system("cls");
						cout << "You died.";
						Sleep(650);
						system("cls");
						cout << "You died..";
						Sleep(650);
						system("cls");
						cout << "You died...";
						Sleep(1500);
						system("cls");
						cout << _score;
						Sleep(3000);
						cd.stop();
						break;
					}

				}

			}
		}
	}

	void addSnowMan(int x, int y) {
		_snowmen.push_back(SnowMan(x, y, _cellSize));
	}
	
	void addStones(int x, int y) {
		_stones.push_back(Stone(x, y, _cellSize));
	}

	void addTree(int x, int y) {
		_trees.push_back(Tree(x, y, _cellSize));
	}

	void addRedTree(int x, int y) {
		_redtrees.push_back(RedTree(x, y, _cellSize));
	}

	void addPresent(int x, int y) {
		_presents.push_back(Present(x, y, _cellSize));
	}

	void addHappyDino(int x, int y) {
		_hDino.push_back(HappyDino(x, y, _cellSize));
	}

	void addAngryDino(int x, int y) {
		_aDino.push_back(AngryDino(x, y, _cellSize));
	}

	// Взгляд на клетку вверх
	int lookUp(int cellX, int cellY) {
		if (cellY == 0)
			return -1;
		return find(cellX, cellY - 1);
	}

	// Взгляд на клетку вниз
	int lookDown(int cellX, int cellY) {
		if (cellY == _cellsYCount - 1)
			return -1;
		return find(cellX, cellY + 1);
	}

	// Взгляд на клетку вправо
	int lookRight(int cellX, int cellY) {
		if (cellX == _cellsXCount - 1)
			return -1;
		return find(cellX + 1, cellY);
	}

	// Взгляд на клетку влево
	int lookLeft(int cellX, int cellY) {
		if (cellX == 0)
			return -1;
		return find(cellX - 1, cellY);
	}

	// Обновляем картину
	void refresh(ConsoleDrawer& cd) {
		for (int i = 0; i < _hDino.size(); i++) {
			// Смотрим, где стоит динозавр
			int x = _hDino[i].getCellX();
			int y = _hDino[i].getCellY();
			// Находим то дерево, на котором он стоит
			for (int j = 0; j < _trees.size(); j++) {
				if (_trees[j].isInside(x, y)) {
					// Переносим дерево в другое место
					// Цикл нужен, чтобы не попасть в место, которое уже занято
					for (int k = 0; k < 100; k++) {
						int _x = rand() % _cellsXCount;
						int _y = rand() % _cellsYCount;
						if (find(_x, _y) == 0) {
							_trees[j] = Tree(_x, _y, _cellSize);
							break;
						}
					}
				}
			} 
			for (int j = 0; j < _redtrees.size(); j++) {
				if (_redtrees[j].isInside(x, y)) {
					// Переносим дерево в другое место
					// Цикл нужен, чтобы не попасть в место, которое уже занято
					for (int k = 0; k < 100; k++) {
						int _x = rand() % _cellsXCount;
						int _y = rand() % _cellsYCount;
						if (find(_x, _y) == 0) {
							_redtrees[j] = RedTree(_x, _y, _cellSize);
							break;
						}
					}
				}
			}
			for (int j = 0; j < _presents.size(); ++j) {
				if (_presents[j].isInside(x, y)) {
					_score += 200;
					_presents.erase(_presents.begin() + j);
				}
			}
		}
		for (int i = 0; i < _aDino.size(); i++) {
			// Смотрим, где стоит динозавр
			int x = _aDino[i].getCellX();
			int y = _aDino[i].getCellY();
			// Находим то дерево, на котором он стоит
			for (int j = 0; j < _trees.size(); j++) {
				if (_trees[j].isInside(x, y)) {
					// Переносим дерево в другое место
					// Цикл нужен, чтобы не попасть в место, которое уже занято
					for (int k = 0; k < 100; k++) {
						int _x = rand() % _cellsXCount;
						int _y = rand() % _cellsYCount;
						if (find(_x, _y) == 0) {
							_trees[j] = Tree(_x, _y, _cellSize);
							break;
						}
					}
				}
			}
			for (int j = 0; j < _redtrees.size(); j++) {
				if (_redtrees[j].isInside(x, y)) {
					// Переносим дерево в другое место
					// Цикл нужен, чтобы не попасть в место, которое уже занято
					for (int k = 0; k < 100; k++) {
						int _x = rand() % _cellsXCount;
						int _y = rand() % _cellsYCount;
						if (find(_x, _y) == 0) {
							_redtrees[j] = RedTree(_x, _y, _cellSize);
							break;
						}
					}
				}
			}
			for (int j = 0; j < _presents.size(); ++j) {
				if (_presents[j].isInside(x, y)) {
					_presents.erase(_presents.begin() + j);
				}
			}
		}
	}

};

int main() {
	ConsoleDrawer cd;
	srand(time(0));

	const int CellSize = 60;
	int width = cd.getWidth() / CellSize;
	int height = cd.getHeight() / CellSize;
	DinoPark dinoPark(width, height, CellSize);

	dinoPark.addSnowMan(1, 2);
	dinoPark.addSnowMan(1, 3);
	dinoPark.addSnowMan(1, 4);
	dinoPark.addSnowMan(3, 3);
	dinoPark.addSnowMan(3, 5);
	dinoPark.addSnowMan(5, 3);
	dinoPark.addSnowMan(5, 5);


	dinoPark.addStones(7, 6);

	dinoPark.addPresent(0, 2);
	dinoPark.addPresent(8, 4);
	dinoPark.addPresent(9, 2);

	dinoPark.addRedTree(0, 0);
	dinoPark.addRedTree(7, 1);

	dinoPark.addTree(1, 1);
	dinoPark.addTree(2, 1);
	dinoPark.addTree(3, 1);
	dinoPark.addTree(4, 1);
	dinoPark.addTree(4, 3);
	dinoPark.addTree(4, 5);
	dinoPark.addTree(3, 4);
	dinoPark.addTree(5, 4);

	dinoPark.addHappyDino(4, 4);
	dinoPark.addAngryDino(7, 3);

	cd.clearScreen();

	while (cd.isWork()) {
		cd.drawBackground();
		dinoPark.draw(cd);
		dinoPark.step(cd);
	}
	return 0;
}
