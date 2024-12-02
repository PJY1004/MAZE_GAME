#include<iostream>
#include<Windows.h>
#include<conio.h>
#include<queue>
#include<random>
#include<time.h>
using namespace std;

bool start_flag, end_flag; // ���� ����, ���� ����
int W, H, trap; //�̷��� �ʺ�, ����
char map[50][50]; //��ü �̷� ��
int visited[20][20]; //�� Ž�� ���� ǥ��
int numSet[20][20]; //Eller �˰��򿡼� ����� �� ���� ���� ��ȣ
int score; //�̷� ���� ���� �� ���� �̵� Ƚ��
int min_path; //bfs�� Ž���� ��� ����

int dx[4] = { 0, 0, 1, -1 };  // �̵��� 4�� �������θ� ����
int dy[4] = { 1, -1, 0, 0 };

typedef struct node { // BFS Ž������ ����, �� node�� ���̴�.
	int x;		// ���� x��ǥ, y��ǥ
	int y;
	int num;	// bfs Ž������ ��, �ش� �濡 �̸������ �ּ� �̵� Ƚ��
};

//�ܼ� ������ Ư�� ��ġ�� Ŀ���� �̵���Ű�� �Լ� 
void gotoXY(int x, int y) {
	COORD pos;
	pos.X = x * 2;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

//�ܼ� â �����ϴ� �Լ�
void SetConsole() {

	system("mode con:cols=100 lines=45");
	system("title MazeGame");
}

//���� ����ȭ�� �׸��� �Լ�
void DrawGame() {

	system("cls");
	gotoXY(10, 6);  // gotoXY�� �ش� ��ǥ�� Ŀ�� �̵��ϴ� �Լ�.
	cout << "===========================================================";
	gotoXY(10, 8);
	cout << "===================      Maze Game      ===================";
	gotoXY(10, 10);
	cout << "===========================================================";
	gotoXY(20, 14);
	cout << " 's' key ���� ����";
	gotoXY(20, 16);
	cout << " 'q' key ���� ����";

}

//���� ���� �� ���� �����ִ� �Լ�
void ReGame() {
	system("cls");  // �ϴ� ���� ȭ�� �� ����.
	gotoXY(19, 6);
	cout << "   ���� �̵� Ƚ�� : " << score + 1;  // score ��� �� 
	gotoXY(17, 8);
	cout << "================================";
	gotoXY(17, 10);
	if ((end_flag == 1) && (score < 0))	// score, min_path(bfs Ž������ ã�� �ּ� �̵� ��) ��� ���迡 ���� LOSE , WIN ��� 
		cout << "==           LOSE(Q.Q)        ==";
	else
		cout << "==            WIN(^_^)        ==";
	gotoXY(17, 12);
	cout << "================================";

	gotoXY(10, 16);
	cout << "===========================================================";
	gotoXY(10, 18);
	cout << "====               �ٽ� �����ϰڽ��ϱ�?                 ===";
	gotoXY(10, 20);
	cout << "===========================================================";
	gotoXY(20, 24);
	cout << "'s' key: �� �� ��!";
	gotoXY(20, 26);
	cout << "'q' key: ����";
}

//���� ����/���� Ű �Է¹޴� �Լ�
bool SetGame() {
	int key;
	//DrawGame();
	while (true) {
		if (_kbhit()) {
			key = _getch();
		}
		else {
			key = 0;
		}

		if (key == 'S' || key == 's') {
			return true;
		}
		else if (key == 'Q' || key == 'q') {
			break;
		}
	}
	return false;
}

// �̷��� �ʺ�, ���� �Է��ϴ� �Լ�
void Input() {
	system("cls");
	gotoXY(15, 12);
	cout << "< 2 �̻�, 15������ ������ �Է��� �ּ���. >";
	gotoXY(17, 16);
	cout << "�̷��� ���� ���� �Է�: ";
	cin >> W;
	gotoXY(17, 18);
	cout << "�̷��� ���� ���� �Է�: ";
	cin >> H;
	gotoXY(17, 20);
	cout << "�̷� ������ ���� �Է�: ";
	cin >> trap;
}

// �̷ο� ���� ������ �ʱ�ȭ�ϴ� �Լ�
void InitMaze() {
	int i, j;

	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) {
			numSet[i][j] = i * W + j; // numSet�� ���ȣ �迭. �� ��ȣ ���� ����� �̷��� ���������� �й���.
			visited[i][j] = false;      // visited�� �� Ž�� ���� ��Ÿ�� �迭. �� Ž�� ���� ��� 0�� ǥ��
		}
	}
	// numSet�� ��¥ �÷��̾ ���ƴٴ� �� �ִ� ��(����) ��Ÿ���� �迭,
	// map�� numset�� �� �渶�� ���κ�('|'), ���κ�('-') ���� ����� �ѷ����̵��� ���ڴ�ó�� ����� �迭�� �� ������ ��Ÿ��.

	for (i = 0; i < H * 2 + 1; i++) {		// �ʺ�, ���� ��� 15���� �Է� ������ ������..
		for (j = 0; j < W * 2 + 1; j++) {
			if (i % 2 == 0) {	// ¦�� ��° �࿡�� �̷� ���� ������(���� ��)�� �׷��ش�.
				if (j % 2 == 0) {		// ���� �ʱ� ȭ�� �̷� �׸��� ��.. 
					map[i][j] = '+';   // ¦����, ¦���� ĭ���� '+'��,
				}
				else {
					map[i][j] = '-';   // ¦����, Ȧ������ ĭ���� ���κ�('-')�� �׷��ش�.
				}
			}
			else {		// Ȧ�� ��° �࿡�� �̷� ���� ������(���� ��)�� �׷��ش�.
				if (j % 2 == 0) {	  // Ȧ�� ��, ¦�� ������ �̷� ���� ������(���κ�)�� �׸���.
					map[i][j] = '|';  // for�� i,j ���� ���� '<' �̹Ƿ� �� �� �������� ���κ� �׸��� ���� ��(i+1)���� �Ѿ�Ե�
				}					  // �׷� 0 ~ W ���� ���κ��� (W + 1)���� �׷����� �ȴ�.
				else {
					map[i][j] = ' '; // Ȧ�� ��, Ȧ�� ���� �� ����(�÷��̾ ���ƴٴ� ����)���� �þ߿� �ȰŽ����� ��ĭ���� ����.
				}
			}
		}
	}			// �̷��� �ϸ� ����� ���κ�, ���κ����� �̷ΰ� �� �����ִ� ����.. MadeMaze_Eller �Լ�(�ٷ� ���� ����)���� �� �վ��� ����. 
}

