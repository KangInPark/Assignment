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
	Set(int _n)
	{
		n = _n;
	}
	void Init()
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
	int Union(int x, int y)
	{
		x = Find(x);
		y = Find(y);
		if(x != y)parent[y] = x;
	}
	char Check(int x, int y)
	{
		x = Find(x);
		y = Find(y);
		return x==y ? 'Y' : 'N';
	}
};

int main(int argc, char** argv)
{
	int n , m, op, x, y;
	FILE *in = fopen(argv[1], "r");
	FILE *out = fopen(argv[2], "w");
	if(fscanf(in, "%d %d", &n, &m));
	Set S(n);
	S.Init();
	for(int i = 0; i <m; i++){
		if(fscanf(in, "%d %d %d", &op, &x, &y));
		if(op == 0)S.Union(x, y);
		else{
			fprintf(out, "%c\n", S.Check(x, y));
		}
	}
	fclose(in);
	fclose(out);
	return 0;
}
