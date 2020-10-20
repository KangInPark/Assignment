import numpy as np

class LinearRegression:
    def __init__(self, num_features):
        self.num_features = num_features
        self.W = np.zeros((self.num_features, 1))

    def train(self, x, y, epochs, batch_size, lr, optim):
        final_loss = None   # loss of final epoch

        # Training should be done for 'epochs' times with minibatch size of 'batch_size'
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
                    for j in range(self.num_features):
                        grad[j] += (np.dot(x[i], self.W) - y[i]) * (x[i][j])
                for j in range(self.num_features):
                    grad[j] /= size
                self.W = optim.update(self.W, grad, lr)
                if n == epochs-1:
                    loss = 0
                    for i in range(step, step+size):
                        loss += np.square(np.dot(x[i], self.W) - y[i])
                    loss /= (2 * size)
                    final_loss += loss
        final_loss /= iteration
        # ============================================================
        return final_loss

    def eval(self, x):
        pred = None

        # Evaluation Function
        # Given the input 'x', the function should return prediction for 'x'

        # ========================= EDIT HERE ========================
        y = np.zeros((len(x),1))
        for i in range(len(x)):
            y[i] = np.dot(x[i], self.W)
        pred = y
        # ============================================================
        return pred
