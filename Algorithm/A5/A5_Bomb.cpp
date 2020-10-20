#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <limits>
#include <queue>

using namespace std;

double dist(double x1, double y1, double x2, double y2)
{
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

class graph
{
public:
	double t, s; 
	int n, m;
	double nc[100][2], mc[100][2];
	int **net;
	int *chk, *path;
	void Init(char *dir)
	{
		FILE *in = fopen(dir, "r");
		if(fscanf(in, "%d %d",&n, &m));
		if(fscanf(in, "%lf %lf",&t, &s));
		net = new int*[n+m+2];
		chk = new int[n+m+2];
		path = new int[n+m+2];
		for(int i = 0 ; i < n+m+2;i++){
			net[i] = new int[n+m+2];
		}
		for(int i = 1 ; i <= n; i++)
			net[0][i]=1;
		for(int i = n+1 ; i<= n+m; i++)
			net[i][n+m+1]=1;
		for(int i = 0 ; i < n; i++){
			if(fscanf(in, "%lf %lf", &nc[i][0], &nc[i][1]));
		}
		for(int i = 0 ; i < m ; i++){
			if(fscanf(in, "%lf %lf", &mc[i][0], &mc[i][1]));
		}
		for(int i = 0 ; i< n; i++){
			for( int j = 0; j < m; j++){
				if(dist(nc[i][0],nc[i][1],mc[j][0],mc[j][1]) <= t * s){
					net[i+1][n+1+j]=1;
				}
			}
		}
		fclose(in);
	}
};

bool dfs(graph &g, int cur)
{
	int n = g.n;
	int m = g.m;
	if(cur == n + m +1)return true;
	for(int i = 0 ; i<n+m+2;i++){
		if(g.net[cur][i]<=0 || g.chk[i]==1)continue;
		g.chk[i] = 1;
		g.path[i] = cur;
		if(dfs(g,i))return true;
	}
	return false;
} 

int fold(graph &g)
{
	int **f, **r;
	int n = g.n;
	int m = g.m;
	int ret = 0;
	while(dfs(g,0)){
		ret++;
		int cur = n+m+1;	
		while(cur!=0){
			int next = g.path[cur];
			g.net[next][cur]--;
			g.net[cur][next]++;
			cur = next;
		}
		for(int i = 0 ; i < n+m+2 ; i++){
			g.chk[i] = 0;
			g.path[i] = 0;
		}
	}
	return ret;
}

int main(int argc, char** argv)
{
	graph g;
	FILE *out = fopen(argv[2], "w");
	g.Init(argv[1]);
	int ret = fold(g);
	fprintf(out, "%d", g.n - ret);
	fclose(out);
	return 0;
}
