/*
  Digital signal processing function declarations,
  including filtering and normalization.
  Contributor(s): Michael Tsien
*/

#include <vector>

#ifndef __FILTER__
#define __FILTER__

/*
  Simple first-order low-pass filter.
  Parameters:
    &input  - data to be filtered
    &output - array to put filtered data
    dt      - time between samples
    cutoff  - low-pass cutoff frequency (Hz)
*/
void lpf(std::vector<float> &input, std::vector<float> &output,
        float dt, float cutoff);

/*
  Simple first-order high-pass filter.
  Parameters:
    &input  - data to be filtered
    &output - array to put filtered data
    dt      - time between samples
    cutoff  - high-pass cutoff frequency (Hz)
*/
void hpf(std::vector<float> &input, std::vector<float> &output,
        float dt, float cutoff);

/*
  Bandpass filter.
  &input  - data to be filtered
  &output - array to put filtered data
  lowCutoff   - low-pass cutoff frequency (Hz)
  highCutoff  - high-pass cutoff frequency (Hz)
  fs          - sampling frequency (Hz)
  order       - order of the filter
*/
void filter(std::vector<float> &input, std::vector<float> &output,
        float lowCutoff, float highCutoff, float fs, int order);

/*
  Normalizes a given array.
  &input  - data to be normalized
  &output - array to put normalized data
*/
void normalize(std::vector<float> &input, std::vector<float> &output);

#endif // __FILTER__