from math import log2
import numpy as np

def entropy(Pr):
    if max(Pr) < 1:
        return np.sum(-p*log2(p) for p in Pr)
    else:
        return 0

def Bool(p):
    return entropy([p,1-p])

def Remainder(A,train_features,train_labels): # 对样本求经过A划分后的剩余熵，返回剩余熵
    rangeA = set([i[A] for i in train_features])
    divide = [ [ [train_features[i],train_labels[i]]  for i in range(len(train_features)) if train_features[i][A] == l] for l in rangeA ]
    pk = [np.sum([0] + [1 for l in d if l[1] == 1]) for d in divide]
    nk = [len(divide[i]) - pk[i] for i in range(len(divide))]
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
        if len(A) == 0 or len(set(train_labels)) <= 1 or len(train_features) <= 1:  
            node.label = np.argmax(np.bincount(train_labels))  
            return node
        av = A[np.argmin([Remainder(i,train_features,train_labels) for i in A])]
        range_av = set([i[av] for i in train_features])
        for l in range_av:
            Dvfeatures = [ feat for feat in train_features if feat[av] == l] 
            Dvlabels   = [ train_labels[i] for i in range(len(train_labels)) if train_features[i][av] == l]
            node.child.append([ l, self.TreeGenerate(Dvfeatures,Dvlabels,[a for a in A if a != av])])
        
        node.isleaf = False 
        node.attr = av     
        return node

    def fit(self, train_features, train_labels):
        self.root = self.TreeGenerate(train_features,train_labels,list(set(train_labels)))

    def predict1(self, features):   # 预测一个
        node = self.root
        while node.isleaf == False:
            av = features[node.attr]
            for i in node.child:
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


