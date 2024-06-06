/*
  Digital signal processing function implementation,
  including filtering and normalization.
  Contributor(s): Michael Tsien
*/

#include <iostream>
#include <vector>
#include <cmath>
#include "features.hpp"

// from https://en.wikipedia.org/wiki/Low-pass_filter
void lpf(std::vector<float> &input, std::vector<float> &output,
    float dt, float cutoff) {
    float rc = 1 / (2*M_PI*cutoff);
    float alpha = dt / (rc + dt);
    output[0] = alpha * input[0];
    for (int i = 1; i < output.size(); i++) {
        output[i] = alpha*input[i] + (1-alpha)*output[i-1];
    }
}

// from https://en.wikipedia.org/wiki/High-pass_filter 
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

  // filter order number of times
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