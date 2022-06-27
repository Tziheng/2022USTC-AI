import numpy as np
from cvxopt import matrix, solvers
# import cvxpy as cp
from utils import *

solvers.options['show_progress'] = False    # 关闭cvxopt求解过程

class SupportVectorMachine:
    def __init__(self, C=1, kernel='Gauss', epsilon=1e-4):
        self.C = C
        self.epsilon = epsilon
        self.kernel = kernel

        # Hint: 你可以在训练后保存这些参数用于预测
        # SV即Support Vector，表示支持向量，SV_alpha为优化问题解出的alpha值，
        # SV_label表示支持向量样本的标签。
        self.SV = []
        self.SV_alpha = []
        self.SV_label = []

    def KERNEL(self, x1, x2, d=2, sigma=1):
        #d for Poly, sigma for Gauss
        if self.kernel == 'Gauss':
            K = np.exp(-(np.sum((x1 - x2) ** 2)) / (2 * sigma ** 2))
        elif self.kernel == 'Linear':
            K = np.dot(x1,x2)
        elif self.kernel == 'Poly':
            K = (np.dot(x1,x2) + 1) ** d
        else:
            raise NotImplementedError()
        return K
    
    def fit(self, train_features:np.ndarray, train_labels:np.ndarray):
        m = len(train_features)
        # 二次型规划
        # minimize (1/2)*x'*P*x + q'*x subject to G*x <= h,A*x = b
        P = np.array([[train_labels[i]*train_labels[j]*self.KERNEL(train_features[i],train_features[j]) for j in range(m)] for i in range(m)])
        q = np.array([-1]*m)
        G = np.array(list(-1*np.eye(m)) + list(np.eye(m)))
        h = np.array([0]*m + [self.C]*m) 
        A = np.array([train_labels])
        b = np.array([0])
        # cvxopt求解二次型规划
        Pc = matrix(P,tc='d')
        qc = matrix(q,tc='d')
        Gc = matrix(G,tc='d')
        hc = matrix(h,tc='d')
        Ac = matrix(A,tc='d')
        bc = matrix(b,tc='d')
        sol = solvers.qp(Pc,qc,Gc,hc,Ac,bc)
        a = sol['x']
    
        # cvxpy求解代码, 该问题不满足DCP约束，无法用cvxpy求解
        # x = cp.Variable(m)
        # obj = cp.Minimize(0.5*x.T@P@x + q@x)        
        # constraints = [G@x <= h, A@x == b]
        # prob = cp.Problem(obj,constraints)
        # prob.solve( )
        # a = x.value

        for i in range(m): # a[i]不为零，则是支持向量
            if a[i] >= self.epsilon:   
                self.SV.append(train_features[i])
                self.SV_alpha.append(a[i])
                self.SV_label.append(train_labels[i])

        self.b = np.average([train_labels[j] - np.sum([a[i]*train_labels[i]*self.KERNEL(train_features[i],train_features[j]) for i in range(m) ]) for j in range(m) if 0 <= a[j] <= self.C])
        
    def predict1(self, test_data):  # 预测一个
        t = np.sum([self.SV_alpha[i] * self.SV_label[i]* self.KERNEL(test_data,self.SV[i]) for i in range(len(self.SV))]) + self.b
        if t > 0:
            return 1
        return -1

    def predict(self, test_data) -> np.ndarray:
        return np.array([self.predict1(i) for i in test_data])

if __name__=='__main__':

    train_features, train_labels, test_features, test_labels = load_svm_dataset()
    n = 137
    model = SupportVectorMachine(kernel='Gauss')
    model.fit(train_features[0:n], train_labels[0:n])
