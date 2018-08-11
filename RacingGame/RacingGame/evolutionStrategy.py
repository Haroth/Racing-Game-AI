from subprocess import Popen, PIPE
from keras.models import load_model
import numpy as np
import copy

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
model = load_model('model24-12')
weights = model.get_weights()

# -------------------------------------------------------------- creating parameters
inputSize = 6
firstLayerSize = 24
secondLayerSize = 12
outputSize = 4
sigma = 0.2
epocs = 300
population = 200
learningRate = 0.01

bestModelReward = 6.334 # current best model reward

populationWeights = np.empty((population,),dtype=object)
populationRewards = np.zeros(population)

# -------------------------------------------------------------- changing weights values to random
weights[0] = np.random.randn(inputSize, firstLayerSize)
weights[1] = np.random.randn(firstLayerSize)
weights[2] = np.random.randn(firstLayerSize, secondLayerSize)
weights[3] = np.random.randn(secondLayerSize)
weights[4] = np.random.randn(secondLayerSize, outputSize)
weights[5] = np.random.randn(outputSize)

bestModelWeights = copy.deepcopy(weights)


# -------------------------------------------------------------- starting epoc loop
for i in range(epocs):
    p = Popen(['RacingGamePopulation.exe'], shell=True, stdout=PIPE, stdin=PIPE) # open pipe for communication with exe
    # -------------------------------------------------------------- starting population loop
    for j in range(population):
        if j % 10 == 0:
           print(j, end=' ', flush=True)
        gameFinished = False

        # -------------------------------------------------------------- adding noise to weights of the net
        populationWeights[j] = copy.deepcopy(weights)

        rand = np.random.randn(inputSize, firstLayerSize)
        populationWeights[j][0] = populationWeights[j][0] + sigma * rand

        rand = np.random.randn(firstLayerSize)
        populationWeights[j][1] = populationWeights[j][1] + sigma * rand

        rand = np.random.randn(firstLayerSize, secondLayerSize)
        populationWeights[j][2] = populationWeights[j][2] + sigma * rand

        rand = np.random.randn(secondLayerSize)
        populationWeights[j][3] = populationWeights[j][3] + sigma * rand

        rand = np.random.randn(secondLayerSize, outputSize)
        populationWeights[j][4] = populationWeights[j][4] + sigma * rand

        rand = np.random.randn(outputSize)
        populationWeights[j][5] = populationWeights[j][5] + sigma * rand

        model.set_weights(populationWeights[j])

        # -------------------------------------------------------------- starting game loop
        while gameFinished == False:
            # -------------------------------------------------------------- C++ input processing
            inputState = p.stdout.readline().strip().decode("utf-8")
            inputState = np.array([float(x) for x in inputState.split()], dtype=np.float32)

            inputState[0] = inputState[0] / 900 - 1;
            inputState[1] = inputState[1] / 400 - 1
            inputState[4] = inputState[4] / 15
            inputState[[5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]] = -np.exp(- inputState[[5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]] / 500) + 1

            # -------------------------------------------------------------- checking for game ending
            gameFinished = inputState[16]
            if gameFinished == True:
                populationRewards[j] = inputState[17]
                if populationRewards[j] > bestModelReward:
                    bestModelReward = populationRewards[j]
                    model.save('bestModel24-12')
                    print("New best reward: " + str(bestModelReward))

            inputState = inputState[[4, 6, 9, 10, 11, 14]]
            inputState = np.expand_dims(inputState, axis=0)

            # -------------------------------------------------------------- predicting output of the net and processing it
            outputControl = model.predict(inputState)
            outputControl = outputControl[0, :]
            outputControl = roundControl(outputControl)

            outputControl = ' '.join(map(str, outputControl))
            outputControl = outputControl + '\n'

            outputControl = bytes(outputControl, 'UTF-8')
            p.stdin.write(outputControl)
            p.stdin.flush()


        # -------------------------------------------------------------- ending game loop
    # -------------------------------------------------------------- ending population loop
    p.kill()

    print(" ")
    print("Epoc: " + str(i))
    print("Min reward: " + str(min(populationRewards)))
    print("Max reward: " + str(max(populationRewards)))
    print("Mean reward: " + str(np.mean(populationRewards)))
    print("Standard dev: " + str(np.std(populationRewards)))

    # -------------------------------------------------------------- writing logs to the file
    with open('logs.txt', 'a') as file:
        file.write(str(i + 1) + " " + str(round(min(populationRewards), 3)) + " " + str(round(max(populationRewards), 3)) + " " +
        str(round(np.mean(populationRewards), 3)) + " " + str(round(np.std(populationRewards), 3)) + " ")

    # -------------------------------------------------------------- updating weights of the new model
    if (np.std(populationRewards) != 0):
        populationRewards = (populationRewards - np.mean(populationRewards)) / np.std(populationRewards)
    else:
        populationRewards = populationRewards - np.mean(populationRewards)
    for x in range(population):
        for layer in range(6):
            bestModelWeights[layer] = bestModelWeights[layer] + learningRate * populationWeights[x][layer] * populationRewards[x]
    model.set_weights(bestModelWeights)

    # -------------------------------------------------------------- checking reward of the new model
    newModelReward = 0
    p = Popen(['RacingGameOne.exe'], shell=True, stdout=PIPE, stdin=PIPE)

    gameFinished = False
    while gameFinished == False:
        inputState = p.stdout.readline().strip().decode("utf-8")
        inputState = np.array([float(x) for x in inputState.split()], dtype=np.float32)

        inputState[0] = inputState[0] / 900 - 1;
        inputState[1] = inputState[1] / 400 - 1
        inputState[4] = inputState[4] / 15
        inputState[[5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]] = -np.exp(
            - inputState[[5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]] / 500) + 1

        gameFinished = inputState[16]
        if gameFinished == True:
            newModelReward = inputState[17]

        inputState = inputState[[4, 6, 9, 10, 11, 14]]
        inputState = np.expand_dims(inputState, axis=0)

        outputControl = model.predict(inputState)
        outputControl = outputControl[0, :]
        outputControl = roundControl(outputControl)

        outputControl = ' '.join(map(str, outputControl))
        outputControl = outputControl + '\n'

        outputControl = bytes(outputControl, 'UTF-8')  # Needed in Python 3.
        p.stdin.write(outputControl)
        p.stdin.flush()
    # game loop
    p.kill()

    print("New model reward: " + str(newModelReward))
    with open('logs24-12.txt', 'a') as file:
        file.write(str(newModelReward) + "\n")

    weights = copy.deepcopy(bestModelWeights)

