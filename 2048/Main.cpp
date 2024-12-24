# include <Siv3D.hpp> // Siv3D v0.6.15

constexpr int GRID_SIZE = 4; // グリッドのサイズ（4x4）
constexpr int CELL_SIZE = 120; // セルの大きさ
constexpr int SNOW_SPEED_MIN = 1, SNOW_SPEED_MAX=2; // 雪の速度

// 盤面の状態
Array<Array<int>> board(GRID_SIZE, Array<int>(GRID_SIZE, 0));

// ランダムな位置にタイルを追加
void addRandomTile()
{
	Array<Point> emptyCells;
	for (int y = 0; y < GRID_SIZE; ++y)
	{
		for (int x = 0; x < GRID_SIZE; ++x)
		{
			if (board[y][x] == 0)
			{
				emptyCells.emplace_back(x, y);
			}
		}
	}

	if (emptyCells.empty()) return; // 空きがない場合は何もしない

	const auto& [x, y] = emptyCells.choice();
	board[y][x] = (RandomBool(0.9) ? 2 : 4); // 90% で 2, 10% で 4
}

// ゲーム初期化
void initGame()
{
	board.assign(GRID_SIZE, Array<int>(GRID_SIZE, 0));
	for (int i = 0; i < 2; ++i) // 初期にランダムな2つのタイルを追加
	{
		addRandomTile();
	}
}

// 盤面を左にスライドする
bool slideLeft()
{
	bool moved = false;

	for (int y = 0; y < GRID_SIZE; ++y)
	{
		Array<int> row;
		for (int x = 0; x < GRID_SIZE; ++x)
		{
			if (board[y][x] > 0)
			{
				row.push_back(board[y][x]);
			}
		}

		for (size_t i = 0; i + 1 < row.size(); ++i)
		{
			if (row[i] == row[i + 1])
			{
				row[i] *= 2;
				row.erase(row.begin() + i + 1);
			}
		}

		while (row.size() < GRID_SIZE)
		{
			row.push_back(0);
		}

		for (int x = 0; x < GRID_SIZE; ++x)
		{
			if (board[y][x] != row[x])
			{
				moved = true;
				board[y][x] = row[x];
			}
		}
	}
	return moved;
}

// 盤面を右にスライドする
bool slideRight()
{
	bool moved = false;

	for (int y = 0; y < GRID_SIZE; ++y)
	{
		Array<int> row;
		for (int x = GRID_SIZE - 1; x >= 0; --x)
		{
			if (board[y][x] > 0)
			{
				row.push_back(board[y][x]);
			}
		}

		for (size_t i = 0; i + 1 < row.size(); ++i)
		{
			if (row[i] == row[i + 1])
			{
				row[i] *= 2;
				row.erase(row.begin() + i + 1);
			}
		}

		while (row.size() < GRID_SIZE)
		{
			row.push_back(0);
		}

		for (int x = 0; x < GRID_SIZE; ++x)
		{
			if (board[y][GRID_SIZE - 1 - x] != row[x])
			{
				moved = true;
				board[y][GRID_SIZE - 1 - x] = row[x];
			}
		}
	}
	return moved;
}

// 盤面を上にスライドする
bool slideUp()
{
	bool moved = false;

	for (int x = 0; x < GRID_SIZE; ++x)
	{
		Array<int> column;
		for (int y = 0; y < GRID_SIZE; ++y)
		{
			if (board[y][x] > 0)
			{
				column.push_back(board[y][x]);
			}
		}

		for (size_t i = 0; i + 1 < column.size(); ++i)
		{
			if (column[i] == column[i + 1])
			{
				column[i] *= 2;
				column.erase(column.begin() + i + 1);
			}
		}

		while (column.size() < GRID_SIZE)
		{
			column.push_back(0);
		}

		for (int y = 0; y < GRID_SIZE; ++y)
		{
			if (board[y][x] != column[y])
			{
				moved = true;
				board[y][x] = column[y];
			}
		}
	}
	return moved;
}

// 盤面を下にスライドする
bool slideDown()
{
	bool moved = false;

	for (int x = 0; x < GRID_SIZE; ++x)
	{
		Array<int> column;
		for (int y = GRID_SIZE - 1; y >= 0; --y)
		{
			if (board[y][x] > 0)
			{
				column.push_back(board[y][x]);
			}
		}

		for (size_t i = 0; i + 1 < column.size(); ++i)
		{
			if (column[i] == column[i + 1])
			{
				column[i] *= 2;
				column.erase(column.begin() + i + 1);
			}
		}

		while (column.size() < GRID_SIZE)
		{
			column.push_back(0);
		}

		for (int y = 0; y < GRID_SIZE; ++y)
		{
			if (board[GRID_SIZE - 1 - y][x] != column[y])
			{
				moved = true;
				board[GRID_SIZE - 1 - y][x] = column[y];
			}
		}
	}
	return moved;
}

// メイン関数
void Main()
{
	Scene::SetBackground(ColorF(0.95));
	FontAsset::Register(U"GameFont", 40, Typeface::Bold);

	FontAsset::Register(U"ButtonFont", 30, Typeface::Regular);

	initGame();

	const Rect newGameButton(500, 20, 100, 50); // 「新しいゲームを始める」ボタンの座標とサイズ

	Array<Vec2> snowflakes;
	Array<double> snowspeed;

	while (System::Update())
	{
		// 入力処理
		bool moved = false;
		if (KeyLeft.down()) moved = slideLeft();
		if (KeyRight.down()) moved = slideRight();
		if (KeyUp.down()) moved = slideUp();
		if (KeyDown.down()) moved = slideDown();

		// ボタンがクリックされたらゲームを初期化
		if (newGameButton.leftClicked())
		{
			initGame();
		}

		if (moved)
		{
			addRandomTile(); // 動きがあった場合のみ新しいタイルを追加
		}

		// 盤面描画
		for (int y = 0; y < GRID_SIZE; ++y)
		{
			for (int x = 0; x < GRID_SIZE; ++x)
			{
				Rect rect(x * CELL_SIZE + 20, y * CELL_SIZE + 100, CELL_SIZE - 5);
				rect.rounded(10).draw(Palette::Orange);
				if (board[y][x] > 0)
				{
					rect.stretched(-10).draw(ColorF(1.0, 0.8, 0.6));
					FontAsset(U"GameFont")(board[y][x]).drawAt(rect.center(), Palette::Black);
				}
			}
		}

		// 雪を追加
		if (snowflakes.size() < 100 && RandomBool(0.05))
		{
			snowflakes.push_back(Vec2(Random(Scene::Width()), 0));
			snowspeed.push_back(Random(SNOW_SPEED_MIN, SNOW_SPEED_MAX));
		}

		// 雪を描画
		for (int i=0;i< snowflakes.size();++i)
		{
			snowflakes.at(i).y += snowspeed.at(i);
			Circle(snowflakes.at(i), 5).draw(Palette::White);
		}

		// 画面外の雪を削除
		Erase_if(snowflakes, [](const Vec2& snowflake) { return snowflake.y > Scene::Height(); });

		// ボタンの描画
		newGameButton.draw(ColorF(0.6, 0.8, 0.95)).drawFrame(2, 0, Palette::Black);
		FontAsset(U"ButtonFont")(U"new").drawAt(newGameButton.center(), Palette::Black);
	}
}
