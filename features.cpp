#include <vector>
#include <cmath>

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