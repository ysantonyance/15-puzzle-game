#include <iostream>
#include <windows.h> // COORD HANDLE SetConsoleTextAttribute SetConsoleCursorPosition
#include <conio.h> // _getch
#include <algorithm>
#include <fstream>
using namespace std;

enum Color : short {
	BLACK, DARKBLUE, DARKGREEN, TURQUOISE, DARKRED,
	PURPLE, DARKYELLOW, GREY, DARKGREY, BLUE, GREEN,
	CYAN, RED, PINK, YELLOW, WHITE
};

enum Key : short {
	LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80,
	ENTER = 13, SPACE = 32, ESCAPE = 27, BACKSPACE = 8
};

class Tile
{
	int value; //значение

public:

	void SetValue(int value) //устанавливает значение
	{
		this->value = value;
	}

	int GetValue() //получает значение
	{
		return value;
	}

	bool IsEmpty() // проверка на пустоту клетки
	{
		if (value == 0) return true;
		else return false;
	}
};

template <size_t S>
class Board
{
	Tile board[S][S];

public:

	int CountInversions(int ar[S*S]) //счетик инверсий ">"
	{
		int inversions = 0;
		for (int i = 0; i < S*S; i++)
		{
			if (ar[i] == 0)
				continue;
			for (int j = i + 1; j < S*S; j++)
			{
				if (ar[j] != 0 && ar[i] > ar[j])
					inversions++;
			}
		}

		return inversions;
	}

	bool IsSolvable(int ar[S*S]) //проверка на возможность решения
	{
		int inversions = CountInversions(ar);

		int EmptyRowBottom;
		for (int i = 0; i < S*S; i++)
		{
			if (ar[i] == 0)
			{
				EmptyRowBottom = S - (i / S); //узнаем на какой строчке находится 0
				break;
			}
		}

		if (S % 2 == 1)
			return (inversions % 2 == 0);
		else
		{
			return ((EmptyRowBottom % 2 == 0 && inversions % 2 == 1) ||
				(EmptyRowBottom % 2 == 1 && inversions % 2 == 0));
		}
	}

	void Shuffle() //разброс
	{
		int temp[S*S];

		do
		{
			int index = 0;
			for (int i = 0; i < S; i++)
			{
				for (int j = 0; j < S; j++)
				{
					temp[index++] = board[i][j].GetValue();
				}
			}

			for (int i = S*S-1; i > 0; i--)
			{
				int j = rand() % (i + 1);
				swap(temp[i], temp[j]);
			}
		} while (!IsSolvable(temp));

		

		int index = 0;
		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < S; j++)
			{
				board[i][j].SetValue(temp[index++]);
			}
		}
	}

	void Move(Key command) //движение
	{
		int emptyX, emptyY;
		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < S; j++)
			{
				if (board[i][j].IsEmpty() == true)
				{
					emptyX = i;
					emptyY = j; //находим 0, и сразу же сохраняем координаты
				}
			}
		}

		switch (command)
		{
		case Key::UP:
			if (emptyX > 0) 
				swap(board[emptyX][emptyY], board[emptyX - 1][emptyY]);
			break;
		case Key::DOWN:
			if (emptyX < S - 1) 
				swap(board[emptyX][emptyY], board[emptyX + 1][emptyY]);
			break;
		case Key::LEFT:
			if (emptyY > 0) 
				swap(board[emptyX][emptyY], board[emptyX][emptyY - 1]);
			break;
		case Key::RIGHT:
			if (emptyY < S - 1) 
				swap(board[emptyX][emptyY], board[emptyX][emptyY + 1]);
			break;
		}
	}

	bool IsWin() // проверка на победу
	{
		int expected = 1;
		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < S; j++)
			{
				if (i == S - 1 && j == S - 1)
				{
					if (board[i][j].GetValue() != 0)
						return false;
				}
				else
				{
					if (board[i][j].GetValue() != expected++)
						return false;
				}
			}
		}

		COORD text_bar;
		text_bar.X = 0;
		text_bar.Y = S * S;
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(h, Color::GREEN);
		SetConsoleCursorPosition(h, text_bar);
		cout << "You WON!\n";
		SetConsoleTextAttribute(h, Color::WHITE);
		return true;
	}

	Tile& GetTile(int index1, int index2) //возвращении определенной клетки
	{
		return board[index1][index2];
	}

	Board() //конструкутор заполняет борд числами от 0 до 15
	{
		int count = 1;
		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < S; j++)
			{
				if (count != S* S)
					board[i][j].SetValue(count++);
				else
					board[i][j].SetValue(0);
			}
		}
	}
};

