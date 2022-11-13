// KP-genetic.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <utility>

using namespace std;

const int initialIndividuals = 200;
vector<pair<int, string>> individuals(initialIndividuals);

const float mutationRate = 0.8;

int maxWeight, items, steps, maxCurrentCost = INT_MIN;
vector<int> weights;
vector<int> costs;
random_device rd;
mt19937 generator(rd());
uniform_real_distribution<float>  distribution(0, 1);
uniform_int_distribution<int>  individualPicker(0, initialIndividuals - 1);

bool toMutate() {
    return distribution(generator) <= mutationRate;
}

bool toInclude() {
    return distribution(generator) <= 0.5;
}

bool isConfigurationPossible(const string& configuration) {
    int length = configuration.length();
    int weight = 0;

    for (int i = 0; i < length; i++)
    {
        if (configuration[i] == '1') {
            weight += weights[i];
        }
    }

    return weight <= maxWeight;
}

void mutateConfiguration(pair<int, string>& configruationToMutate) {
    int length = configruationToMutate.second.length();
    uniform_int_distribution<int>  distribution(0, length - 1);
    int index = distribution(generator);
    if (configruationToMutate.second[index] == '0') {
        configruationToMutate.second[index] = '1';
        configruationToMutate.first += weights[index];
        return;
    }

    configruationToMutate.second[index] = '0';
    configruationToMutate.first -= weights[index];
}

pair<string, int> generateBoolVector() {
    string res;
    int currentWeight = 0;

    for (int i = 0; i < items; i++)
    {
        if (toInclude() && currentWeight + weights[i] < maxWeight) {
            res += '1';
            currentWeight += weights[i];
        }
        else {
            res += '0';
        }
    }

    return { res , currentWeight };
}

pair<int, string> crossOver(const string& first, const string& second) {
    int length = first.length();
    int currentWeight = 0;
    string result;
    for (int i = 0; i < length / 2; i++) {
        result += first[i];
        if (first[i] == '1') currentWeight += weights[i];
    }

    for (int i = length / 2; i < length; i++) {
        result += second[i];
        if (second[i] == '1') currentWeight += weights[i];
    }

    return { currentWeight, result };
}

int getConfigurationCost(const string& configuration) {
    int length = configuration.length();
    int cost = 0;
    for (size_t i = 0; i < length; i++)
    {
        if (configuration[i] == '1') {
            cost += costs[i];
        }
    }

    return cost;
}

void generatePopulation() {
    int populated = 0;
    int currentWeight = 0;
    while (populated < initialIndividuals) {
        pair<string, int> individual = generateBoolVector();
        int cost = individual.second > maxWeight ? 0 : getConfigurationCost(individual.first);
        individuals[populated] = { cost, individual.first };

        if (cost > maxCurrentCost) {
            maxCurrentCost = cost;
        }

        populated++;
    }
}

void input() {
    scanf_s("%d", &maxWeight);
    scanf_s("%d", &items);

    int currentWeight;
    int currentCost;
    for (int i = 0; i < items; i++)
    {
        scanf_s("%d", &currentWeight);
        scanf_s("%d", &currentCost);
        weights.push_back(currentWeight);
        costs.push_back(currentCost);
    }

    generatePopulation();
}

int getIndexOfWorstIndividual() {
    int index = 0;
    int min = INT_MAX;
    int size = individuals.size();
    for (int i = 0; i < size; i++)
    {
        if (individuals[i].first < min) {
            min = individuals[i].first;
            index = i;
        }
    }

    return index;
}

int main()
{
    input();
    int populations = 0;
    int lastMaxCurrentCost = maxCurrentCost;

    while (populations != 5000) {
        if (++steps % 5 == 0) {
            cout << maxCurrentCost << endl;
        }

        vector<pair<int, string>> children;
        //generate 25% population 4 times
        for (int i = 0; i < 4; i++)
        {
            int half = initialIndividuals / 2;
            half = half % 2 == 0 ? half : half + 1;
            vector<int> indexes;
            while (indexes.size() != half) {
                int index = individualPicker(generator);
                if (find(indexes.begin(), indexes.end(), index) == indexes.end()) {
                    indexes.push_back(individualPicker(generator));
                }
            }

            vector<pair<int, string>> tournamentWinners;
            for (size_t i = 0; i < half; i += 2)
            {
                tournamentWinners.push_back(individuals[indexes[i]] > individuals[indexes[i + 1]] ? individuals[indexes[i]] : individuals[indexes[i + 1]]);
            }

            for (size_t i = 0; i < tournamentWinners.size(); i += 2)
            {
                string first = tournamentWinners[i].second;
                string second = tournamentWinners[i + 1].second;

                pair<int, string> crossoverFirstSecond = crossOver(first, second);
                pair<int, string> crossoverSecondFirst = crossOver(second, first);

                if (toMutate()) {
                    mutateConfiguration(crossoverFirstSecond);
                }

                if (toMutate()) {
                    mutateConfiguration(crossoverSecondFirst);
                }

                int costOfCongiruation1 = getConfigurationCost(crossoverFirstSecond.second);
                costOfCongiruation1 = crossoverFirstSecond.first > maxWeight ? 0 : costOfCongiruation1;

                int costOfCongiruation2 = getConfigurationCost(crossoverSecondFirst.second);
                costOfCongiruation2 = crossoverSecondFirst.first > maxWeight ? 0 : costOfCongiruation2;

                children.push_back({ costOfCongiruation1, crossoverFirstSecond.second });
                children.push_back({ costOfCongiruation2, crossoverSecondFirst.second });

                if (costOfCongiruation1 > maxCurrentCost) {
                    maxCurrentCost = costOfCongiruation1;
                    lastMaxCurrentCost = maxCurrentCost;
                }

                if (costOfCongiruation2 > maxCurrentCost) {
                    maxCurrentCost = costOfCongiruation2;
                    lastMaxCurrentCost = maxCurrentCost;
                }
            }
        }

        //add children
        move(children.begin(), children.end(), back_inserter(individuals));

        //remove bad children
        while (individuals.size() != initialIndividuals) {
            int indexOfWorst = getIndexOfWorstIndividual();
            individuals.erase(individuals.begin() + indexOfWorst);
        }

        populations++;
    }

    return 0;
}

/*
Example:
5 3
3 2
1 5
2 3


5000 24
90 150
130 35
1530 200
500 160
150 60
680 45
270 60
390 40
230 30
520 10
110 70
320 30
240 15
480 10
730 40
420 70
430 75
220 80
70 20
180 12
40 50
300 10
900 1
2000 150

*/