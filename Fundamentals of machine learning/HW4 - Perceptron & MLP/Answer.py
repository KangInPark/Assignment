import numpy as np
np.random.seed(428)

"""
*** This is where you write your answers ***

- Write your answers ONLY in here. We will grade ONLY with this file.
- ***** Write your answers in 'EDIT HERE' part ONLY!!!!! *****

- Read instructions carefully and write your answers.

- Please do not code any other functionality other than what is said to do. 
  You will not get the same results, otherwise. (Ask through e-mail or icampus note)
  ex) shuffle data every epoch (You don't need to care this. But you can try personally.)
  ex) set/change random seed (I've already done it.)

*** 이곳에 정답을 적어주세요 ***

- 여기에'만' 정답을 적어주세요. 이 파일만으로 과제가 채점됩니다.
- ***** 'EDIT HERE'라고 되어있는 곳에만 코드를 넣어주세요. *****

- 설명을 자세히 읽고 답을 적으세요.

- 명시되어있는 기능 이외의 다른 기능을 추가하지마세요. 
  정답 결과와 같은 결과가 나오지 않을 수 있습니다. (문의는 쪽지 혹은 이메일로 주세요)
  ex) 매 epoch에 데이터를 섞는다. (신경쓰지 않으셔도 됩니다. 물론 과제 이외에 개인적인 실험은 하셔도 좋습니다.)
  ex) Random seed를 설정한다/바꾼다. (이미 설정 해놓았습니다.)
"""

# ============================ Perceptron ==============================

def sign(z):
    """
    Sign function for Perceptron
    sign(z) = 1 if z > 0, -1 otherwise.

    [Inputs]
        z : input for sign function in any shape
    [Outputs]
        sign_z : sign value for z

    """
    sign_z = None
    # =============== EDIT HERE ===============
    bool = z > 0
    sign_z = np.where(bool,1.,-1.)
    # =========================================
    return sign_z

class Perceptron:
    def __init__(self, num_features):
        # NOTE : In this assignment, weight and bias are separated. Be careful.
        self.W = np.random.rand(num_features, 1)
        self.b = np.random.rand(1)

    def forward(self, x):
        """
        Forward path of Perceptron (single neuron).
        x -- (weight, bias) --> z -- (sign function) --> sign(z)

        [Inputs]
            x : input for perceptron. Numpy array of (N, D)
        [Outputs]
            out : output of perceptron. Numpy array of (N, 1)
        """
        out = None
        if len(x.shape) < 2:
            x = np.expand_dims(x, 0)
        # =============== EDIT HERE ===============
        tmp = np.dot(x, self.W)
        tmp += self.b
        out = sign(tmp)
        # =========================================
        return out

    def stochastic_train(self, x, y, learning_rate):
        num_data = x.shape[0]
        """
        Stochastic Training of perceptron 
        Perceptron Stochastic Training updates weights on every data (not batch)
        Training ends when there is no misclassified data.
        See Lecture Notes 'W09 Neural network basics'
        Again, Do not implement funtionalities such as shuffling data or sth. 

        [Inputs]
            x : input for perceptron. Numpy array of (N, D)
            y : label of data x. Numpy array of (N, )
            learning_rate : learning rate.
        
        [Outputs]
            None
        """
        while True:
            # Repeat until quit condition is satisfied.
            quit = True

            for i in range(num_data):
            # =============== EDIT HERE ===============
                hypo = self.forward(x[i])
                if y[i] != hypo:
                    quit = False
                    self.W = self.W + learning_rate * np.sum(np.dot(y[i], x[i]))
            # =========================================
            if quit:
                break


    def batch_train(self, x, y, learning_rate):
        num_data = x.shape[0]
        """
        Batch Training of perceptron 
        Perceptron Batch Training updates weights all at once for every data (not everytime)
        Training ends when there is no misclassified data.
        See Lecture Notes 'W09 Neural network basics'
        Again, Do not implement funtionalities such as shuffling data or sth. 
    
        [Inputs]
            x : input for perceptron. Numpy array of (N, D)
            y : label of data x. Numpy array of (N, )
            learning_rate : learning rate.
    
        [Outputs]
            None
        """
        while True:
            # gradients of W & b
            dW = np.zeros_like(self.W)
            db = np.zeros_like(self.b)

            # Repeat until quit condition is satisfied.
            quit = True
            for i in range(num_data):
            # =============== EDIT HERE ===============
                if i == 0:
                    list= []
                hypo= self.forward(x[i])
                if y[i] != hypo:
                    quit = False
                    list.append(np.sum(np.dot(y[i],x[i])))
            self.W = self.W + learning_rate * np.sum(list)
            # =========================================
            if quit:
                break

