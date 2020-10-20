#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <climits>

#define MAX_V 50001
#define MAX_E 200001

using namespace std;

class Set
{
public:
	int n;
	int *parent;
	void Init(int n)
	{
		parent = new int[n+1];
		for(int i = 0; i<= n ; i++)
			parent[i]= i;
	}
	int Find(int x)
	{
		if(parent[x]== x) return x;
        return parent[x] = Find(parent[x]);
	}
	bool Union(int x, int y)
	{
		x = Find(x);
		y = Find(y);
		if(x != y){
			parent[y] = x; 
			return true;
		}
		return false;
	}
};

class Graph{
public:
	int v, e, val;
	vector< pair<int, pair<int, int>> > list;
	vector< pair<int, int> > *adj;
	int chk[MAX_E];
	int dp[MAX_V][16];
	int cost[MAX_V][16];
	int depth[MAX_V];
	int max_h;
	int cnt;
	Set S;
	void Init(char *dir)
	{
		memset(chk,0,sizeof(chk));
		memset(dp,0,sizeof(dp));
		memset(cost,0,sizeof(cost));
		memset(depth,0,sizeof(depth));
		int t1, t2, t3;
		FILE *in = fopen(dir, "r");
		if(fscanf(in, "%d %d",&v, &e));
		for(int i = 0 ; i< e; i++){
			if(fscanf(in, "%d %d %d", &t1, &t2, &t3));
			list.push_back({t3, {t1, t2}});
		}
		fclose(in);
		sort(list.begin(), list.end());
		val = 0;
		cnt = 0;
		S.Init(v);
		adj = new vector< pair<int, int> >[v+1];
	}
	void kruskal()
	{
		for(int i = 0 ; i < e; i++){
			if(S.Union(list[i].second.first, list[i].second.second)){
				val += list[i].first;
				chk[i] = 1;
				cnt++;
			}
		}
	}
	void Init_dp()
	{
		for(int i = 0 ; i<list.size(); i++){
			if(!chk[i])continue;
			adj[list[i].second.first].push_back({list[i].second.second, list[i].first});
			adj[list[i].second.second].push_back({list[i].second.first, list[i].first});
		}
		max_h = (int)floor(log2(MAX_V));
	}
	void make(int cur, int prev)
	{
		int next;
		for(int i = 0 ; i<adj[cur].size();i++){
			next = adj[cur][i].first;
			if(next==prev)continue;
			depth[next] = depth[cur] + 1;
			dp[next][0] = cur;
			cost[next][0] = adj[cur][i].second;
			make(next,cur);
		}
	}
	void set_dp()
	{
		Init_dp();
		make(1,0);
		for(int i = 1; i<=max_h; i++){
			for(int j = 1; j<=v; j++){
				dp[j][i] = dp[dp[j][i-1]][i-1];           
				cost[j][i] = max(cost[dp[j][i - 1]][i - 1] , cost[j][i - 1]);
			}
		}
	}
	int s_max(int x, int i, int w)
	{
		if (i == 0) return -1;
		int ret = -1;
		if(cost[x][i-1] == w)
			ret = max(ret, s_max(x,i-1, w));
		else ret = max(ret, cost[x][i-1]);
		if(cost[dp[x][i-1]][i-1] == w)
			ret = max(ret, s_max(dp[x][i-1], i-1, w));
		else ret = max(ret, cost[dp[x][i-1]][i-1]);
		return ret;
	}

	int lca(int x, int y, int w)
	{
		int ret = -1;
		if(depth[x] != depth[y]){
			if( depth[x] > depth[y])
				swap(x,y);
			for(int i = max_h; i>=0 ; i--){
				if(depth[y] - depth[x] >= (1 << i)){
					if(cost[y][i] == w)
						ret = max(ret, s_max(y,i,w));
					else
						ret = max(ret , cost[y][i]);
					y = dp[y][i];
				}
			}
		}
		if (x==y)return ret;
		else
		{
			for(int i = max_h; i>=0 ; i--){
				if(dp[x][i] != dp[y][i]){
					if(cost[x][i] == w) ret = max(ret, s_max(x,i,w));
					else ret = max(ret, cost[x][i]);

					if(cost[y][i] == w) ret = max(ret, s_max(y,i,w));
					else ret = max(ret , cost[y][i]);
					x = dp[x][i];
					y = dp[y][i];
				}
			}
		}
		if(cost[x][0] < w) ret = max(ret, cost[x][0]);
		if(cost[y][0] < w) ret = max(ret, cost[y][0]);
		return ret;
	}

	int answer()
	{
		int ret = INT_MAX, w;
		for(int i = 0; i < e; i++){
			if(!chk[i]){
				w = lca(list[i].second.first, list[i].second.second, list[i].first);
				if(w == -1)continue;
				ret = min(ret, val - w + list[i].first);
			}
		}
		return ret;
	}
};
		
int main(int argc, char** argv)
{
	Graph g;
	int ret;
	g.Init(argv[1]);	
	g.kruskal();
	if(g.e <= g.v-1 || g.cnt != g.v-1) ret = -1;
	else{	
		g.set_dp();
		ret = g.answer();
	}
	FILE *out = fopen(argv[2], "w");
	fprintf(out, "%d", ret);
	fclose(out);
	return 0;
}
