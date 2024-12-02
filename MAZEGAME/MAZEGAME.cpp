#include<iostream>
#include<Windows.h>
#include<conio.h>
#include<queue>
#include<random>
#include<time.h>
using namespace std;

bool start_flag, end_flag; // 게임 시작, 종료 여부
int W, H, trap; //미로의 너비, 높이
char map[50][50]; //전체 미로 맵
int visited[20][20]; //방 탐색 여부 표시
int numSet[20][20]; //Eller 알고리즘에서 사용할 각 방의 집합 번호
int score; //미로 게임 진행 시 남은 이동 횟수
int min_path; //bfs로 탐색한 경로 길이

int dx[4] = { 0, 0, 1, -1 };  // 이동은 4개 방향으로만 가능
int dy[4] = { 1, -1, 0, 0 };

typedef struct node { // BFS 탐색에서 쓰고, 각 node는 방이다.
	int x;		// 방의 x좌표, y좌표
	int y;
	int num;	// bfs 탐색으로 얻어낸, 해당 방에 이르기까지 최소 이동 횟수
};

//콘솔 내부의 특정 위치로 커서를 이동시키는 함수 
void gotoXY(int x, int y) {
	COORD pos;
	pos.X = x * 2;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

//콘솔 창 세팅하는 함수
void SetConsole() {

	system("mode con:cols=100 lines=45");
	system("title MazeGame");
}

//게임 시작화면 그리는 함수
void DrawGame() {

	system("cls");
	gotoXY(10, 6);  // gotoXY는 해당 좌표로 커서 이동하는 함수.
	cout << "===========================================================";
	gotoXY(10, 8);
	cout << "===================      Maze Game      ===================";
	gotoXY(10, 10);
	cout << "===========================================================";
	gotoXY(20, 14);
	cout << " 's' key 눌러 시작";
	gotoXY(20, 16);
	cout << " 'q' key 눌러 종료";

}

//게임 종료 후 승패 보여주는 함수
void ReGame() {
	system("cls");  // 일단 게임 화면 싹 지움.
	gotoXY(19, 6);
	cout << "   남은 이동 횟수 : " << score + 1;  // score 출력 후 
	gotoXY(17, 8);
	cout << "================================";
	gotoXY(17, 10);
	if ((end_flag == 1) && (score < 0))	// score, min_path(bfs 탐색으로 찾은 최소 이동 수) 대소 관계에 따라 LOSE , WIN 출력 
		cout << "==           LOSE(Q.Q)        ==";
	else
		cout << "==            WIN(^_^)        ==";
	gotoXY(17, 12);
	cout << "================================";

	gotoXY(10, 16);
	cout << "===========================================================";
	gotoXY(10, 18);
	cout << "====               다시 시작하겠습니까?                 ===";
	gotoXY(10, 20);
	cout << "===========================================================";
	gotoXY(20, 24);
	cout << "'s' key: 한 번 더!";
	gotoXY(20, 26);
	cout << "'q' key: 종료";
}

//게임 시작/종료 키 입력받는 함수
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

// 미로의 너비, 높이 입력하는 함수
void Input() {
	system("cls");
	gotoXY(15, 12);
	cout << "< 2 이상, 15이하인 정수를 입력해 주세요. >";
	gotoXY(17, 16);
	cout << "미로의 가로 길이 입력: ";
	cin >> W;
	gotoXY(17, 18);
	cout << "미로의 세로 길이 입력: ";
	cin >> H;
	gotoXY(17, 20);
	cout << "미로 아이템 개수 입력: ";
	cin >> trap;
}

// 미로에 대한 정보들 초기화하는 함수
void InitMaze() {
	int i, j;

	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) {
			numSet[i][j] = i * W + j; // numSet은 방번호 배열. 방 번호 지정 방식을 이렇게 선형적으로 분배함.
			visited[i][j] = false;      // visited은 방 탐색 여부 나타낸 배열. 방 탐색 여부 모두 0로 표시
		}
	}
	// numSet이 진짜 플레이어가 돌아다닐 수 있는 방(공간) 나타내는 배열,
	// map은 numset의 각 방마다 세로벽('|'), 가로벽('-') 으로 사방이 둘러싸이도록 디코더처럼 연결된 배열로 맵 구조를 나타냄.

	for (i = 0; i < H * 2 + 1; i++) {		// 너비, 높이 모두 15까지 입력 가능한 이유임..
		for (j = 0; j < W * 2 + 1; j++) {
			if (i % 2 == 0) {	// 짝수 번째 행에는 미로 가로 구조물(가로 벽)을 그려준다.
				if (j % 2 == 0) {		// 게임 초기 화면 미로 그리는 중.. 
					map[i][j] = '+';   // 짝수행, 짝수열 칸에는 '+'를,
				}
				else {
					map[i][j] = '-';   // 짝수행, 홀수열의 칸에는 가로벽('-')를 그려준다.
				}
			}
			else {		// 홀수 번째 행에는 미로 세로 구조물(세로 벽)을 그려준다.
				if (j % 2 == 0) {	  // 홀수 행, 짝수 열에만 미로 세로 구조물(세로벽)을 그린다.
					map[i][j] = '|';  // for문 i,j 조건 보면 '<' 이므로 각 행 마지막에 세로벽 그리고 다음 행(i+1)으로 넘어가게됨
				}					  // 그럼 0 ~ W 까지 세로벽은 (W + 1)개가 그려지게 된다.
				else {
					map[i][j] = ' '; // 홀수 행, 홀수 열은 빈 공간(플레이어가 돌아다닐 공간)으로 시야에 안거슬리게 빈칸으로 냅둠.
				}
			}
		}
	}			// 이렇게 하면 현재는 가로벽, 세로벽으로 미로가 다 막혀있는 상태.. MadeMaze_Eller 함수(바로 다음 실행)에서 길 뚫어줄 예정. 
}