# ======================================================================



# ====================== MultiLayer Perceptron =========================

def softmax(z):
    # Numerically stable softmax. Already implemented.
    z = z - np.max(z, axis=1, keepdims=True)
    _exp = np.exp(z)
    _sum = np.sum(_exp, axis=1, keepdims=True)
    sm = _exp / _sum
    return sm

class ReLU:
    """
    ReLU Function. ReLU(x) = max(0, x)
    Implement forward & backward path of ReLU.
    """

    def __init__(self):
        # 1 (True) if ReLU input < 0
        self.zero_mask = None

    def forward(self, z):
        """
        ReLU Forward.
        ReLU(x) = max(0, x)

        z --> (ReLU) --> out

        [Inputs]
            z : ReLU input in any shape.

        [Outputs]
            out : ReLU(z).
        """
        out = None
        # =============== EDIT HERE ===============
        out = np.maximum(0, z)
        self.zero_mask = z < 0
        # =========================================
        return out

    def backward(self, d_prev):
        """
        ReLU Backward.

        z --> (ReLU) --> out
        dz <-- (dReLU) <-- d_prev(dL/dout)

        [Inputs]
            d_prev : Gradients until now.
            d_prev = dL/dk, where k = ReLU(z).

        [Outputs]
            dz : Gradients w.r.t. ReLU input z.
        """
        dz = None
        # =============== EDIT HERE ===============
        dRelu = np.where(self.zero_mask, 0 ,1)
        dz = dRelu * d_prev
        # =========================================
        return dz

class Sigmoid:
    """
    Sigmoid Function.
    Implement forward & backward path of Sigmoid.
    """
    def __init__(self):
        self.out = None

    def forward(self, z):
        """
        Sigmoid Forward.

        z --> (Sigmoid) --> self.out

        [Inputs]
            z : Sigmoid input in any shape.

        [Outputs]
            self.out : Sigmoid(z).
        """
        self.out = None
        # =============== EDIT HERE ===============
        self.out = 1 / (1 + np.exp(-1 * z))
        # =========================================
        return self.out

    def backward(self, d_prev):
        """
        Sigmoid Backward.

        z --> (Sigmoid) --> self.out
        dz <-- (dSigmoid) <-- d_prev(dL/d self.out)

        [Inputs]
            d_prev : Gradients until now.

        [Outputs]
            dz : Gradients w.r.t. Sigmoid input z.
        """
        dz = None
        # =============== EDIT HERE ===============
        dSigmoid = self.out * (1 - self.out)
        dz = d_prev * dSigmoid
        # =========================================
        return dz


"""
** Multilayer Perceptron **
[input -- (input layer) --> hidden1 -- (hidden layer) --> hidden -- (output layer) --> output]

You will implement input/hiddden/output layers.

TIP : Draw computational graph and compute gradients to code BEFOREHAND.
"""

