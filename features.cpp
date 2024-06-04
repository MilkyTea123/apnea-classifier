#include <vector>
#include <cmath>
#include "features.hpp"

using namespace std;

// inputs:
// window - in ms, how large of a window to detect for local maxima
// sr - sampling rate
vector<int> findLocalMaxima(vector<float> arr, int sr, int window) {
  vector<int> maxima = {}; // output vector for indexes of local maxima
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

vector<int> find_peaks(vector<float> data, float height, float distance) {
  vector<int> peaks;
  for (int i = 1; i < data.size()-1; i++) {
    if (data[i-1] < data[i] && data[i] > data[i+1] && data[i] > height) {
      if (peaks.empty() || (i - peaks.back()) > distance) {
        peaks.push_back(i);
      }
    }
  }
  return peaks;
}

vector<float> findRRIntervals(vector<float> signal, int sr) {
  float height = findMean(signal) + 1.5 * findStdDev(signal);
  float distance = sr * 0.5;
  vector<int> peakInds = find_peaks(signal, height, distance);
  vector<float> rrIntervals;
  for (int i = 1; i < peakInds.size(); i++) {
    rrIntervals.push_back((float)(peakInds[i] - peakInds[i-1]) / sr);
  }
  return rrIntervals;
}

// returns the rr intervals, input is an array of RRs
vector<float> rr_intervals(vector<int> rr, int sr) {
  vector<float> rr_int = {};
  // push the intervals between the RR
  for (int i = 0; i < rr.size(); i++ ) {
    rr_int.push_back((rr[i] - rr[i-1]) / sr);
  }
  return rr_int;
}

// window in ms
vector<float> rr_intervals_from_samples(vector<float> signal, int sr, int window) {
  vector<int> rr = findLocalMaxima(signal, sr, window);
  return rr_intervals(rr, sr);
} 

// returns mean of given array
float findMean(vector<int> arr) {
  float sum = 0.0;
  for (int num : arr) {
    sum += num;
  }
  return sum / arr.size();
}

float findMean(vector<float> arr) {
  float sum = 0.0;
  for (float num : arr) {
    sum += num;
  }
  return sum / arr.size();
}

// returns standard deviation of given array
float findStdDev(vector<int> arr) {
  float mean = findMean(arr);

  // calculate variance
  float var = 0.0;
  for (int num : arr) {
    var += (num - mean) * (num - mean); 
  }
  var = var / arr.size();

  //calculate standard deviation
  return sqrt(var);
}

float findStdDev(vector<float> arr) {
  float mean = findMean(arr);

  // calculate variance
  float var = 0.0;
  for (float num : arr) {
    var += (num - mean) * (num - mean); 
  }
  var = var / arr.size();

  //calculate standard deviation
  return sqrt(var);
}

float findMedian(vector<float> sorted) {
  int med = sorted.size() / 2;
  return sorted[med];
}

float findIQR(vector<float> sorted) {
  int len = sorted.size();
  int q1 = len/2 - len/4;
  int q3 = len/2 + len/4;
  return sorted[q3] - sorted[q1];
}

vector<int> findNN50(vector<float> rrIntervals) {
  vector<int> nn50 = { 0, 0 };
  for (int i = 0; i < rrIntervals.size()-1; i++) {
    if (rrIntervals[i] - rrIntervals[i + 1] > 0.05) {
      nn50[0]++;
    }
    if (rrIntervals[i+1] - rrIntervals[i] > 0.05) {
      nn50[1]++;
    }
  }

  return nn50;
}

float findSDSD(vector<float> signal) {
  vector<float> diffs;
  for (int i = 1; i < signal.size(); i++) {
    diffs.push_back(signal[i] - signal[i-1]);
  }
  int len = diffs.size();
  float diffMean = findMean(diffs);
  float sum = 0;
  for (int i = 0; i < len; i++) {
    sum += (diffs[i] - diffMean)*(diffs[i] - diffMean);
  }

  return sqrt(sum / len);
}

float findRMSSD(vector<float> signal) {
  float sumSqrDiffs = 0;
  int len = 0;
  for (int i = 1; i < signal.size(); i++) {
    sumSqrDiffs += (signal[i] - signal[i-1])*(signal[i] - signal[i-1]);
    len++;
  }

  return sqrt(sumSqrDiffs / len);
}

float findMAD(vector<float> signal) {
  float mean = findMean(signal);
  float sum = 0;
  for (float n : signal) {
    sum += abs(n - mean);
  }

  return sum / signal.size();
}
  