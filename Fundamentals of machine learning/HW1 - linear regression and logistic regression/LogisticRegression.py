import numpy as np

class LogisticRegression:
    def __init__(self, num_features):
        self.num_features = num_features
        self.W = np.zeros((self.num_features, 1))

    def train(self, x, y, epochs, batch_size, lr, optim):
        final_loss = None   # loss of final epoch

        # Train should be done for 'epochs' times with minibatch size of 'batch size'
        # The function 'train' should return the loss of final epoch
        # Loss of an epoch is calculated as an average of minibatch losses

        # ========================= EDIT HERE ========================
        step = 0
        final_loss = 0
        for n in range(epochs):
            if len(x)%batch_size ==0:
                iteration = int(len(x)/batch_size)
            else:
                iteration = int(len(x) / batch_size) + 1
            for iter in range(iteration):
                if iter == iteration - 1:
                    size = len(x) - batch_size * iter
                else:
                    size = batch_size
                grad = np.zeros((self.num_features, 1))
                for i in range(step, step+size):
                    h = self._sigmoid(np.dot(x[i], self.W))
                    for j in range(self.num_features):
                        grad[j] += (h - y[i]) * (x[i][j])
                self.W = optim.update(self.W, grad, lr)
                if n == epochs-1:
                    loss = 0
                    for i in range(step, step+size):
                        h = self._sigmoid(np.dot(x[i], self.W))
                        if y[i]==1:
                            loss += y[i] * h
                        else:
                            loss += np.log(1-h)
                    final_loss -= loss
        final_loss /= iteration
        # ============================================================
        return final_loss

    def eval(self, x):
        threshold = 0.5
        pred = None

        # Evaluation Function
        # Given the input 'x', the function should return prediction for 'x'
        # The model predicts the label as 1 if the probability is greater or equal to 'threshold'
        # Otherwise, it predicts as 0

        # ========================= EDIT HERE ========================
        y = np.zeros((len(x), 1))
        for i in range(len(x)):
            h = self._sigmoid(np.dot(x[i], self.W))
            if h>= threshold:
                y[i] = 1
            else:
                y[i] = 0
        pred = y
        # ============================================================

        return pred

    def _sigmoid(self, x):
        sigmoid = None

        # Sigmoid Function
        # The function returns the sigmoid of 'x'

        # ========================= EDIT HERE ========================
        sigmoid = 1 / (1 + np.exp(-1 * x))
        # ============================================================
        return sigmoid