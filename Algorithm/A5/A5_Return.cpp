#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <limits>
#include <queue>

using namespace std;

class graph
{
public:
	int ** adj;
	int n;
	int m;
	int s;
	int t;
	int map[26][26];
	int dx[4] = {0, 1, 0, -1};
	int dy[4] = {1, 0, -1, 0};
	void Init(char *dir)
	{
		FILE *in = fopen(dir, "r");
		if(fscanf(in, "%d %d",&n, &m));
		if(fscanf(in, "%d %d",&t, &s));
		adj = new int*[n*m+1];
		for(int i = 0 ; i< n*m+1; i++){
			adj[i] = new int[n*m+1];
		}
		for(int i = 1 ; i <= n; i++){
			for(int j = 1 ; j<=m; j++){
				if(fscanf(in, "%d", &map[i][j]));
			}
		}
		for(int i = 1 ; i<= n*m ; i++){
			for(int j = 1; j <= n*m ; j++){
				if(i == j)adj[i][j] = 0;
				else adj[i][j] = 99999999;
			}
		}
		for(int i = 1 ; i<=n; i++){
			for(int j = 1; j <= m; j++){
				for(int k = 0; k < 4; k++){
					int nx = i + dx[k];
					int ny = j + dy[k];
					if(nx>=1 && ny>=1 && nx<=n && ny<=m && abs(map[i][j]-map[nx][ny])<=t){
						int t1 = (i-1) * m + j;
						int t2 = (nx-1) * m + ny;
						int cost = map[i][j] >= map[nx][ny] ? 1.0 : (map[i][j] - map[nx][ny]) * (map[i][j] - map[nx][ny]);
						adj[t1][t2] = cost;
					}
				}
			}
		}
		fclose(in);
	}
};

int floyd(graph &g)
{
	int n = g.n;
	int m = g.m;
	for(int k = 1 ; k<= n * m; k++){
		for(int i = 1; i<= n * m; i++){
			for(int j = 1; j<= n * m; j++){
				if(g.adj[i][j] > g.adj[i][k] + g.adj[k][j]){
					g.adj[i][j] = g.adj[i][k] + g.adj[k][j];
				}
			}
		}
	}
	int ret = 0;
	int mx, my;
	for(int i = 1 ; i<= n * m ; i++){
		int tmp = g.adj[1][i] + g.adj[i][1];
		if( tmp <= g.s){
			mx = (i-1) / m + 1;
			my = i - (mx-1) * m;
			ret = max(ret, g.map[mx][my]);
		}
	}
	return ret;
}

int main(int argc, char** argv)
{
	graph g;
	FILE *out = fopen(argv[2], "w");
	g.Init(argv[1]);
	int ret = floyd(g);
	fprintf(out, "%d", ret);
	fclose(out);
	return 0;
}
