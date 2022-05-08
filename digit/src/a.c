#include "a.h"
#include <stdio.h>
#include <string.h>
// rd(x,y,i,j) 计算(x,y)到(i,j)的路程
#define rd(x,y,i,j) ((x > i ? x - i : i - x) + (y > j ? y - j : j - y))

// 算法 
void A_h1(int start[5][5], int target[5][5]);
void A_h2(int start[5][5], int target[5][5]);
void IDA_h1(int start[5][5], int target[5][5]);
void IDA_h2(int start[5][5], int target[5][5]);
void (* func)(int start[5][5], int target[5][5]);
// 启发函数
int h1(int start[5][5], int target[5][5]);
int h2(int start[5][5], int target[5][5]);
// 四个转移函数，shift_func存储该4个转移函数
int up(int start[5][5],int status[5][5]);
int down(int start[5][5],int status[5][5]);
int left(int start[5][5],int status[5][5]);
int  right(int start[5][5],int status[5][5]);

int ((*shift_func[4])(int start[5][5],int status[5][5])) = {up,down,left,right};  // 4个转移函数

int main(int argc, char* argv[]){
    if(argc != 4) return printf("输入参数个数有误！\n");

    // 选择算法
    if(!strcmp(argv[1], "A_h1"))
        func = A_h1;
    else if(!strcmp(argv[1], "A_h2"))
        func = A_h2;
    else if(!strcmp(argv[1], "IDA_h1"))
        func = IDA_h1;
    else if(!strcmp(argv[1], "IDA_h2"))
        func = IDA_h2;
    else return printf("输入了未知参数\n");
    
    int start[5][5], target[5][5]; 
    // 读取初始状态与最终状态
    FILE * fp;
    if(!(fp = fopen(argv[2],"r"))) return printf("没有这个文件：%s\n",argv[2]);
    for(int i = 0; i < 5; ++i) fscanf(fp,"%d %d %d %d %d\n", start[i], start[i]+1, start[i]+2, start[i]+3, start[i]+4);
    fclose(fp);
    if(!(fp = fopen(argv[3],"r"))) return printf("没有这个文件：%s\n",argv[3]);
    for(int i = 0; i < 5; ++i) fscanf(fp,"%d %d %d %d %d\n",target[i],target[i]+1,target[i]+2,target[i]+3,target[i]+4);
    fclose(fp);

    func(start,target);
    return 0;
}
// A_star IDA_star 在 a.h 完成
void A_h1(int start[5][5], int target[5][5]){
    A_star(start,target,h1);
}
void A_h2(int start[5][5], int target[5][5]){
    A_star(start,target,h2);
}
void IDA_h1(int start[5][5], int target[5][5]){
    IDA_star(start,target,h1);
}
void IDA_h2(int start[5][5], int target[5][5]){
    IDA_star(start,target,h2);
}
int h1(int start[5][5], int target[5][5]){  // 返回错位的星球个数
    int s = 0;
    for(int i = 0; i < 5; ++i)
        for(int j = 0; j < 5; ++j)
            if(start[i][j] != target[i][j] && target[i][j])
                if(target[i][j] < 0) 
                    exit(printf("黑洞错位"));
                else 
                    s++;
    return s;
}
int h2(int start[5][5], int target[5][5]){  // 自定义的耗散函数
    int s = 0;
    int r[5],minr;// r[5] 存储了5种路线   
    for(int i = 0; i < 5; ++i)
        for(int j = 0; j < 5; ++j)
            if(start[i][j] != target[i][j] && start[i][j]){
                if(target[i][j] < 0) 
                    exit(printf("黑洞错位"));
                int x = 0, y = 0;
                while (start[i][j] != target[x][y]) // 找到target中此星球的位置
                    y == 4 ? (x++, y = 0) : y++;
                minr = r[0] = rd(x,y,i,j);   // 不通过星际航道的路程
                r[1] = rd(x,y,0,2) + rd(4,2,i,j) + 1;
                r[2] = rd(x,y,4,2) + rd(0,2,i,j) + 1;
                r[3] = rd(x,y,2,0) + rd(2,4,i,j) + 1;
                r[4] = rd(x,y,2,4) + rd(2,0,i,j) + 1;
                for(int i = 1; i < 5; ++i)  // 找出 r[5] 最小值
                    if(minr > r[i])  minr = r[i];
                s += minr;
            }
    return s;
}
int  up(int start[5][5],int status[5][5]){ // 输入初始状态，返回转移后的状态status，转移失败返回0，转移成功返回1
    int x,y;
    for(int i = 0; i < 5; ++i)
        for(int j = 0; j < 5; ++j)
            if(!(status[i][j] = start[i][j])){
                x = i;  y = j;      // 找到飞船
            }
    if(x == 0 && y != 2) return 0;  // 这几个位置不能使用up转移，转移失败，返回0
    if(start[x?x-1:4][y] < 0) return 0; // 遇见黑洞，不能转移，返回0
    status[x?x-1:4][y] = start[x][y];
    status[x][y] = start[x?x-1:4][y];
    return 1;
}
int  down(int start[5][5],int status[5][5]){
    int x,y;
    for(int i = 0; i < 5; ++i)
        for(int j = 0; j < 5; ++j)
            if(!(status[i][j] = start[i][j])){
                x = i;  y = j;
            }
    if(x == 4 && y != 2) return 0;  // 这几个位置不能使用up转移
    if(start[x!=4?x+1:0][y] < 0) return 0; // 遇见黑洞，不能转移
    status[x!=4?x+1:0][y] = start[x][y];
    status[x][y] = start[x!=4?x+1:0][y];
    return 1;
}
int  left(int start[5][5],int status[5][5]){
    int x,y;
    for(int i = 0; i < 5; ++i)
        for(int j = 0; j < 5; ++j)
            if(!(status[i][j] = start[i][j])){
                x = i; y = j;
            }
    if(y == 0 && x != 2) return 0;  // 这几个位置不能使用up转移
    if(start[x][y?y-1:4] < 0) return 0; // 遇见黑洞，不能转移
    status[x][y?y-1:4] = start[x][y];
    status[x][y] = start[x][y?y-1:4];
    return 1;
}
int  right(int start[5][5],int status[5][5]){
    int x,y;
    for(int i = 0; i < 5; ++i)
        for(int j = 0; j < 5; ++j)
            if(!(status[i][j] = start[i][j])){
                x = i; y = j;
            }
    if(y == 4 && x != 2) return 0;  // 这几个位置不能使用up转移
    if(start[x][y!=4?y+1:0] < 0) return 0; // 遇见黑洞，不能转移
    status[x][y!=4?y+1:0] = start[x][y];
    status[x][y] = start[x][y!=4?y+1:0];
    return 1;
}