// Eller �˰������� ���� �̷� �����(InitMaze���� ���� ��� �� �մ� �۾�)
void MadeMaze_Eller() {
	int i, j, k;

	for (i = 0; i < H; i++) { // ������ ����, ���� ���� ��� �� �྿ ����
		int remove_wall; //�� �������� �����ϴ� flag

		for (j = 0; j < W - 1; j++) { // ���� �� ���� ����

			// ������ ���� ��, ���� �ٸ� �� ��ȣ ���� ���� ��� ���� 
			if (i == H - 1) {
				remove_wall = 1;
			}
			else { // ������ ���� �ƴ϶��, ���� �� ���� random���� ����
				remove_wall = rand() % 2;
			}

			if ((remove_wall == 1) && (numSet[i][j + 1] != numSet[i][j])) { // ������ ���� �۾�. 
				// removewall flag�� 1�̰�, ������ �波�� ��ȣ�� �ٸ��ٸ�(=���� �����ִٸ�)
				numSet[i][j + 1] = numSet[i][j]; // ������ �波�� �� ��ȣ ������ ���ְ�
				int nx = j * 2 + 2;	 // ������(Ȧ����, ¦���� ĭ) ���� �۾���. 
				int ny = i * 2 + 1;	 // ���� ���� ���ʿ� Ȧ����, ¦���� ĭ���� �׷Ȱ�,  		
				map[ny][nx] = ' ';	// nx = j * 2 + 2; �̹Ƿ� numset[i][j]�� ������ ���� ���� ���ŵ�.
			}						// j-1 ĭ�� �񱳿��ٸ� nx = j * 2 �� �ص� �������, �׷��� for�� ���� ��ü�� �ٲ�ߵ�. 

		}

		if (i == H - 1) // ������ �࿡ �����ߴٸ� ���� �����ϸ� �ȵ�.. 
			break;		// while�� Ż��.



		int cnt = 1; // ���� �࿡�� ���� ���� �� ���� ���� 

		for (j = 0; j < W; j += cnt) { // ���� �� ���� ����
			cnt = 1;

			if (j < W - 1) { // �� ������ ���� �ƴ϶��
				for (k = j + 1; k < W; k++) {   // �ش� �� numSet[i][j]�� ������ ����� Ž���Ѵ�.
					if (numSet[i][j] != numSet[i][k]) // �� ��ȣ�� �ٸ� �� ���� ������ Ž���ϸ�
						break;
					cnt++;				// �ٸ� ���ȣ�� ������ �ʾҴٸ� �ش� ���� ���� ����(cnt)�� +1��
				}
			}
			// ���� ���� ���̰� 1�� ���(cnt++ �ȵ� = numSet[i][j+1]�� numSet[i][j]�� ���ȣ �ٸ� = �� �ٷ� �����ʿ� ���������� ���θ���)
			if (cnt == 1) {
				numSet[i + 1][j] = numSet[i][j]; // �Ʒ� ���κ� �����ϸ� ����Ǳ� ������ �ٷ� �Ʒ� �� ��ȣ�� ���� ������.
				map[i * 2 + 2][j * 2 + 1] = ' '; // ����. ���� ���� ����� �ȵǹǷ�.. �ش� ���� �ٷ� �Ʒ� ���κ�(¦����,Ȧ������ ��ġ) ������.
			}
			else {		// ���� ���̰� 2 �̻��� ���. �⺻������ ��� ���� �� ��ȣ�� ����� component�� �ϴ� �Ʒ��� ������ �� �ϳ����� ���ٰ���.

				// �ϴ� �� ���κ� �ϳ� �������ִ� �۾�
				int remove_idx = rand() % cnt;			// �ϴ� ���� ���ʿ� �ִ� ���� ���� 2�̻�¥�� ���� �Ʒ� ���κ� �����ؼ� �� ���ش�.
				remove_idx += j; // numSet[i][j]�� �� ��ġ�̹Ƿ�.. ������ ���κ� �ε���(remove_idx)�� ���α��� cnt�� ���� random���� ������.

				numSet[i + 1][remove_idx] = numSet[i][remove_idx]; // �Ʒ� ���κ�("-",¦����,Ȧ������ ��ġ) ���� �۾�.
				map[i * 2 + 2][remove_idx * 2 + 1] = ' ';

				// �ϳ� ���������� ������ �Ʒ� ���κ��� random���� ����
				for (int s = j; s < j + cnt; s++) { // ���� �� ��ȣ ĭ ���鼭
					if (s != remove_idx) {
						remove_wall = rand() % 2;   // random���� �Ʒ� ���κ� ���� 

						if (remove_wall == 1) {
							numSet[i + 1][s] = numSet[i][s];
							map[i * 2 + 2][s * 2 + 1] = ' ';
						}
					}
				}
			}

		}
	}
}

