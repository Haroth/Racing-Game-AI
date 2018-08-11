from keras.models import Model
from keras.layers import Input, Dense
import numpy as np
import numpy.linalg as la
from keras.models import load_model


np.random.seed(7)

# -------------------------------------------------------------- loading data file
with open('input.txt') as f:
    data = f.read()

# -------------------------------------------------------------- creating data array and splitting it
data = np.array([[float(x) for x in line.split()] for line in data.split('\n')][:-1], dtype=np.float32)
n_in = 16
n_out = 4

n_val = len(data) - int(0.25 * len(data))

np.random.shuffle(data)

x = data[:, :n_in]
y = data[:, n_in:]

# -------------------------------------------------------------- input processing
x[:, 0] = x[:, 0] / 900 - 1
x[:, 1] = x[:, 1] / 400 - 1
x[:, 4] = x[:, 4] / 15
x[:, [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]] = -np.exp( - x[:, [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]] / 500) + 1

# -------------------------------------------------------------- choosing right inputs
inputs = [4, 6, 9, 10, 11, 14]

x_train = x[:n_val, inputs]
y_train = y[:n_val, :]

x_val = x[n_val:, inputs]
y_val = y[n_val:, :]

# -------------------------------------------------------------- creating structure of the net
input = Input((5,))
h = Dense(32, activation='relu')(input)
h = Dense(16, activation='relu')(h)
out_disc = Dense(4, activation='sigmoid')(h)


# -------------------------------------------------------------- creating model and training
model = Model(input, out_disc)
model.compile('adam', 'binary_crossentropy')
model.fit(x_train, y_train, 100, 1000, 2, validation_data=(x_val, y_val))

model.save('model')



