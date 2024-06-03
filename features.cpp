#include <vector>
#include <cmath>
#include "features.hpp"

// inputs:
// window - in ms, how large of a window to detect for local maxima
// sr - sampling rate
std::vector<int> findLocalMaxima(std::vector<float> arr, int sr, int window) {
  std::vector<int> maxima = {}; // output vector for indexes of local maxima
  int window_len = (sr * window) / 1000;
  // initializing variables
  int count = 0;
  int max_index = 0;
  int max_val = 0;

  // find mean and standard deviation
  float mean  = findMean(arr);
  float std_dev = findStdDev(arr);

  //iterate through array, finding the maximum value in each window
  for (int i = 0; i < arr.size(); i ++) {
    count++;
    // replaces current max value and index if greater value is found
    if (arr[i] > max_val) {
        max_index = i;
        max_val = arr[i];
    }
    // the case of when the index is equal to a multiple of the window
    if (count >= window_len) {
      //Serial.println(i);
      if (max_val > (mean + 2 * std_dev)) { // add to vector of r-peaks if value is 2 std dev greater than mean
        maxima.push_back(max_index);
      }
      max_val = 0;
      count = 0;
    }
  }

  // // print the array
  // for (int i = 0; i < maxima.size(); i++) {
  //   Serial.println(maxima[i]);
  // }
  return maxima;
}

// returns the rr intervals, input is an array of RRs
std::vector<float> rr_intervals(std::vector<int> rr, int sr) {
  std::vector<float> rr_int = {};
  // push the intervals between the RR
  for (int i = 0; i < rr.size(); i++ ) {
    rr_int.push_back((rr[i] - rr[i-1]) / sr * 1000);
  }
  return rr_int;
}

// window in ms
std::vector<float> rr_intervals_from_samples(std::vector<float> signal, int sr, int window) {
  std::vector<int> rr = findLocalMaxima(signal, sr, window);
  return rr_intervals(rr, sr);
} 

// returns mean of given array
float findMean(std::vector<int> arr) {
  float sum = 0.0;
  for (int num : arr) {
    sum += num;
  }
  return sum / arr.size();
}

float findMean(std::vector<float> arr) {
  float sum = 0.0;
  for (float num : arr) {
    sum += num;
  }
  return sum / arr.size();
}

// returns standard deviation of given array
float findStdDev(std::vector<int> arr) {
  float mean = findMean(arr);

  // calculate variance
  float var = 0.0;
  for (int num : arr) {
    var += (num - mean) * (num - mean); 
  }
  var = var / arr.size();

  //calculate standard deviation
  return std::sqrt(var);
}

float findStdDev(std::vector<float> arr) {
  float mean = findMean(arr);

  // calculate variance
  float var = 0.0;
  for (float num : arr) {
    var += (num - mean) * (num - mean); 
  }
  var = var / arr.size();

  //calculate standard deviation
  return std::sqrt(var);
}