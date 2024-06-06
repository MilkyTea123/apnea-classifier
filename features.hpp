/*
  Feature extraction function declare. Most features
  use RR-intervals from ECG data, which is the time in
  between R-peaks of the ECG signal. Information about
  RR-intervals and the features used can be found at
  https://ieeexplore.ieee.org/document/6220730.
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
    arr - data to find the standard deviation for
*/
float findStdDev(vector<float> arr);


/*
  Returns the median of the provided data. Assumes
  the input data is sorted.
  Parameters:
    sorted - data to find the median for
*/
float findMedian(vector<float> sorted);

/*
  Returns the interquartile range (IQR) of the provided
  data. Assumes the input data is sorted.
  Parameters:
    sorted - data to find the IQR for
*/
float findIQR(vector<float> sorted);

/*
  Returns an array of the NN50 values of the given array.
  Output index 1 is NN50 1, the number of times one RR
  interval exceeds the next by 50 ms.
  Output index 2 is NN50 2, the number of times one RR
  interval exceeds the previous by 50 ms.
  Parameters:
    rrIntervals - array of RR-intervals
*/
vector<int> findNN50(vector<float> rrIntervals);

/*
  Returns the standard deviation of adjacent RR-interval
  differences.
  Parameters:
    signal - array of RR-intervals
*/
float findSDSD(vector<float> signal);

/*
  Returns the square root of the mean of the sum of
  adjacent RR-interval differences.
  Parameters:
    signal - array of RR-intervals
*/
float findRMSSD(vector<float> signal);

/*
  Returns the mean of magnitude of the
  differences between all RR-intervals and
  the mean RR-intervals.
  Parameters:
    signal - array of RR-intervals
*/
float findMAD(vector<float> signal);