template <size_t S>
class Renderer abstract 
{
public:
	virtual void Render(Board<S>&) = 0;
};

template<size_t S>
class ConsoleRenderer : public Renderer <S>
{
public:
	void Render(Board<S>& board) override // вывод борда
	{
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

		cout << "Use arrows to move. Esc to exit.\n";
		for (int i = 0; i < S; i++)
			cout << "+----";
		cout << "+\n";

		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < S; j++)
			{
				int value = board.GetTile(i, j).GetValue();
				if (value == 0)
					cout << "|    ";
				else if (value < 10)
					cout << "|  " << value << " ";
				else
					cout << "| " << value << " ";
			}
			cout << "|\n";

			for (int x = 0; x < S; x++)
				cout << "+----";
			cout << "+\n";
		}
	}
};


class InputHandler abstract
{
public:
	
	virtual Key GetCommand() = 0;
};

class KeyboardInputHandler : public InputHandler
{
public:

	Key GetCommand() override
	{
		int key = _getch();
		if (key == 224)
			key = _getch();
		return Key(key);
	}
};


template<size_t S>
class FileManager
{
public:
	static bool SaveGame(Board<S>& board, const string& filename) //загрузка файла
	{
		ofstream out(filename);
		if (!out)
		{
			cout << "Error!\n";
			return false;
		}

		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < S; j++)
			{
				out << board.GetTile(i, j).GetValue() << " ";
			}
			out << "\n";
		}

		return true;
	}

	static bool LoadGame(Board<S>& board, const string& filename) //вывод файла
	{
		ifstream in(filename);
		if (!in)
		{
			cout << "Error!\n";
			return false;
		}

		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < S; j++)
			{
				int value;
				if (!(in >> value))
					return false;
				board.GetTile(i, j).SetValue(value);
			}
		}

		return true;
	}
};

template<size_t S>
class Game
{
	Board<S> board;
	Renderer<S>* renderer;
	InputHandler* input;
	int turnCounter = 0;
	bool IsOver = false;

public:

	Game(Renderer<S>* rend, InputHandler* input) : renderer(rend), input(input) 
	{
		board.Shuffle(); //после создания карты при запуске сразу же разброс
	}

	void Run() //игра
	{
		while (!IsOver)
		{
			system("cls");
			renderer->Render(board);

			Key command = input->GetCommand();

			switch (command)
			{
			case Key::UP:
				board.Move(command);
				turnCounter++;
				break;
			case Key::DOWN:
				board.Move(command);
				turnCounter++;
				break;
			case Key::LEFT:
				board.Move(command);
				turnCounter++;
				break;
			case Key::RIGHT:
				board.Move(command);
				turnCounter++;
				break;
			case Key::ESCAPE:
				IsOver = true;
				break;
			case Key::ENTER:
				FileManager<S>::SaveGame(board, "save.txt");
				break;
			case Key::BACKSPACE:
				FileManager<S>::LoadGame(board, "save.txt");
				break;
			}

			if (board.IsWin())
				IsOver = true;
		}
		
	}

};

int main()
{
	srand(time(0));
	ConsoleRenderer<4> renderer4;
	//ConsoleRenderer<3> renderer3;
	//ConsoleRenderer<5> renderer5;
	KeyboardInputHandler keyboardInput;
	Game<4> StandardGame(&renderer4, &keyboardInput);
	//Game<3> SmallGame(&renderer3, &keyboardInput);
	//Game<5> BigGame(&renderer5, &keyboardInput);
	StandardGame.Run();
}