from utils import *
from DecisionTree import DecisionTree


def test_decisiontree():
    train_features, train_labels, test_features, test_labels = load_decisiontree_dataset()
    model = DecisionTree()
    model.fit(train_features, train_labels)
    results = model.predict(test_features)
    # results = np.random.randint(2, size=56)
    print('DecisionTree acc: {:.2f}%'.format(get_acc(results, test_labels) * 100))

if __name__=='__main__':
    test_decisiontree() 

