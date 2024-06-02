// From ChatGPT

#include <iostream>
#include <vector>
#include <cmath>
#include "features.hpp"

// https://en.wikipedia.org/wiki/Low-pass_filter
void lpf(std::vector<float> &input, std::vector<float> &output,
    float dt, float cutoff) {
    float rc = 1 / (2*M_PI*cutoff);
    float alpha = dt / (rc + dt);
    output[0] = alpha * input[0];
    for (int i = 1; i < output.size(); i++) {
        output[i] = alpha*input[i] + (1-alpha)*output[i-1];
    }
}

void hpf(std::vector<float> &input, std::vector<float> &output,
    float dt, float cutoff) {
    float rc = 1 / (2*M_PI*cutoff);
    float alpha = rc / (rc + dt);
    output[0] = alpha * input[0];
    for (int i = 1; i < output.size(); i++) {
        output[i] = alpha*output[i-1] + alpha*(input[i]-input[i-1]);
    }
}

void filter(std::vector<float> &input, std::vector<float> &output,
  float lowCutoff, float highCutoff, float fs, int order) {
  std::vector<float> inter1(input.size());

  inter1 = input;

  for (int i = 0; i < order; i++) {
      lpf(input, inter1, 1/fs, highCutoff);
      hpf(inter1, output, 1/fs, lowCutoff);
  }

  output = inter1;
}

void normalize(std::vector<float> &input, std::vector<float> &output) {
    float mean = findMean(input);
    float std = findStdDev(input);
    output.resize(input.size());

    for (int i = 0; i < input.size(); i++) {
        output[i] = (input[i] - mean) / std;
    }
}

// int main() {
//     // Example usage
//     std::vector<float> input;
//     std::vector<float> output;
//     float samplingFreq = 1000.0; // Hz
//     float lowCutoffFreq = 5.0; // Hz
//     float highCutoffFreq = 35.0; // Hz
//     int order = 4;

//     for (int i = 0; i < samplingFreq*0.5; i++) {
//         input.push_back(sin(i*20/samplingFreq*2*M_PI)*sin(i*1/samplingFreq*2*M_PI)*sin(i*200/samplingFreq*2*M_PI));
//     }

//     output.resize(input.size());
//     std::vector<float> inter(input.size());

//     filter(input, inter, lowCutoffFreq, highCutoffFreq, samplingFreq, 6);
//     normalize(inter, output);

//     // Print the filtered output
//     std::cout << "test = [" << output[0];
//     for (size_t i = 1; i < output.size(); ++i) {
//         std::cout << ", " << output[i];
//     }
//     std::cout << "]" << std::endl;

//     return 0;
// }