class InputLayer:
    """
    Input Layer
    input -- (input layer) --> hidden

    Implement forward & backward path.
    """
    def __init__(self, num_features, num_hidden_1, activation):
        # Weights and bias
        self.W = np.random.rand(num_features, num_hidden_1)
        self.b = np.zeros(num_hidden_1)
        # Gradient of Weights and bias
        self.dW = None
        self.db = None
        # Forward input
        self.x = None
        # Activation function (Sigmoid or ReLU)
        self.act = activation()

    def forward(self, x):
        """
        Input layer forward
        - Feed forward
        - Apply activation function you implemented above.

        [Inputs]
           x : Input data (N, D)

        [Outputs]
            self.out : Output of input layer. Hidden. (N, H)
        """
        self.x = None
        self.out = None
        # =============== EDIT HERE ===============
        self.x = x
        self.out = np.dot(self.x , self.W) + self.b
        self.out = self.act.forward(self.out)
        # =========================================
        return self.out

    def backward(self, d_prev):
        """
        Input layer backward
        x and (W & b) --> z -- (activation) --> hidden
        dx and (dW & db) <-- dz <-- (activation) <-- hidden

        - Backward of activation
        - Gradients of W, b

        [Inputs]
            d_prev : Gradients until now.

        [Outputs]
            None
        """
        self.dW = None
        self.db = None
        # =============== EDIT HERE ===============
        dz = self.act.backward(d_prev)
        self.dW = self.x.T.dot(dz)
        self.db = np.ones_like(dz.T).dot(dz)[0]
        # =========================================

class SigmoidOutputLayer:
    def __init__(self, num_hidden_2, num_outputs):
        # Weights and bias
        self.W = np.random.rand(num_hidden_2, num_outputs)
        self.b = np.zeros(num_outputs)
        # Gradient of Weights and bias
        self.dW = None
        self.db = None
        # Input (x), label(y), prediction(y_hat)
        self.x = None
        self.y = None
        self.y_hat = None
        # Loss
        self.loss = None
        # Sigmoid function
        self.sigmoid = Sigmoid()

    def forward(self, x, y):
        """
        Sigmoid output layer forward
        - Make prediction
        - Calculate loss

        ## Already Implemented ##
        """
        self.y_hat = self.predict(x)
        self.y = y
        self.x = x

        self.loss = self.binary_ce_loss(self.y_hat, self.y)

        return self.loss

    def binary_ce_loss(self, y_hat, y):
        """
        Calcualte "Binary cross-entropy loss"
        Add 'eps' for stability inside log function.

        [Inputs]
            y_hat : Prediction
            y : Label

        [Outputs]
            loss value
        """
        eps = 1e-10
        bce_loss = None
        # =============== EDIT HERE ===============
        loss = y * np.log(y_hat + eps) + (1 - y) * np.log(1 - y_hat + eps)
        bce_loss = - np.mean(loss)
        # =========================================
        return bce_loss

    def predict(self, x):
        """
        Make prediction in probability. (Not 0 or 1 label!!)

        [Inputs]
            x : input data

        [Outputs]
            y_hat : Prediction
        """
        y_hat = None
        # =============== EDIT HERE ===============
        z = np.matmul(x, self.W) + self.b
        y_hat = self.sigmoid.forward(z)
        # =========================================
        return y_hat

    def backward(self, d_prev=1):
        """
        Calculate gradients of input (x), W, b of this layer.
        Save self.dW, self.db to update later.

        x and (W & b) --> z -- (activation) --> y_hat --> Loss
        dx and (dW & db) <-- dz <-- (activation) <-- dy_hat <-- Loss

        [Inputs]
            d_prev : Gradients until here. (Always 1 since its output layer)

        [Outputs]
            dx : Gradients of output layer input x (Not MLP input x!)
        """
        batch_size = self.y.shape[0]
        dx = None
        # =============== EDIT HERE ===============
        dz = self.sigmoid.backward(((1-self.y)/(1-self.y_hat)-self.y/self.y_hat) / self.x.shape[0] * d_prev)
        self.dW = self.x.T.dot(dz)
        self.db = np.ones_like(dz.T).dot(dz)[0]
        dx = self.W.dot(dz.T).T
        # =========================================
        return dx

