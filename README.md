# EE400a Final Project: DPAP Sleep Apnea Detection
by the Sleepiest Soldiers<br>
Contributors: Michael Tsien, Anya Guyton, Andrew Pan, Andy Cai
## Snoring Detection
## ECG Signal Classification
To classify ECG data, we looked at "Detection of obstructive sleep apnea through ECG signal features" (https://ieeexplore.ieee.org/document/6220730) by L. Almazaydeh, K. Elleithy and M. Faezipour to find critical features to train our machine learning model. Training used the "Apnea-ECG Dataset" from Physionet (https://physionet.org/content/apnea-ecg/1.0.0/), which contained ECG readings lasting about 7 to 10 hours for each of 35 patients with each minute annotated by experts to denote whether the patient was experiencing an apnea episode. The data included multiple filetypes, but for our project, we only needed the .apn, .hea files for annotation labels, and .dat for the ECG signal values. 
