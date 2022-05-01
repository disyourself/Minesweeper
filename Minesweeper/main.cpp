#include<stdio.h>
#include<easyx.h>
#include<time.h>
#include<string.h>
#include<graphics.h>

#define Row 10
#define Col 10

int map[Row][Col];
int boomNUM = 9;
bool isDead = false;


IMAGE img[15];
IMAGE icon;

void show() 
{
	for (int i=0;i<Row;i++)
	{
		for (int j = 0; j < Col; j++) {
			printf(" %2d ", map[i][j]);
		}
		printf("\n");
	}
}

void init(int num) {

	isDead = false;

	//设置随机数种子
	srand((unsigned)time(NULL));
	for (int i = 0; i < Row; i++) {
		for (int j = 0; j < Col; j++)
		{
			map[i][j] = 0;
		}
	}
	//埋雷
	for (int i = 0; i < num; ) {
		int r = rand() % Row;
		int c = rand() % Col;
		if (0 == map[r][c]) {
			map[r][c] = -1;
			i++;
		}
	}

	//处理雷周围的数据
	for (int i = 0; i < Row; i++) {
		for (int k = 0; k < Col; k++) {
			if (map[i][k] == -1) {
				for (int m = i - 1; m < i + 2; m++) {
					for (int n = k - 1; n < k + 2; n++) {
						if (m >= 0 && m < Row &&
							n >= 0 && n < Col &&
							-1 != map[m][n]
							)
							map[m][n]++;
					}
				}
			}
		}
	}

	//加载图片
	for (int i = 0; i < 15; i++)
	{
		char filename[20]="";
		sprintf_s(filename, "./res/%d.jpg", i);
		loadimage(&img[i], filename, 40, 40);
	}

	//数据加密（遮罩数据)
	for (int i = 0; i < Row; i++) {
		for (int j = 0; j < Col; j++)
		{
			map[i][j] += 20;
		}
	}

}

void gameDraw() {
	for (int i = 0; i < Row; i++) {
		for (int j = 0; j < Col; j++) {
			int x = i * 40;
			int y = j * 40;

			
			if (map[i][j] >= 0 && map[i][j] <= 8)
				putimage(x, y, img + map[i][j]);

			else if (-1 == map[i][j]) {
				putimage(x, y, img + 9);
			}

			//爆炸
			else if (-21 == map[i][j]) {
				putimage(x, y, img + 10);
			}

			//遮罩数据
			else if (map[i][j] >= 19 && map[i][j] <= 28) {				
				putimage(x, y, img + 13);
			}

			//右键标记
			else if (map[i][j] >= 39 && map[i][j] <= 48) {
				putimage(x, y, img + 11);
			}
		}
	}
}


void openNull(int row, int col)
{
	if (0 == map[row][col])
	{
		for (int i = row - 1; i <= row + 1; i++) {
			for (int j = col - 1; j <= col + 1; j++)	
			{
				if ((i >= 0 && i < Row && j >= 0 && j < Col)
					&& map[i][j] >= 20)
				{
					map[i][j] -= 20;
					openNull(i, j);
				}
			}
		}
	}
}



void isWin() {
	if (isDead) {
		//踩雷了 失败
		printf("you lose!\n");
		system("pause");

		init(boomNUM);
	}
	else {
		int clearedBlocks=0;

		for (int i = 0; i < Row; i++) {
			for (int j = 0; j < Col; j++)
			{
				if (map[i][j]!=-1&&map[i][j]>=0&&map[i][j]<=8) {
					
					clearedBlocks++;
					printf("left: %d\n", Row * Col - clearedBlocks);
				}
			}
		}

		if (clearedBlocks == Row * Col - boomNUM-1) {
			printf("you win!!!!\n");
			system("pause");
			init(boomNUM);
		}
	}
}


bool boom(int row, int col) {
	if (-1 == map[row][col]) {
		map[row][col] -= 20;
		for (int i = 0; i < Row; i++) {
			for (int j = 0; j < Col; j++) 
			{
				if (map[i][j] >= 19 && map[i][j] <= 28) {
					map[i][j] -= 20;

					return true;
				}
			}
		}
	}
	return false;
}



void mouseEvent() 
{
	//获取鼠标消息
	MOUSEMSG msg = GetMouseMsg();

	int row = msg.x / 40;
	int col = msg.y / 40;

	if (msg.uMsg == WM_LBUTTONDOWN) {
		if (map[row][col] >= 19)
		{
			map[row][col] -= 20;

			//如果点到的是空白
			openNull(row, col);
			//炸弹
			isDead = boom(row, col);
			
			gameDraw();

			Sleep(200);

			isWin();
		}
	}
	//右键标记
	else if (msg.uMsg == WM_RBUTTONDOWN) {
		if (map[row][col] >= 19 && map[row][col] < 29)
		{
			map[row][col] += 20;
		}
	}
}


int main(int argc,char**argv) {
	initgraph(400,400,EW_SHOWCONSOLE);
	

	if (argc > 1) boomNUM = (int)argv[1];

	init(boomNUM);
	show(); 

	while (1)
	{
		//BeginBatchDraw();

		gameDraw();
		mouseEvent();

		//FlushBatchDraw();
	}

	getchar();

	return 0;
}