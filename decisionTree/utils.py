import numpy as np
def get_acc(y, pred):
    return np.sum(y == pred) / len(y)


def load_decisiontree_dataset():
    train_data = np.loadtxt('dataset/dt/dt_train.data', delimiter=',', dtype='int64')
    test_data = np.loadtxt('dataset/dt/dt_test.data', delimiter=',', dtype='int64')
    train_features = train_data[:,1:]
    train_labels = train_data[:,0]
    test_features = test_data[:,1:]
    test_labels = test_data[:,0]

    return train_features, train_labels, test_features, test_labels