//�̷� ȭ�鿡 ���
void PrintMaze() {

	system("cls");

	gotoXY(2, 2);
	cout << "=================";
	gotoXY(2, 3);
	cout << "==   ����: q   ==";
	gotoXY(2, 4);
	cout << "=================";

	gotoXY(12, 2);
	cout << " ============================";
	gotoXY(12, 3);
	cout << " ==  ���� �̵� Ƚ�� : " << score << "   ==";
	gotoXY(12, 4);
	cout << " ============================";

	gotoXY(28, 2);
	cout << " ================================";
	gotoXY(28, 3);
	cout << " ==  @ : Ż�ⱸ, # : Ƚ�� + 1  == ";
	gotoXY(28, 4);
	cout << " ================================";

	for (int i = 0; i < H * 2 + 1; i++) {  // �̷� �׸� ���
		gotoXY(12, 8 + i);
		for (int j = 0; j < W * 2 + 1; j++) {
			cout << map[i][j];
		}
		cout << endl;
	}
}

//������ �� ����Ű �Է¹޴� �Լ�
int PressKey() { //1. up 2. down 3. left 4. right
	int key = 0;
	while (true) {
		if (_kbhit()) {
			key = _getch(); // getch()�� ����Ű �Է� ������ 224,(72/75/77/80) �������� �����Ƿ�..

			if (key == 'Q' || key == 'q') { // ������ q ������ ���� ����.
				return -1;
			}

			if (key == 224) {  // ���� Ű �Է� �޾� �÷��̾ �̵��� �� ����.
				key = _getch();
				switch (key) {
				case 72: //��
					return 0;
					break;
				case 80: //�Ʒ�
					return 1;
					break;
				case 75: //����
					return 2;
					break;
				case 77: //������
					return 3;
					break;
				default:
					break;
				}
			}
			else
				break;
		}
	}
	return -1;
}

