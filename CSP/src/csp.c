#include <stdio.h>
// 工人最大数目
#define MAXNUM 10
// MRVstatus=1 开启MRV优化 MRVstatus=0，关闭MRV
#define MRVstatus 1

int dispatch[MAXNUM][7] = {0};      // dispatch[0][3]==1 表示工人0在周四值日
int range[MAXNUM][7][2] = {0};      // range[id][day] 是dispatch[id][day] 的值域
int num;                            // 工人数

int senior[MAXNUM];                 // senior[n]=id    表示id工人是第n个senior
int numsen;                         // senior人数
int AB[MAXNUM][MAXNUM] = {0};       // AB工人工作时间冲突

int x;  // 需要每天安排至少 x 个人值班
int cnt = 0;    // 每调用一次dfs，cnt+1，用于记录迭代次数

/*
1. 每个工人每周必须休息2天或2天以上
2. 每个工人每周不可以连续休息3天(不考虑跨周情况)
3. 周六周天也需要有人值班，即一周7天每天都要安排工人值班
4. 需要每天安排至少 x 个人值班
5. 每天至少要有一名级别为 senior 的工人值班
6. 工人a不想与工人b同一天工作
*/

int dfs(int k);
int prt();

int main(int argc, char* argv[]){
    
    FILE * fp;
    int a,b;

    if(!(fp = fopen(argv[1],"r"))) return printf("文件不存在");
    fscanf(fp,"num:%d,x:%d,num of senior:%d\n",&num,&x,&numsen);

    // 记录senior
    for(int i = 0; i < numsen; senior[i++]--) fscanf(fp,"%d",senior+i);
    
    // 记录员工工作时间冲突
    while (fscanf(fp,"%d %d",&a,&b) == 2)
        AB[a-1][b-1] = AB[b-1][a-1] = 1;
    
    fclose(fp);

    if(dfs(0))
        prt();
    else
        printf("failed\n");
    return 0;
}

int check1(int id,int day);
int check2(int id,int day);
int check3(int id,int day);
int check4(int id,int day);
int check5(int id,int day);
int check6(int id,int day);

// 按天排班. dfs(id,day) 说明0,1,...,day-1都已经排好
int dfs(int k){
    cnt++;
    int id = k % num, day = k / num;
    if(day == 7) 
        return 1;

    for(int color = 0;color < 2; color++){  // 给格子上色（这里只有两种颜色，代表值日与休息）
        dispatch[id][day] = color;
        if(check1(id,day) || check2(id,day) || check4(id,day) || check5(id,day) || check6(id,day))
            continue;
        // 无冲突，下面进行dfs迭代下一个格子
        if(dfs(k+1)) 
            return 1;
    }
    
    return 0;
}

// 1. 每个工人每周必须休息2天或2天以上
// id工人工作大于5天返回1，否则返回0
int check1(int id,int day){
    int s = 0;
    for(int i = 0; i <= day; ++i)
        if(dispatch[id][i])
            s++;
    if(s > 5)
        return 1;

    return 0;
}

// 2. 每个工人每周不可以连续休息3天(不考虑跨周情况)
// id工人在day之前有3天连休返回1，没有连休返回0
int check2(int id,int day){
    if(day <= 1) return 0;
    if(                                                                                   \
    (day >= 2 && dispatch[id][0] == 0 && dispatch[id][1] == 0 && dispatch[id][2] == 0) || \
    (day >= 3 && dispatch[id][1] == 0 && dispatch[id][2] == 0 && dispatch[id][3] == 0) || \
    (day >= 4 && dispatch[id][2] == 0 && dispatch[id][3] == 0 && dispatch[id][4] == 0) || \
    (day >= 5 && dispatch[id][3] == 0 && dispatch[id][4] == 0 && dispatch[id][5] == 0) || \
    (day >= 6 && dispatch[id][4] == 0 && dispatch[id][5] == 0 && dispatch[id][6] == 0)    )
            return 1;

    return 0;
} 

// 3. 周六周天也需要有人值班，即一周7天每天都要安排工人值班

// 4. 需要每天安排至少 x 个人值班
// day天不足x个人值班返回1，否则返回0
int check4(int id, int day){
    int s = 0; 
    for(int i = 0; i <= id; ++i)
        if(dispatch[i][day])
            s++;
    if(s + num - 1 - id < x)    // 该天不足x人
        return 1;

    return 0;
}

// 5. 每天至少要有一名级别为 senior 的工人值班
// 所有senior不值日返回1，否则返回0
int check5(int id,int day){
    int s = 0;
    for(int i = 0; i < numsen; ++i)
        if(senior[i] > id) 
            return 0;
        else if(dispatch[senior[i]][day])
                s++;
    if(s == 0)
        return 1;
    return 0;
}

// 6. 工人a不想与工人b同一天工作
// 工人id有冲突返回1，否则返回0
int check6(int id,int day){
    if(!dispatch[id][day]) return 0; // 工人id没排班，不存在冲突
    for(int i = 0; i < id; ++i)
        if(dispatch[i][day] && AB[i][id])
            return 1;

    return 0;
}

int prt(){  // 输出
    for(int i = 0; i < 7; ++i,printf("\n"))
        for(int j = 0; j < num; ++j)
            if(dispatch[j][i])
                printf("%d ",j+1);
    return 0;
}

