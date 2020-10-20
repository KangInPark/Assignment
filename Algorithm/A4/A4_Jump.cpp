#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <limits>
#include <queue>

using namespace std;

class Graph
{
public:
	int v;
	int n, G;
	int start;
	int **map;
	vector < pair<int, int> > list;
	vector < pair<int, double> > *adj;
	void Init(char *dir)
	{
		FILE *in = fopen(dir, "r");
		if(fscanf(in, "%d %d",&n, &G));
		v = n + 1;
		list.push_back({0,0});
		for(int i = 0 ; i < n; i++){
			int t1, t2;
			if(fscanf(in, "%d %d", &t1, &t2));
			list.push_back({t1, t2});
		}
		fclose(in);
		sort(list.begin(), list.end());
		adj = new vector < pair<int, double> >[n+2];
		for(int i = 0 ; i<list.size(); i++){
			int x = list[i].first;
			int y = list[i].second;
			int j = i + 1;
			while(abs(list[j].first - x) <=2 ){
				if( abs(list[j].second - y) <= 2){
					double w = sqrt(pow(x - list[j].first, 2) + pow(y - list[j].second, 2));
					adj[i+1].push_back({j+1, w});
					adj[j+1].push_back({i+1, w});
				}
				j++;
			}
		}
	}
};

class node
{
public:
	int index;
	double w;
	node(int _index, double _w){
		index = _index;
		w = _w;
	}
	bool operator < (const node & x) const
	{
		return this->w > x.w;
	}
};

int dij(Graph &g)
{
	int table[g.v + 1];
	double weight[g.v + 1]; 
	for(int i = 1 ; i<=g.v; i++){
		table[i] = 0;
		weight[i] = numeric_limits<double>::max();
	}
	weight[1] = 0.0;

	priority_queue<node> pq;
	pq.push(node(1, 0.0));
	while(!pq.empty())
	{
		node cur = pq.top();
		pq.pop();
		if(table[cur.index])continue;
		table[cur.index]= 1;
		for(int i = 0 ; i<g.adj[cur.index].size(); i++){
			node next = node(g.adj[cur.index][i].first, g.adj[cur.index][i].second);
			if(!table[next.index]){
				if( weight[next.index] > cur.w + next.w){
					weight[next.index] = cur.w + next.w;
					pq.push(node(next.index, weight[next.index]));
				}
			}
		}
	}
	double tmp = numeric_limits<double>::max();
	for(int i = 1 ; i< g.list.size(); i++){
		if(g.G <= g.list[i].second){
			tmp = min(tmp, weight[i+1]);	
		}
	}
	return (int)round(tmp);
}

int main(int argc, char** argv)
{
	Graph g;
	g.Init(argv[1]);
	FILE *out = fopen(argv[2], "w");
	if(g.G == 0){
		fprintf(out, "0");
		fclose(out);
		return 0;
	}
	fprintf(out, "%d", dij(g));
	fclose(out);
	return 0;
}
