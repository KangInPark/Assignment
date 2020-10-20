#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>

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
	vector< pair<int, pair<int, int>> > MST;
	Set S;
	void Init(char *dir)
	{
		FILE *in = fopen(dir, "r");
		int t1, t2, t3;
		if(fscanf(in, "%d %d",&v, &e));
		for(int i = 0 ; i< e; i++){
			if(fscanf(in, "%d %d %d", &t1, &t2, &t3));
			list.push_back({t3, {t1, t2}});
		}
		fclose(in);
		sort(list.begin(), list.end());
		val = 0;
		S.Init(v);
	}

	void kruskal()
	{
		for(int i = 0 ; i < e; i++){
			if(S.Union(list[i].second.first, list[i].second.second)){
				val += list[i].first;
				MST.push_back(list[i]);
			}
		}
	}
};

int main(int argc, char** argv)
{
	Graph g;
	g.Init(argv[1]);
	g.kruskal();
	FILE *out = fopen(argv[2], "w");
	fprintf(out, "%d", g.val);
	fclose(out);
	return 0;
}
