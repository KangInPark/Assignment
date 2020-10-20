#include <stdio.h>
#include <stack>
#include <vector>

using namespace std;
int main(int argc, char* argv[])
{
	if(argc<3)return -1;
	FILE *f1 = fopen( argv[1], "r");
	FILE *f2 = fopen( argv[2], "w");
	stack<char> S;
	stack<double> calc;
	vector<char> list;
	int tmp[200] = {0};
	double a, b;
	tmp['/'] = 2;
	tmp['*'] = 2;
	tmp['+'] = 1;
	tmp['-'] = 1;
	char c;
	while(0<fscanf(f1, " %c", &c)){
		if(c >= '1' && c <= '9')list.push_back(c);
		else if(S.empty())S.push(c);
		else if(c == '(')S.push(c);
		else if(c == ')'){
			while(S.top()!='('){
				list.push_back(S.top());
				S.pop();
			}
			S.pop();
		}
		else if(tmp[c] <= tmp[S.top()]){
			list.push_back(S.top());
			S.pop();
			S.push(c);
		}
		else{
			S.push(c);
		}
	}
	fclose(f1);
	while(!S.empty()){
		list.push_back(S.top());
		S.pop();
	}
	for(int i = 0; i<list.size();i++){
	       	if(list[i]>='1' && list[i]<='9')calc.push((double)(list[i]-'0'));
		else{
			b = calc.top();
			calc.pop();
			a = calc.top();
			calc.pop();
			if( list[i] == '+')calc.push(a+b);
			else if(list[i] == '-')calc.push(a-b);
			else if(list[i] == '*')calc.push(a*b);
			else calc.push(a/b);
		}
	}
	fprintf(f2, "%.2f", calc.top());
	fclose(f2);
	return 0;
}		
