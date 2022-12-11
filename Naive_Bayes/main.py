import pandas as pd
import numpy as np
from collections import Counter

def calculate_probabilities(values):
    variables = len(values[0])
    rows = len(values)
    classes_counter = Counter(values[:,0])
    republicans = []
    democrats = []
    for v in range(1, variables):
        republicans.append({'y': 0, 'n': 0, '?': 0})
        democrats.append({'y': 0, 'n': 0, '?': 0})
        for r in range(0, rows):
            if values[r][0] == 'republican':
                republicans[v - 1][values[r][v]] += 1 / float(classes_counter['republican'])
            else:
                democrats[v - 1][values[r][v]] += 1 / float(classes_counter['democrat'])

    return {'republicans': republicans, 'democrats': democrats}

def classify(probabilities, test, class_probabilities):
    variables = len(test) - 1
    republican = 1
    democrat = 1
    for i in range(0, variables):
        republican *= probabilities['republicans'][i][test[i]]
        democrat *= probabilities['democrats'][i][test[i]]

    republican *= class_probabilities['republican']
    democrat *= class_probabilities['democrat']

    if republican > democrat:
        return 'republican'

    return 'democrat'


# Importing the dataset
dataset = pd.read_csv('house-votes-84.data')
x = dataset.values

n = int(len(x) / 10)

# using list comprehension
sets = [x[i * n:(i + 1) * n] for i in range((len(x) + n - 1) // n)]
class_counter = Counter(x[:,0])
class_probabilities = {'republican': class_counter['republican']/len(x), 'democrat': class_counter['democrat']/len(x)}

mean_accuracy = 0
for i in range(0, 10):
    training_set = [x for ind, x in enumerate(sets) if ind != i]
    flat_list = np.array([item for sublist in training_set for item in sublist])
    test_set = sets[i]
    probabilities = calculate_probabilities(flat_list)
    accuracy = 0
    for k in range(0, len(test_set)):
        real = test_set[k][0]
        classification = classify(probabilities, test_set[k][1:], class_probabilities)
        if real == classification:
            accuracy += 1

    accuracy /= float(len(test_set))
    print(accuracy)
    mean_accuracy += accuracy

print('Mean')
print(mean_accuracy / float(10))


