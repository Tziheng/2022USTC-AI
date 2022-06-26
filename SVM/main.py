from utils import *
from SVM import SupportVectorMachine


def test_svm(C=1, kernel='Linear', epsilon = 1e-4):
    train_features, train_labels, test_features, test_labels = load_svm_dataset()
    model = SupportVectorMachine(C, kernel, epsilon)
    model.fit(train_features, train_labels)
    pred = model.predict(test_features)
    print('SVM({} kernel) acc: {:.2f}%'.format(kernel, get_acc(pred, test_labels.reshape(-1,)) * 100))
    

if __name__=='__main__':
    test_svm(1, 'Linear', 1e-4)
    test_svm(1, 'Poly', 1e-4)
    test_svm(1, 'Gauss', 1e-4)    

