#include <stdio.h>

float map[7][7];
float memo[7][1<<7]; 
int cntfunc[3];
int cnttravel[3];
int cnt0 = 0;
int cnt1 = 0;
float sqrt(float x)
{
        int cnt = 0;
        float n = x / 2.0;
        float t = 0;
        while( n != t){
                t = n;
                n = (n + x/n) / 2.0;
                cnt++;
        }
        printf("%d\n",cnt);
        return n;
}
float func(int check, int n)
{
        if(check== ((1<<7)-1)) {cntfunc[0]++;return memo[n][check] = map[n][0];}

	if(memo[n][check] !=0) {cntfunc[1]++;return memo[n][check];}
	
	float dis = 99999999, tmp;

        for(int i = 1; i < 7; i++){
                if(check & (1<<i))continue;
                tmp = func(check|(1<<i), i) + map[n][i];
		if(dis > tmp) dis = tmp;
                cnt0++;
        }
        cntfunc[2]++;
        return memo[n][check] = dis;
}

void travel(int check, int n, float val)
{
	if(check== ((1<<7)-1)){cnttravel[0]++;return;}
	for(int i = 0 ; i < 7; i++){
		if(check&(1<<i)){cnt1++;continue;}
		if(val - map[n][i] == memo[i][check|(1<<i)]){
			travel(check|(1<<i), i, memo[i][check|(1<<i)]);
			cnttravel[1]++;
                        return;
		}
	}
        cnttravel[2]++;
}

int main()
{
        int i, j, dx, dy, city[7][2] = {0,0,8,6,2,4,6,7,1,3,9,4,2,3};
        float ans;
        for( i = 0 ; i < 7; i++){
                for( j = 0 ; j < 7; j++){
                        if(i == j)
                                map[i][j] = 0;
                        else{
                                dx = city[j][0] - city[i][0];
                                dy = city[j][1] - city[i][1];
                                printf("%d %d : ", i, j);
                                map[i][j] = sqrt(dx*dx + dy*dy);
                        }
                }
        }
        ans = func(1, 0);
	printf("1 ");
	travel(1,0,ans);
	printf("1\n");
	printf("%f", ans);
        printf("\n%d %d %d\n%d %d %d", cntfunc[0], cntfunc[1], cntfunc[2], cnttravel[0], cnttravel[1], cnttravel[2]);
        printf("\n%d %d",cnt0, cnt1);
}                                                                                                             