class HiddenLayer:
    def __init__(self, num_hidden_1, num_hidden_2):
        # Weights and bias
        self.W = np.random.rand(num_hidden_1, num_hidden_2)
        self.b = np.zeros(num_hidden_2)
        # Gradient of Weights and bias
        self.dW = None
        self.db = None
        # ReLU function
        self.act = ReLU()

    def forward(self, x):
        """
        Hidden layer forward
        - Feed forward
        - Apply activation function you implemented above.

        [Inputs]
           x : Input data (N, D)

        [Outputs]
            self.out : Output of input layer. Hidden. (N, H)
        """
        self.x = None
        self.out = None
        # =============== EDIT HERE ===============
        self.x = x
        self.out = self.act.forward(self.x.dot(self.W)+self.b)
        # =========================================
        return self.out

    def backward(self, d_prev):
        """
        Hidden layer backward
        x and (W & b) --> z -- (activation) --> output
        dx and (dW & db) <-- dz <-- (activation) <-- output

        - Calculate gradients of input (x), W, b of this layer.
        - Save self.dW, self.db to update later.

        [Inputs]
            d_prev : Gradients until here.

        [Outputs]
            dx : Gradients of output layer input x (Not MLP input x!)
        """
        dx = None
        self.dW = None
        self.db = None
        # =============== EDIT HERE ===============
        dz = self.act.backward(d_prev)
        self.dW = self.x.T.dot(dz)
        self.db = np.ones_like(dz.T).dot(dz)[0]
        dx = self.W.dot(dz.T).T
        # =========================================
        return dx

class SoftmaxOutputLayer:
    def __init__(self, num_hidden_2, num_outputs):
        # Weights and bias
        self.W = np.random.rand(num_hidden_2, num_outputs)
        self.b = np.zeros(num_outputs)
        # Gradient of Weights and bias
        self.dW = None
        self.db = None
        # Input (x), label(y), prediction(y_hat)
        self.x = None
        self.y = None
        self.y_hat = None
        # Loss
        self.loss = None

    def forward(self, x, y):
        """
        Softmax output layer forward
        - Make prediction
        - Calculate loss

        ## Already Implemented ##
        """
        self.y_hat = self.predict(x)
        self.y = y
        self.x = x

        self.loss = self.ce_loss(self.y_hat, self.y)

        return self.loss

    def ce_loss(self, y_hat, y):
        """
        Calcualte "Cross-entropy loss"
        Add 'eps' for stability inside log function.

        [Inputs]
            y_hat : Prediction
            y : Label

        [Outputs]
            loss value
        """
        eps = 1e-10
        ce_loss = None
        # =============== EDIT HERE ===============
        ce_loss = - np.mean(np.sum(y * np.log(y_hat + eps), axis=1))
        # =========================================
        return ce_loss

    def predict(self, x):
        """
        Make prediction in probability. (Not 0, 1, 2 ... label!!)
        # Use softmax function above.

        [Inputs]
            x : input data

        [Outputs]
            y_hat : Prediction
        """
        y_hat = None
        # =============== EDIT HERE ===============
        z = x.dot(self.W) + self.b
        y_hat = softmax(z)
        # =========================================
        return y_hat

    def backward(self, d_prev=1):
        """
        Calculate gradients of input (x), W, b of this layer.
        Save self.dW, self.db to update later.

        x and (W & b) --> z -- (activation) --> y_hat --> Loss
        dx and (dW & db) <-- dz <-- (activation) <-- dy_hat <-- Loss

        [Inputs]
            d_prev : Gradients until here. (Always 1 since its output layer)

        [Outputs]
            dx : Gradients of output layer input x (Not MLP input x!)
        """
        batch_size = self.y.shape[0]
        dx = None
        # =============== EDIT HERE ===============
        dz = (self.y_hat - self.y) / self.y.shape[0] * d_prev
        self.dW = self.x.T.dot(dz)
        self.db = np.ones_like(dz.T).dot(dz)[0]
        dx = self.W.dot(dz.T).T
        # =========================================
        return dx

# ======================================================================