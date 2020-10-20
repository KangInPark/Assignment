#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <climits>
#include <queue>

using namespace std;

class Graph
{
public:
	int v, e;
	int start;
	vector < pair<int, int> > *adj;
	void Init(char *dir)
	{
		FILE *in = fopen(dir, "r");
		if(fscanf(in, "%d %d %d",&v, &e, &start));
		adj = new vector < pair<int, int> >[v+1];
		for(int i = 0 ; i < e; i++){
			int t1, t2, t3;
			if(fscanf(in, "%d %d %d", &t1, &t2, &t3));
			adj[t2].push_back({t1,t3});
		}
		fclose(in);
	}
};

class node
{
public:
	int index;
	int w;
	node(int _index, int _w){
		index = _index;
		w = _w;
	}
	bool operator < (const node & x) const
	{
		return this->w > x.w;
	}
};

void dij(Graph &g, int start, char *dir)
{
	int table[g.v + 1][2]; // 0 : chk , 1 : weight
	for(int i = 1 ; i<=g.v; i++){
		table[i][0] = 0;
		table[i][1] = INT_MAX;
	}
	table[start][1] = 0;

	priority_queue<node> pq;
	pq.push(node(start, 0));
	while(!pq.empty())
	{
		node cur = pq.top();
		pq.pop();
		if(table[cur.index][0])continue;
		table[cur.index][0] = 1;
		for(int i = 0 ; i<g.adj[cur.index].size(); i++){
			node next = node(g.adj[cur.index][i].first, g.adj[cur.index][i].second);
			if(!table[next.index][0]){
				if(table[next.index][1] > cur.w + next.w){
					table[next.index][1] = cur.w + next.w;
					pq.push(node(next.index, table[next.index][1]));
				}
			}
		}
	}
	int tmp = -1;
	int tmpi = 0;
	for(int i = 1 ; i<=g.v; i++){
		if(table[i][1] != INT_MAX)
		{
			tmpi++;
			if(table[i][1] > tmp){
				tmp = table[i][1];
			}
		}
	}
	FILE *out = fopen(dir, "w");
	fprintf(out, "%d %d",tmpi, tmp);
	fclose(out);
}

int main(int argc, char** argv)
{
	Graph g;
	g.Init(argv[1]);
	dij(g, g.start, argv[2]);
	return 0;
}
