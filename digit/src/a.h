#include <stdio.h>
#include <stdlib.h>
// 定义 INT_MAX (Ubuntu下INT_MAX是没有定义的，windows下mingw是定义的，这里用 ifndef 防止重定义)
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif
typedef struct Tree{    // A*搜索采用树结构 保存搜索路径
    int gn;             // 起点到此位置的 实际耗散
    int hn;             // 此位置到终点的 预估耗散
    int fn;             // 预估总耗散：A*中 gn+hn，IDA*中作为next_limit
    int status[5][5];   // 该节点状态
    struct Tree *child[4];// 孩子(最多4个，因为只有上下左右四个状态转移函数，每个转移函数对应一个孩子)
    int n;              // 孩子数目
    struct Tree *father;// 父节点
    int action;         // 转移函数， 父节点通过 shift_func[action] 得到此节点
    struct Tree *tminfn;// 以该节点为根的子树中fn最小的节点，minfn()函数中使用与维护
    // 在IDA*中  n, tminfn，child中的3个 不会用到 
}Tree;
// 初始化一个树，根节点由初始状态数组 start[5][5] 生成
Tree* initA_star(int start[5][5],int gn, int hn);
// 返回以t为根的子树中fn最小的节点
Tree* minfn(Tree* t);
// 按照转移函数，扩展节点t
void extendTree(Tree* t,int target[5][5], int h(int start[5][5], int target[5][5]));
// 输出转移步骤
void prt(Tree *tmp);

void A_star(int start[5][5], int target[5][5],int h(int start[5][5], int target[5][5])){
    Tree* root = initA_star(start,0,h(start,target)),*tmp;
    while ((tmp = minfn(root))->hn)     // while (tmp is not 终点)
        extendTree(tmp,target,h);       // 扩展最小fn的叶节点
    prt(tmp);
}

// IDA* 的搜索迭代函数
Tree* search(Tree* t, int bound,int target[5][5],int h(int start[5][5], int target[5][5]));

void IDA_star(int start[5][5], int target[5][5],int h(int start[5][5], int target[5][5])){
    Tree* root = initA_star(start,0,h(start,target)),*tmp;
    int bound = root->fn;
    while ((tmp = search(root,bound,target,h))->hn){     // while(tmp is not 终点)  tmp是在bound限制下的迭代深度搜索
        bound = tmp->fn;                   // 放松限制
        if(bound >= 100000){
            printf("failed");
            return;
        }
    }
    prt(tmp);
}

Tree* initA_star(int start[5][5],int gn, int hn){
    Tree* t = (Tree*)malloc(sizeof(Tree));
    if(!t) exit(printf("initA_star分配空间失败"));
    t->gn = gn;
    t->hn = hn;
    t->fn = gn + hn;
    t->n = 0;
    t->father = NULL;
    t->action = -1;
    minfn(t);
    for(int i = 0; i < 5; ++i) 
        for(int j = 0; j < 5; ++j)
            t->status[i][j] = start[i][j];
    return t;
}
Tree* minfn(Tree* t){ // 更新祖宗节点的最小fn,返回以t为根的子树的最小fn的叶节点
    if(!t) return NULL; // t是NULL
    if(t->n) return t->tminfn; // t->n 不为0，说明非叶子节点，直接返回 t->tminfn
    // t是叶子节点，下面是做维护
    t->tminfn = t;
    for(Tree *tmp = t; tmp; tmp = tmp->father) // 从叶子到根结点，依次维护
        if(t->fn < tmp->tminfn->fn)
            tmp->tminfn = t;
        else if (tmp->tminfn->n){               // 此结点的tminfn不再是叶子，则从此结点孩子里的tminfn选取一个作为tminfn
            tmp->tminfn = t;
            for(int i = 0; i < tmp->n; ++i)
                if(tmp->tminfn->fn > tmp->child[i]->tminfn->fn)
                    tmp->tminfn = tmp->child[i]->tminfn;
        }
    return t->tminfn;
}
void extendTree(Tree* t,int target[5][5], int h(int start[5][5], int target[5][5])){  // 延展节点t
    extern int ((*shift_func[4])(int start[5][5],int status[5][5]));
    for(int i = 0; i < 4; ++i){
        // 去除 A->B->A 这种转移 (仅这一步能够减少非常非常多的重复状态，时间减少两到三倍)
        if(i == 0 && t->action == 1) continue;
        if(i == 1 && t->action == 0) continue;
        if(i == 2 && t->action == 3) continue;
        if(i == 3 && t->action == 2) continue;

        Tree * tmp = (Tree*)malloc(sizeof(Tree));
        if(!tmp) exit(printf("extendTree分配空间失败"));
        if(shift_func[i](t->status,tmp->status)){   // 可通过 转移函数i 转移
            t->child[t->n++] = tmp;
            tmp->gn = t->gn + 1;
            tmp->hn = h(tmp->status,target);
            tmp->fn = tmp->gn + tmp->hn;
            tmp->n = 0;
            tmp->father = t;
            tmp->action = i;
            minfn(tmp);     // tmp是叶子结点，这里调用minfn()是起维护作用，详细请看该函数
        }
        else
            free(tmp);
    }
}
void prt(Tree *tmp){        // 输出路径
    int actions[1000], i = 0, steps = tmp->gn;
    while (i < steps){
        actions[steps - i - 1] = tmp->action;
        tmp = tmp->father;
        i++;
    }
    for(i = 0; i < steps ; ++i)
    switch (actions[i]){
    case 0:printf("U");break;
    case 1:printf("D");break;
    case 2:printf("L");break;
    case 3:printf("R");break;
    default:break;
    }
}


// IDA*用
Tree* search(Tree* t, int bound,int target[5][5],int h(int start[5][5], int target[5][5])){
    extern int ((*shift_func[4])(int start[5][5],int status[5][5]));
    if(t->fn > bound) return t;
    if(!t->hn) return t;
    int min = INT_MAX;
    for(int i = 0; i < 4; ++i){
        // 去除 A->B->A 这种转移 (仅这一步能够减少非常非常多的重复状态，时间减少两到三倍)
        if(i == 0 && t->action == 1) continue;
        if(i == 1 && t->action == 0) continue;
        if(i == 2 && t->action == 3) continue;
        if(i == 3 && t->action == 2) continue;

        Tree * tmp = (Tree*)malloc(sizeof(Tree));
        if(!tmp) exit(printf("extendTree分配空间失败"));
        if(shift_func[i](t->status,tmp->status)){   // 可通过 转移函数i 转移
            t->child[i] = tmp;
            tmp->gn = t->gn + 1;
            tmp->hn = h(tmp->status,target);
            tmp->fn = tmp->gn + tmp->hn;
            tmp->father = t;
            tmp->action = i;
            tmp = search(tmp,bound,target,h);   // ***迭代深搜***
            if(!tmp->hn) return tmp;            // 找到终点
            if(tmp->fn < min) min = tmp->fn;
            free(t->child[i]);
        }
        else
            free(tmp);
    }
    t->fn = min;    // 存储下一个limit限制，这里虽然打破了fn=gn+hn的规则，但赋值给fn没有问题，因为fn本意是对耗散的总估值，gn+hn只是其中一种表示，
    return t;
}