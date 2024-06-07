# EE400a Final Project: DPAP Sleep Apnea Detection
by the Sleepiest Soldiers<br>
Contributors: Michael Tsien, Anya Guyton, Andrew Pan, Andy Cai
## Snoring Detection
### Training and Deploying
To classify snoring, we used the "Snoring Dataset" (https://www.kaggle.com/datasets/tareqkhanemu/snoring) from Kaggle by Tareq Khan. The dataset contains 1000 1-second audio samples, 500 with snoring and the other 500 without. We uploaded the files to Edge Impulse and trained a classification model using an MFCC for data processing. We then deployed the model provided by Edge Impulse to the Arduino Nano 33 BLE Sense. Then, we modified the provided file to update the output pressure of the CPAP machine based on the detection of snoring.
### Testing
To test the application, we first uploaded the program to the Arduino Nano 33 BLE Sense. Then, you could make or play any snoring sound and observe the Serial Monitor output to see whether snoring is detected and how the pressure is updated.
## ECG Signal Classification
To classify ECG data, we looked at "Detection of obstructive sleep apnea through ECG signal features" (https://ieeexplore.ieee.org/document/6220730) by L. Almazaydeh, K. Elleithy and M. Faezipour to find critical features to train our machine learning model. Training used the "Apnea-ECG Database" from Physionet (https://physionet.org/content/apnea-ecg/1.0.0/), which contained ECG readings lasting about 7 to 10 hours for each of 35 patients with each minute annotated by experts to denote whether the patient was experiencing an apnea episode. The data included multiple filetypes, but for our project, we only needed the .apn, .hea files for annotation labels, and .dat for the ECG signal values. These files can be found in the apnea.zip file in the main directory of the repository.
### Data Preprocessing and Training
To process and train the data, we used Google Colab. The notebook can be found at /ecg-classification/ApneaECGDataProcess.ipynb. To use the notebook, start by importing the required dependencies. Then, run the "Process Data" section. In this section, there are a  few key variables to change. NOTE: Skip the "For .wav Audio" section.
1. "parent_dir" under "Set Up Data Files" should be the directory containing the unzipped folder with the data files (.apn, .hea, .dat).
2. "data_dir" under "Set Up Data Files" should be set to the name of the folder containing the data files.
3. "datasets" under "For Features" should be the range of patients included in training the model (inclusive, exclusive).
4. "portion" under "For Features" determines how much the amount of data should be scaled down for training. (range is from 0 to 1)
### Training the Model
To train the model, use the "Keras Neural Network Model (from lab)" section. In this section, there are two options: "Multiple Patients," and "Single Patient." If you loaded in data from multiple patients, run "Multiple Patients." Otherwise, run "Single Patient." In these sections, you can adjust the typical hyperparameters: batch size, learning rate, epochs, etc. Running the "Convert to Tensorflow Lite" section will convert the trained model to .tflite and .h files and move them to the model directory, which is the "parent_dir/saved_models/." NOTE: the output model for the multiple patients is named "model2" and for a single patient is "model_p15," but these can be changed to whatever you like.
### Deploying
The next step is to deploy to the Arduino Nano 33 BLE Sense. To do so, the Arduino file at /ecg-classification/apnea-classifier/apnea-classifier.ino. Make sure that the "dsp" and "features" files are in the same directory. There are two macros that can be adjusted:
1. SAMPLE_RATE which is the simulated sample rate in Hz (recommended 100)
2. WINDOW_SIZE which is the length of time in seconds to collect samples before extracting (recommended 30 or 60)

NOTE: Making these values too high can cause the program to crash due to space limitations in the Arduino.

After uploading to the Arduino, the Serial Monitor will periodically output the likelihood of apnea.
### Testing
To test the performance, we used the ApneaECGDataProcess Colab notebook. First, you want to make sure that the data is imported as described above. Then in the "Application Testing" section, you can again choose the range of patients that can be selected for testing with the "datasets" variable. Running the cells should result in an apnea sample repeated over 10 minutes and a normal sample repeated over 10 minutes, both of which can be played to output the ECG sample aurally to test the apnea detection.
