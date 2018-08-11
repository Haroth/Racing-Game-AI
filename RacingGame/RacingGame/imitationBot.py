from subprocess import Popen, PIPE
from keras.models import load_model
import numpy as np

# -------------------------------------------------------------- rounding control and handling interferences
def roundControl(outputControl):
    if (outputControl[0] >= 0.5 and outputControl[1] >= 0.5):
        if (outputControl[0] >= outputControl[1]):
            outputControl[0] = 1
            outputControl[1] = 0
        else:
            outputControl[0] = 0
            outputControl[1] = 1
    else:
        outputControl[0] = round(outputControl[0])
        outputControl[1] = round(outputControl[1])

    if (outputControl[2] >= 0.5 and outputControl[3] >= 0.5):
        if (outputControl[2] >= outputControl[3]):
            outputControl[2] = 1
            outputControl[3] = 0
        else:
            outputControl[2] = 0
            outputControl[3] = 1
    else:
        outputControl[2] = round(outputControl[2])
        outputControl[3] = round(outputControl[3])
    return outputControl


# -------------------------------------------------------------- loading model
model = load_model('model')
p = Popen(['RacingGameWindow.exe'], shell=True, stdout=PIPE, stdin=PIPE)

# -------------------------------------------------------------- starting game loop
while True:
    # -------------------------------------------------------------- C++ input processing
    inputState = p.stdout.readline().strip().decode("utf-8")
    inputState = np.array([float(x) for x in inputState.split()], dtype=np.float32)

    inputState[0] = inputState[0] / 900 - 1;
    inputState[1] = inputState[1] / 400 - 1
    inputState[4] = inputState[4] / 15
    inputState[[5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]] = -np.exp(
        - inputState[[5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]] / 500) + 1

    gameFinished = inputState[16]

    inputState = inputState[[4, 6, 9, 10, 11, 14]]
    inputState = np.expand_dims(inputState, axis=0)

    # -------------------------------------------------------------- predicting output of the net and processing it
    outputControl = model.predict(inputState)
    outputControl = outputControl[0, :]
    outputControl = roundControl(outputControl)

    outputControl = ' '.join(map(str, outputControl))
    outputControl = outputControl + '\n'
    outputControl = bytes(outputControl, 'UTF-8')

    # -------------------------------------------------------------- sending output to game exe
    p.stdin.write(outputControl)
    p.stdin.flush()