//BFS Ž������ �̷� Ż�� �ִ� ��� �̵� Ƚ�� ��ȯ�ϴ� �Լ�.
int bfs(int ex, int ey) {

	memset(visited, 0, sizeof(visited)); // visited ���� int�� �迭�̶� ���� �� ���� �÷��� �� ���� �� �� �����Ƿ� 0���� �ʱ�ȭ
	queue<node> q; // node ����ü ��� q ����

	q.push({ 0, 0, 0 });
	visited[0][0] = true;

	while (!q.empty()) {
		node temp = q.front();
		q.pop();

		if (temp.x == ex && temp.y == ey) {  // end ���� ���� �� 
			return temp.num;
		}

		for (int i = 0; i < 4; i++) {
			int nx = temp.x + dx[i];  // 4�������� Ž���Ѵ�.
			int ny = temp.y + dy[i];

			if (nx < 0 || nx >= W || ny < 0 || ny >= W) // �̷� ���� ����ų�
				continue;
			if (visited[ny][nx])						// �̹� �湮�ߴ� ���̰ų�
				continue;
			if (map[temp.y * 2 + 1 + dy[i]][temp.x * 2 + 1 + dx[i]] != ' ')	// ����,���κ� � ���� ���� �ش� �濡 �湮x
				continue;

			visited[ny][nx] = true;				// �׷��� �ʴٸ� �湮.
			q.push({ nx, ny, temp.num + 1 });   // ���ο� �� ������ ť�� �ִ´�. 
		}
	}
}

