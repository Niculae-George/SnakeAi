#pragma once
#include <vector>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>

struct Layer {
    int size;
    int prevSize;
    std::vector<std::vector<double>> weights;
    std::vector<double> biases;
    std::vector<double> outputs;
    std::vector<double> deltas; // For backprop

    Layer(int s, int ps) : size(s), prevSize(ps) {
        outputs.resize(size);
        deltas.resize(size);
        biases.resize(size);
        weights.resize(size, std::vector<double>(prevSize));
        
        // Random init
        for(int i=0; i<size; ++i) {
            biases[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
            for(int j=0; j<prevSize; ++j) {
                weights[i][j] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
            }
        }
    }
};

class NeuralNetwork {
public:
    std::vector<Layer> layers;
    double learningRate = 0.01;

    void addLayer(int size) {
        if (layers.empty()) {
            // Input layer has no weights/prevSize, conceptually just placeholders
            // But usually we just define input size separately. 
            // Let's assume the user adds Input Layer first with prevSize=0 (ignored)
            layers.emplace_back(size, 0); 
        } else {
            layers.emplace_back(size, layers.back().size);
        }
    }

    std::vector<double> feedForward(const std::vector<double>& inputs) {
        // Set input layer outputs
        if (inputs.size() != layers[0].size) return {};
        
        layers[0].outputs = inputs;

        // Forward prop
        for (size_t i = 1; i < layers.size(); ++i) {
            for (int j = 0; j < layers[i].size; ++j) {
                double sum = layers[i].biases[j];
                for (int k = 0; k < layers[i].prevSize; ++k) {
                    sum += layers[i].weights[j][k] * layers[i-1].outputs[k];
                }
                // ReLU for hidden, Linear/Sigmoid for output? 
                // Let's use Tanh or Sigmoid for stability in simple C++ implementations
                layers[i].outputs[j] = std::tanh(sum); 
            }
        }
        return layers.back().outputs;
    }

    void backPropagate(const std::vector<double>& targets) {
        // Output layer gradients
        Layer& outLayer = layers.back();
        for (int i = 0; i < outLayer.size; ++i) {
            double output = outLayer.outputs[i];
            double error = targets[i] - output;
            // Derivative of Tanh is 1 - output^2
            outLayer.deltas[i] = error * (1 - output * output);
        }

        // Hidden layer gradients
        for (int i = (int)layers.size() - 2; i > 0; --i) {
            Layer& curr = layers[i];
            Layer& next = layers[i+1];
            for (int j = 0; j < curr.size; ++j) {
                double error = 0.0;
                for (int k = 0; k < next.size; ++k) {
                    error += next.deltas[k] * next.weights[k][j];
                }
                curr.deltas[j] = error * (1 - curr.outputs[j] * curr.outputs[j]);
            }
        }

        // Update Weights
        for (size_t i = 1; i < layers.size(); ++i) {
            for (int j = 0; j < layers[i].size; ++j) {
                layers[i].biases[j] += learningRate * layers[i].deltas[j];
                for (int k = 0; k < layers[i].prevSize; ++k) {
                    layers[i].weights[j][k] += learningRate * layers[i].deltas[j] * layers[i-1].outputs[k];
                }
            }
        }
    }
};
