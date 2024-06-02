
#include <vector>

#ifndef __FILTER__
#define __FILTER__

void lpf(std::vector<float> &input, std::vector<float> &output,
        float dt, float cutoff);

void hpf(std::vector<float> &input, std::vector<float> &output,
        float dt, float cutoff);

void filter(std::vector<float> &input, std::vector<float> &output,
        float lowCutoff, float highCutoff, float fs, int order);

void normalize(std::vector<float> &input, std::vector<float> &output);

#endif // __FILTER__