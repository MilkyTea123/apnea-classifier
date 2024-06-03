
#include <vector>

std::vector<int> findLocalMaxima(std::vector<float> arr, int sr, int window);

float findMean(std::vector<int> arr);

float findMean(std::vector<float> arr);

float findStdDev(std::vector<int> arr);

float findStdDev(std::vector<float> arr);

int findMedian(std::vector<float> sorted);

int findIQR(std::vector<float> sorted);

std::vector<float> rr_intervals_from_samples(std::vector<float> signal, int sr, int window);\

std::vector<float> findRRIntervals(std::vector<float> signal, int sr);