//���� ���� ���� �Լ�. END�� ����/���� Ƚ�� �� ���� ���� ���� ����/���� ���� ���� 3���� ����.
bool SearchMaze() {

	// �̵��� 4�� �������� ������.
	int dx[4] = { 0, 0, -1, 1 }; // key���� �°� (dx,dy) ���� �̵� �� , ��/�Ʒ�/����/�������̵��� dy,dx�迭 ���� ����.
	int dy[4] = { -1, 1, 0, 0 };

	random_device rd;  // random ���̺귯�� ���. ���� ������ ���� random_device ����
	mt19937 gen(rd()); // ���� ���� ���� mt19937 ��ü gen ������ ���
	uniform_int_distribution<int> dis_x(0, W - 1); // (1 ~ W-1) ���� ������ �յ�����ϵ��� ���� ������.
	uniform_int_distribution<int> dis_y(0, H - 1);


	queue<node> q;
	node temp;
	int ex, ey; //������ �����ϰ� ����
	temp.x = 0;
	temp.y = 0;

	ex = dis_x(gen); // ���� ���� ��ġ�� mt19937 �������� ������ ������ random ����.
	ey = dis_y(gen);

	for (int i = 0; i < trap; i++) {
		int hx = dis_x(gen); int hy = dis_y(gen);
		map[hy * 2 + 1][hx * 2 + 1] = '#';       // ������ trap ������ �Էµ� ������ŭ random ��ġ�� ��ġ 
	}

	//start point�� �׻� (1, 1) ����.
	visited[0][0] = 1;
	q.push(temp);
	map[1][1] = '*';

	//ex = (rand() % W); // c ǥ�� rand() �Լ� �ᵵ �Ǵµ�, ���� ���� ������ �׸� ���� �ʾƼ� c++ �������ִ� ���� ������ ����غ�. 
	//ey = (rand() % H);

	map[ey * 2 + 1][ex * 2 + 1] = '@'; // ���� ���� ����.

	min_path = bfs(ex, ey); // bfs Ž���ؼ� ��ǥ('#')���� �ִܰŸ��� ���� min_path�� ����.
	score = min_path;


	PrintMaze(); // start point, end point, min_path ���� ä�� �̷� ���.


	while (!q.empty()) { // ť(FIFO ����)�� ��� �� ������ �ݺ� ����.

		temp = q.front(); // ť �� ��� ���� 

		if (temp.x == ex && temp.y == ey) {  // �ϴ� END ���� �����ߴ��� Ȯ��.
			return true;	// �����ߴٸ� 1 ��ȯ�ϰ� ����.
		}

		int key = PressKey();	// Ű �Է¹޾Ƽ�

		if (key == -1) {	// 'Q' �Ǵ� 'q' �� key�� -1 ����Ǵϱ�
			return false;   // 0 ��ȯ�ϰ� ����.
		}


		int nx = temp.x + dx[key]; // ���� ����Ű��� ���ο� ��ǥ (nx,ny) ����.
		int ny = temp.y + dy[key];


		if (nx < 0 || nx >= W || ny < 0 || ny >= H) { // �� ��ǥ�� �̷� �迭 ���� ����� ���. 
			gotoXY(12, 3);
			cout << "< ���� ���θ������ϴ�. >";
			continue;	 // �� ��ǥ�� �÷��̾�('*')�� �̵���Ű�� �ʰ�, ���� �޽��� ���.
		}
		if (map[temp.y * 2 + 1 + dy[key]][temp.x * 2 + 1 + dx[key]] == '-' || // ��, �Ʒ� �Է� �� [¦����][Ȧ����]�� �̵��Ǹ� '-'�� �ɸ�.
			map[temp.y * 2 + 1 + dy[key]][temp.x * 2 + 1 + dx[key]] == '|') { // ����, ������ �Է� �� [Ȧ����][¦����]�� �̵��Ǹ� '|'�� �ɸ�.
			gotoXY(12, 3);    // �� ��ǥ�� ���κ�, ���κ��� �־� ������ ���.
			cout << "< ���� ���θ������ϴ�. >";
			continue;	 // �� ��ǥ�� �÷��̾�('*')�� �̵���Ű�� �ʰ�, ���� �޽��� ���.
		}

		// ���� ���θ����� �ʰ� ����� �������ٸ� score(���� �̵� Ƚ��) -1��
		score--;

		// ������ �Ծ��ٸ� score + 1 ��(+2���ؼ� ���� score ��������)
		if (map[ny * 2 + 1][nx * 2 + 1] == '#') {  // �ϴ� END ���� �����ߴ��� Ȯ��.
			score += 2; // �����ߴٸ� 1 ��ȯ�ϰ� ����.
		}

		if (score < 0) return true;

		map[temp.y * 2 + 1][temp.x * 2 + 1] = ' '; // ���� �÷��̾�(*)�� �ִ� �ڸ��� �÷��̾� ���븦 ����.
		map[ny * 2 + 1][nx * 2 + 1] = '*';		// �̵��� �� ��ǥ�� �÷��̾�(*)�� ���� �׸�.
		PrintMaze();				// �÷��̾� �̵��ؼ� �ٲ� �̷� �׸��� �׸�.


		q.pop();			// ť���� ���� ��ǥ ����,
		q.push({ nx, ny }); // (nx,ny) ��ǥ�� ����.
	}
}


int main(void) {

	SetConsole(); //�ܼ� â ����

	srand((unsigned int)time(NULL)); // random �Լ����� ��¥ random���� ��µǵ��� ��.
	DrawGame();
	start_flag = SetGame();
	end_flag = false;

	while (true) {

		if (start_flag) {
			Input(); // line 116~ .. sŰ ���� ���� ���� ��, ���ʴ�� �̷� �ʺ�(W)�� ����(H)�� ����.
			InitMaze();  // �̷� �ʱ� ���� ����. ��� ���� ����,���κ��� �����ִ� ������.

			MadeMaze_Eller(); // ���� �� �վ �̷� ����� �ִ� �۾�.
			PrintMaze();	// ���� �̷θ� console â�� ����Ѵ�.

			end_flag = SearchMaze(); // ���� ����. ����Ű ������ �����̰�, q/Q ������ ����.

			if (end_flag) {	 // END ����(#)�� ������ ���� ����� ��� 
				ReGame();	 // �Ǵ� ���� Ƚ�� �� END ������ �������� ���� ���� ����� ���
				start_flag = SetGame();
			}
			else { // Q/q ���� ���� ����� ���.
				DrawGame();	 // ����ȭ������ ���ư�.
				start_flag = SetGame(); // �ٽ� Q/q ������ ���α׷� ���� ����.
			}
		}
		else  // start_flag = 0 �̸� ���� ������ ���� ����. 
			break;
	}
	return 0;
}