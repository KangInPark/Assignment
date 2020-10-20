#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define n1_max 15000000
#define n2_max 150000000

int n1[n1_max];
int n2[n2_max];

int input[3];
int n1_num;
int n2_num;

int answer = 0; //final common


// Find common part of two array and reserve at global variable 'answer' 
void find_common(){
    unsigned int n, th, cnt = 0, right, mid, i, prev=0, diff=0;
    if(n1_num<n2_num){
        n = n1_num;
        th = (unsigned int)(log((double)n2_num)/log(2.0));
        diff = th + 1;
        for(i = 0 ; i< n ; i++){
            while(n2[cnt] < n1[i] && cnt<n2_num){
                if(diff > th){
                    right = n2_num -1;
                    while(cnt <= right){
                        mid = (cnt + right) >> 1;
                        if(n2[mid] > n1[i])right = mid-1;
                        else if(n2[mid] < n1[i])cnt = mid + 1;
                        else break;
                    }
                    cnt = mid;	
                    if(n1[n-1] < n2[cnt])return;
                    break;
                }
                cnt++;
            }
            if(n1[i] == n2[cnt])answer++;
            diff = (diff>>1) + ((cnt-prev)>>1);
            prev=cnt;
        }
    }
    else{
        n = n2_num;
        th = (unsigned int)(log((double)n1_num)/log(2.0));
        diff = th + 1;
        for(i = 0 ; i< n ; i++){
            while(n1[cnt] < n2[i] && cnt<n1_num){
                if(diff > th){
                    right = n1_num -1;
                    while(cnt <= right){
                        mid = (cnt + right) >> 1;
                        if(n1[mid] > n2[i])right = mid-1;
                        else if(n1[mid] < n2[i])cnt = mid + 1;
                        else break;
                    }
                    cnt = mid;
                    if(n2[n-1] < n1[cnt])return;
                    break;
                }
                cnt++;
            }
            if(n2[i] == n1[cnt])answer++;
            diff = (diff>>1) + ((cnt-prev)>>1);
            prev=cnt;
        }
    }
}

int main()
{

FILE *fp =  fopen("sample1_answer.txt", "r");
FILE *fp2 = fopen("sample1_n2.txt", "r");
FILE *fp3 = fopen("sample1_n1.txt", "r");


for(int i =0; i<3; i++){
        fscanf(fp, "%d", &input[i]);
}

fclose(fp);

n2_num = input[0];
n1_num = input[1];
int common = input[2]; //Correct Answer!

for(int i =0; i<n2_num; i++){
        fscanf(fp2, "%d", &n2[i]);
}

for(int i =0; i<n1_num; i++){
        fscanf(fp3, "%d", &n1[i]);
}


fclose(fp2);
fclose(fp3);

find_common();

printf("Real_Common : %d\n",common ); //Real answer
printf("Calculated_Common : %d\n",answer ); //Your guess


return 0;

}
