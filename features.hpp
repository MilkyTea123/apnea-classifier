
#include <vector>

using namespace std;

vector<int> findLocalMaxima(vector<float> arr, int sr, int window);

float findMean(vector<int> arr);

float findMean(vector<float> arr);

float findStdDev(vector<int> arr);

float findStdDev(vector<float> arr);

float findMedian(vector<float> sorted);

float findIQR(vector<float> sorted);

vector<int> findNN50(vector<float> rrIntervals);

float findSDSD(vector<float> signal);

float findRMSSD(vector<float> signal);

float findMAD(vector<float> signal);

vector<float> rr_intervals_from_samples(vector<float> signal, int sr, int window);

vector<float> findRRIntervals(vector<float> signal, int sr);