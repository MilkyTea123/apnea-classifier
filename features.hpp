/*
  Feature extraction function declare.
  Contributor(s): Michael Tsien
*/

#include <vector>

using namespace std;

/*
  Finds the local maximum of the given array.
  Parameters:
    arr    - signal data
    window - in ms, how large of a window to detect for local maxima
    sr     - sampling rate
*/
vector<int> findLocalMaxima(vector<float> arr, int sr, int window);

/*
  Returns an integer array of the index values of the local peaks of
  the given data array.
  Parameters:
    data      - array to find the local maxima for
    height    - minimum height of peaks
    distance  - minimum distance between peaks
*/
vector<int> find_peaks(vector<float> data, float height, float distance);

/*
  Returns the time (s) between adjacent peaks of the given signal array.
  Parameters:
    signal  - array to find the time intervals between peaks
    sr      - sample rate (Hz)
*/
vector<float> findRRIntervals(vector<float> signal, int sr);

/*
  Returns the mean of the provided data.
  Parameters:
    arr - data to find the mean for
*/
float findMean(vector<float> arr);

/*
  Returns the standard deviation of
  the provided data.
  Parameters:
    arr - data to find the mean for
*/
float findStdDev(vector<float> arr);

float findMedian(vector<float> sorted);

float findIQR(vector<float> sorted);

vector<int> findNN50(vector<float> rrIntervals);

float findSDSD(vector<float> signal);

float findRMSSD(vector<float> signal);

float findMAD(vector<float> signal);