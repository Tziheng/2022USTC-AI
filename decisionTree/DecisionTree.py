from math import log2
import numpy as np

# 信息熵
# 输入：随机变量的分布
# 返回：该随机变量的信息熵
def entropy(Pr):
    if max(Pr) < 1:
        return np.sum(-p*log2(p) for p in Pr)
    else:
        return 0

def Bool(p):
    return entropy([p,1-p])

def Remainder(A,train_features,train_labels): # 对样本求经过A划分后的剩余熵，返回剩余熵
    # A的值域
    rangeA = set([i[A] for i in train_features])
    # 按照属性A划分样本
    divide = [ [ [train_features[i],train_labels[i]]  for i in range(len(train_features)) if train_features[i][A] == l] for l in rangeA ]
    # 各组正例个数
    pk = [np.sum([0] + [1 for l in d if l[1] == 1]) for d in divide]
    # 各组反例个数
    nk = [len(divide[i]) - pk[i] for i in range(len(divide))]
    # 正反例总数
    p , n = np.sum(pk) , np.sum(nk)
    return np.sum([ (pk[i] + nk[i])*Bool(pk[i]/(pk[i] + nk[i])) for i in range(len(divide))] )/(p + n)


class Treenode:
    def __init__(self) -> None:
        self.child = []
        self.attr = None
        self.isleaf = True
        self.label = None

class DecisionTree:
    def TreeGenerate(self,train_features,train_labels,A):
        node = Treenode()
        if len(A) == 0 or len(set(train_labels)) <= 1 or len(train_features) <= 1:   # label均相同，或无可划分的属性集
            node.label = np.argmax(np.bincount(train_labels))   # 返回 train_labels 的众数
            return node
        # 选择期望剩余熵最小的作为最优划分
        av = A[np.argmin([Remainder(i,train_features,train_labels) for i in A])]
        # 属性a的值域
        range_av = set([i[av] for i in train_features])
        
        for l in range_av:
            Dvfeatures = [ feat for feat in train_features if feat[av] == l] 
            Dvlabels   = [ train_labels[i] for i in range(len(train_labels)) if train_features[i][av] == l]
            # Dv不会是空集
            node.child.append([ l, self.TreeGenerate(Dvfeatures,Dvlabels,[a for a in A if a != av])])
        
        node.isleaf = False # 改为不是叶子
        node.attr = av      # 该节点是依据attr划分的
        return node

    def fit(self, train_features, train_labels):
        self.root = self.TreeGenerate(train_features,train_labels,list(set(train_labels)))

    def predict1(self, features):   # 预测一个
        node = self.root
        while node.isleaf == False:
            av = features[node.attr]
            for i in node.child:    # 找到相应的孩子
                if i[0] == av:
                    node = i[1]
                    break
        return node.label

    def predict(self,test_features) -> np.ndarray:
        return np.array([self.predict1(feat) for feat in test_features ])


if __name__=='__main__':
    # 测试熵
    print(entropy([0.5]*2))     # 2面骰子    
    print(entropy([0.25]*4))    # 4面骰子
    print(Bool(0.137))    
    print(Bool(0.19))    