// Eller 알고리즘으로 완전 미로 만들기(InitMaze에서 만든 방들 길 뚫는 작업)
void MadeMaze_Eller() {
	int i, j, k;

	for (i = 0; i < H; i++) { // 수직벽 제거, 수평벽 제거 모두 한 행씩 진행
		int remove_wall; //벽 제거할지 결정하는 flag

		for (j = 0; j < W - 1; j++) { // 수직 벽 제거 루프

			// 마지막 행일 때, 서로 다른 방 번호 가진 벽들 모두 제거 
			if (i == H - 1) {
				remove_wall = 1;
			}
			else { // 마지막 행이 아니라면, 수직 벽 제거 random으로 결정
				remove_wall = rand() % 2;
			}

			if ((remove_wall == 1) && (numSet[i][j + 1] != numSet[i][j])) { // 수직벽 제거 작업. 
				// removewall flag가 1이고, 인접한 방끼리 번호가 다르다면(=벽에 막혀있다면)
				numSet[i][j + 1] = numSet[i][j]; // 인접한 방끼리 방 번호 같도록 해주고
				int nx = j * 2 + 2;	 // 수직벽(홀수행, 짝수열 칸) 제거 작업함. 
				int ny = i * 2 + 1;	 // 수직 벽을 애초에 홀수행, 짝수열 칸에만 그렸고,  		
				map[ny][nx] = ' ';	// nx = j * 2 + 2; 이므로 numset[i][j]의 오른쪽 세로 벽이 제거됨.
			}						// j-1 칸과 비교였다면 nx = j * 2 로 해도 됬겠지만, 그러면 for문 범위 자체를 바꿔야됨. 

		}

		if (i == H - 1) // 마지막 행에 도달했다면 수평벽 제거하면 안됨.. 
			break;		// while문 탈출.



		int cnt = 1; // 현재 행에서 같은 집합 속 방의 개수 

		for (j = 0; j < W; j += cnt) { // 수평 벽 제거 루프
			cnt = 1;

			if (j < W - 1) { // 맨 오른쪽 방이 아니라면
				for (k = j + 1; k < W; k++) {   // 해당 방 numSet[i][j]의 오른쪽 방부터 탐색한다.
					if (numSet[i][j] != numSet[i][k]) // 방 번호가 다른 방 나올 때까지 탐색하며
						break;
					cnt++;				// 다른 방번호가 나오지 않았다면 해당 방의 가로 길이(cnt)를 +1함
				}
			}
			// 방의 가로 길이가 1인 경우(cnt++ 안됨 = numSet[i][j+1]과 numSet[i][j]의 방번호 다름 = 방 바로 오른쪽에 수직벽으로 가로막힘)
			if (cnt == 1) {
				numSet[i + 1][j] = numSet[i][j]; // 아래 가로벽 제거하면 연결되기 때문에 바로 아래 방 번호도 같게 맞춰줌.
				map[i * 2 + 2][j * 2 + 1] = ' '; // 제거. 고립된 방을 만들면 안되므로.. 해당 방의 바로 아래 가로벽(짝수행,홀수열에 위치) 제거함.
			}
			else {		// 가로 길이가 2 이상인 경우. 기본적으로 모든 같은 방 번호로 연결된 component에 일단 아래로 내려갈 길 하나씩은 터줄거임.

				// 일단 그 가로벽 하나 제거해주는 작업
				int remove_idx = rand() % cnt;			// 일단 제일 왼쪽에 있는 가로 길이 2이상짜리 방은 아래 가로벽 제거해서 길 터준다.
				remove_idx += j; // numSet[i][j]가 방 위치이므로.. 제거할 가로벽 인덱스(remove_idx)는 가로길이 cnt에 맞춰 random으로 정해줌.

				numSet[i + 1][remove_idx] = numSet[i][remove_idx]; // 아래 가로벽("-",짝수행,홀수열에 위치) 제거 작업.
				map[i * 2 + 2][remove_idx * 2 + 1] = ' ';

				// 하나 제거했으면 나머지 아래 가로벽은 random으로 제거
				for (int s = j; s < j + cnt; s++) { // 같은 방 번호 칸 돌면서
					if (s != remove_idx) {
						remove_wall = rand() % 2;   // random으로 아래 가로벽 제거 

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

//미로 화면에 출력
void PrintMaze() {

	system("cls");

	gotoXY(2, 2);
	cout << "=================";
	gotoXY(2, 3);
	cout << "==   종료: q   ==";
	gotoXY(2, 4);
	cout << "=================";

	gotoXY(12, 2);
	cout << " ============================";
	gotoXY(12, 3);
	cout << " ==  남은 이동 횟수 : " << score << "   ==";
	gotoXY(12, 4);
	cout << " ============================";

	gotoXY(28, 2);
	cout << " ================================";
	gotoXY(28, 3);
	cout << " ==  @ : 탈출구, # : 횟수 + 1  == ";
	gotoXY(28, 4);
	cout << " ================================";

	for (int i = 0; i < H * 2 + 1; i++) {  // 미로 그림 출력
		gotoXY(12, 8 + i);
		for (int j = 0; j < W * 2 + 1; j++) {
			cout << map[i][j];
		}
		cout << endl;
	}
}

//게임할 때 방향키 입력받는 함수
int PressKey() { //1. up 2. down 3. left 4. right
	int key = 0;
	while (true) {
		if (_kbhit()) {
			key = _getch(); // getch()로 방향키 입력 받으면 224,(72/75/77/80) 연속으로 들어오므로..

			if (key == 'Q' || key == 'q') { // 언제든 q 누르면 종료 가능.
				return -1;
			}

			if (key == 224) {  // 방향 키 입력 받아 플레이어가 이동할 수 있음.
				key = _getch();
				switch (key) {
				case 72: //위
					return 0;
					break;
				case 80: //아래
					return 1;
					break;
				case 75: //왼쪽
					return 2;
					break;
				case 77: //오른쪽
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

//BFS 탐색으로 미로 탈출 최단 경로 이동 횟수 반환하는 함수.
int bfs(int ex, int ey) {

	memset(visited, 0, sizeof(visited)); // visited 전역 int형 배열이라 여러 번 게임 플레이 시 오류 날 수 있으므로 0으로 초기화
	queue<node> q; // node 구조체 담는 q 선언

	q.push({ 0, 0, 0 });
	visited[0][0] = true;

	while (!q.empty()) {
		node temp = q.front();
		q.pop();

		if (temp.x == ex && temp.y == ey) {  // end 지점 도착 시 
			return temp.num;
		}

		for (int i = 0; i < 4; i++) {
			int nx = temp.x + dx[i];  // 4방향으로 탐색한다.
			int ny = temp.y + dy[i];

			if (nx < 0 || nx >= W || ny < 0 || ny >= W) // 미로 범위 벗어나거나
				continue;
			if (visited[ny][nx])						// 이미 방문했던 방이거나
				continue;
			if (map[temp.y * 2 + 1 + dy[i]][temp.x * 2 + 1 + dx[i]] != ' ')	// 가로,세로벽 등에 막힌 경우면 해당 방에 방문x
				continue;

			visited[ny][nx] = true;				// 그렇지 않다면 방문.
			q.push({ nx, ny, temp.num + 1 });   // 새로운 방 정보를 큐에 넣는다. 
		}
	}
}

//게임 진행 과정 함수. END에 도착/제한 횟수 내 도착 못해 게임 엔딩/게임 도중 퇴장 3가지 가능.
bool SearchMaze() {

	// 이동은 4개 방향으로 가능함.
	int dx[4] = { 0, 0, -1, 1 }; // key값에 맞게 (dx,dy) 방향 이동 시 , 위/아래/왼쪽/오른쪽이도록 dy,dx배열 순서 맞춤.
	int dy[4] = { -1, 1, 0, 0 };

	random_device rd;  // random 라이브러리 사용. 난수 생성을 위해 random_device 생성
	mt19937 gen(rd()); // 난수 생성 엔진 mt19937 객체 gen 생성해 사용
	uniform_int_distribution<int> dis_x(0, W - 1); // (1 ~ W-1) 범위 내에서 균등분포하도록 난수 생성함.
	uniform_int_distribution<int> dis_y(0, H - 1);


	queue<node> q;
	node temp;
	int ex, ey; //목적지 랜덤하게 설정
	temp.x = 0;
	temp.y = 0;

	ex = dis_x(gen); // 엔딩 지점 위치를 mt19937 엔진으로 생성한 난수로 random 지정.
	ey = dis_y(gen);

	for (int i = 0; i < trap; i++) {
		int hx = dis_x(gen); int hy = dis_y(gen);
		map[hy * 2 + 1][hx * 2 + 1] = '#';       // 아이템 trap 변수에 입력된 개수만큼 random 위치에 배치 
	}

	//start point는 항상 (1, 1) 지점.
	visited[0][0] = 1;
	q.push(temp);
	map[1][1] = '*';

	//ex = (rand() % W); // c 표준 rand() 함수 써도 되는데, 난수 생성 분포가 그리 고르지 않아서 c++ 지원해주는 난수 생성기 사용해봄. 
	//ey = (rand() % H);

	map[ey * 2 + 1][ex * 2 + 1] = '@'; // 게임 종료 지점.

	min_path = bfs(ex, ey); // bfs 탐색해서 목표('#')까지 최단거리를 구해 min_path에 넣음.
	score = min_path;


	PrintMaze(); // start point, end point, min_path 구한 채로 미로 출력.


	while (!q.empty()) { // 큐(FIFO 구조)가 비게 될 경우까지 반복 실행.

		temp = q.front(); // 큐 앞 요소 빼서 

		if (temp.x == ex && temp.y == ey) {  // 일단 END 지점 도착했는지 확인.
			return true;	// 도착했다면 1 반환하고 종료.
		}

		int key = PressKey();	// 키 입력받아서

		if (key == -1) {	// 'Q' 또는 'q' 면 key에 -1 저장되니까
			return false;   // 0 반환하고 종료.
		}


		int nx = temp.x + dx[key]; // 눌린 방향키대로 새로운 좌표 (nx,ny) 설정.
		int ny = temp.y + dy[key];


		if (nx < 0 || nx >= W || ny < 0 || ny >= H) { // 새 좌표가 미로 배열 범위 벗어나는 경우. 
			gotoXY(12, 3);
			cout << "< 벽에 가로막혔습니다. >";
			continue;	 // 새 좌표로 플레이어('*')를 이동시키지 않고, 에러 메시지 출력.
		}
		if (map[temp.y * 2 + 1 + dy[key]][temp.x * 2 + 1 + dx[key]] == '-' || // 위, 아래 입력 시 [짝수행][홀수열]로 이동되며 '-'에 걸림.
			map[temp.y * 2 + 1 + dy[key]][temp.x * 2 + 1 + dx[key]] == '|') { // 왼쪽, 오른쪽 입력 시 [홀수행][짝수열]로 이동되며 '|'에 걸림.
			gotoXY(12, 3);    // 새 좌표에 가로벽, 세로벽이 있어 막히는 경우.
			cout << "< 벽에 가로막혔습니다. >";
			continue;	 // 새 좌표로 플레이어('*')를 이동시키지 않고, 에러 메시지 출력.
		}

		// 벽에 가로막히지 않고 제대로 움직였다면 score(남은 이동 횟수) -1점
		score--;

		// 아이템 먹었다면 score + 1 점(+2점해서 깎인 score 보상해줌)
		if (map[ny * 2 + 1][nx * 2 + 1] == '#') {  // 일단 END 지점 도착했는지 확인.
			score += 2; // 도착했다면 1 반환하고 종료.
		}

		if (score < 0) return true;

		map[temp.y * 2 + 1][temp.x * 2 + 1] = ' '; // 원래 플레이어(*)가 있던 자리에 플레이어 자취를 없앰.
		map[ny * 2 + 1][nx * 2 + 1] = '*';		// 이동한 새 좌표에 플레이어(*)를 새로 그림.
		PrintMaze();				// 플레이어 이동해서 바뀐 미로 그림을 그림.


		q.pop();			// 큐에서 기존 좌표 빼고,
		q.push({ nx, ny }); // (nx,ny) 좌표를 넣음.
	}
}


int main(void) {

	SetConsole(); //콘솔 창 세팅

	srand((unsigned int)time(NULL)); // random 함수에서 진짜 random값이 출력되도록 함.
	DrawGame();
	start_flag = SetGame();
	end_flag = false;

	while (true) {

		if (start_flag) {
			Input(); // line 116~ .. s키 눌러 게임 시작 시, 차례대로 미로 너비(W)와 높이(H)를 받음.
			InitMaze();  // 미로 초기 상태 생성. 모든 방이 가로,세로벽에 막혀있는 상태임.

			MadeMaze_Eller(); // 벽에 길 뚫어서 미로 만들어 주는 작업.
			PrintMaze();	// 만든 미로를 console 창에 출력한다.

			end_flag = SearchMaze(); // 게임 실행. 방향키 누르면 움직이고, q/Q 누르면 종료.

			if (end_flag) {	 // END 지점(#)에 도착해 정상 종료된 경우 
				ReGame();	 // 또는 제한 횟수 내 END 지점에 도달하지 못해 게임 종료된 경우
				start_flag = SetGame();
			}
			else { // Q/q 눌러 강제 종료된 경우.
				DrawGame();	 // 시작화면으로 돌아감.
				start_flag = SetGame(); // 다시 Q/q 누르면 프로그램 실행 종료.
			}
		}
		else  // start_flag = 0 이면 루프 끝내고 게임 종료. 
			break;
	}
	return 0;
}