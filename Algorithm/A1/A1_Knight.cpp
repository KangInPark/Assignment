#include <stdio.h>
#include <queue>
#include <utility>

using namespace std;
int main(int argc, char* argv[])
{
	if(argc<3)return -1;
	FILE* f1 = fopen(argv[1], "r");
	FILE* f2 = fopen(argv[2], "w");

	int n, nx, ny, x, y, chk = 0;
	queue<pair<int, int>> Q;
	pair<int, int> tmp, goal;
	int dx[8] = {-1, 1, 2, 2, 1, -1, -2, -2};
	int dy[8] = {-2, -2, -1, 1, 2, 2, 1, -1};
	int* map;

	if(fscanf(f1,"%d",&n));
	map = new int[n*n];
	for(int i = 0 ; i< n*n ; i++){
		map[i] = 99999;
	}
	if(fscanf(f1, "%d %d", &tmp.first, &tmp.second));
	Q.push(tmp);
	map[tmp.first * n + tmp.second] = 0;
	if(fscanf(f1, "%d %d", &goal.first, &goal.second));
	fclose(f1);	
	while(!Q.empty()){
		tmp = Q.front();
		if(tmp == goal){
			chk = 1;
			break;
		}
		Q.pop();
		x = tmp.first;
		y = tmp.second;
		for(int i = 0; i < 8; i++){
			nx = x + dx[i];
			ny = y + dy[i];
			if(nx>=0 && ny>=0 && ny<n && nx<n)
			{
				if(map[nx*n+ny]>map[x*n+y]+1){
					Q.push(make_pair(nx, ny));
					map[nx*n+ny] = map[x*n + y] + 1;
				}
			}
		}
	}
	if(chk == 0)fprintf(f2, "-1");
	else{
		chk = map[goal.first*n + goal.second];
		if(chk==0)fprintf(f2,"1");
		else fprintf(f2,"%d",chk);
	}
	fclose(f2);
	return 0;
}
