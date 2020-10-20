#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <limits>
#include <queue>

using namespace std;

class Q
{
public:
	int *A;
	int head;
	int tail;
	int n;
	void Init(int _n)
	{
		n = _n;
		A = new int[n];
		head = 0;
		tail = -1;
	}
	bool empty()
	{
		if(tail + 1 == head)return true;
		else return false;
	}
	void push(int t)
	{
		tail++;
		A[tail] = t;
	}
	int pop()
	{
		int ret = A[head];
		head++;
		return ret;
	}
	void print()
	{
		for(int i = 0 ; i < n; i++)printf("%d ",A[i]);
	}
};

class graph
{
public:
	vector<int> *list;
	int *w;
	int *d;
	int n;
	int m;
	void Init(char *dir)
	{
		FILE *in = fopen(dir, "r");
		if(fscanf(in, "%d %d",&n, &m));
		w = new int[n+1];
		d = new int[n+1];
		list = new vector<int>[n+1];
		for(int i = 1 ; i <= n; i++){
			if(fscanf(in, "%d", &w[i]));
			d[i] = 0;
		}
		for(int i = 0 ; i < m; i++){
			int t1, t2;
			if(fscanf(in, "%d %d", &t1, &t2));
			list[t1].push_back(t2);
			d[t2]++;
		}
		fclose(in);
	}
};

int critical(graph &g, Q &q)
{
	int *deg, *time, n;
	n = g.n;
	deg = new int[n+1];
	time = new int[n+1];
	for(int i = 1 ; i<= n; i++){
		time[i] = 0;
		deg[i] = g.d[i];
		if(deg[i] == 0)q.push(i);
	}
	while(!q.empty()){
		int cur = q.pop();
		time[cur]+= g.w[cur];
		for(int i = 0; i < g.list[cur].size(); i++){
			int tmp = g.list[cur][i];
			deg[tmp]--;
			if(deg[tmp]==0)q.push(tmp);
			time[tmp] = max(time[cur], time[tmp]);
		}
	}
	int maxn = time[1];
	for(int i = 1 ; i <= n; i++){
		if(deg[i]!=0)return -1;
		maxn = max(maxn, time[i]);
	}
	return maxn;
}

int main(int argc, char** argv)
{
	graph g;
	Q q;
	FILE *out = fopen(argv[2], "w");
	g.Init(argv[1]);
	q.Init(g.n);
	int ret = critical(g,q);
	fprintf(out, "%d", ret);
	fclose(out);
	return 0;
}
