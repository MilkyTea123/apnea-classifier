/*
  IMU Classifier
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU, once enough samples are read, it then uses a
  TensorFlow Lite (Micro) model to try to classify the movement as a known gesture.
  Note: The direct use of C/C++ pointers, namespaces, and dynamic memory is generally
        discouraged in Arduino examples, and in the future the TensorFlowLite library
        might change to make the sketch simpler.
  The circuit:
  - Arduino Nano 33 BLE or Arduino Nano 33 BLE Sense board.
*/

#include <Arduino_LSM9DS1.h>

#include <TensorFlowLite.h>

#include <PDM.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"

#include "model.h"
#include "dsp.hpp"
#include "features.hpp"
#include <vector>

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 2 * 1024;
byte tensorArena[tensorArenaSize];

// array to map gesture index to a name
const char* CLASSES[] = {
  "normal",
  "apnea"
};

#define NUM_CLASSES (sizeof(CLASSES) / sizeof(CLASSES[0]))

#define RAW_SAMPLE_RATE 16000
#define SAMPLE_RATE 50
#define WINDOW_SIZE 1.5
#define SAMPLES_PER_WINDOW SAMPLE_RATE*WINDOW_SIZE

typedef struct {
  volatile float mean;
  volatile float std;
  volatile float median;
  volatile float iqr;
  volatile float nn50_1;
  volatile float nn50_2;
  volatile float pnn50_1;
  volatile float pnn50_2;
  volatile float sdsd;
  volatile float rmssd;
  volatile float mad;
} Features;

short sampleBuffer[(short)(SAMPLES_PER_WINDOW)];
short rawSampleBuffer[(int)(RAW_SAMPLE_RATE / SAMPLE_RATE)];
// short rawSampleBuffer[1024];

volatile int rawSamplesRead = 0;
volatile int samplesRead = 0;
volatile uint8_t invokeFlag = 0;

volatile uint8_t test_count = 0;

volatile Features features = {0};

void setup() {
  // pinMode(5, INPUT);

  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting setup...");

  // Initialize PDM
  PDM.setBufferSize((int)(2*RAW_SAMPLE_RATE/SAMPLE_RATE));
  PDM.onReceive(onPDMdata);

  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM.");
    while (1);
  } else {
    Serial.println("PDM started successfully.");
  }

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  // if (model->version() != TFLITE_SCHEMA_VERSION) {
  //   error_reporter->Report("Model provided is schema version %d not equal "
  //                          "to supported version %d.",
  //                          model->version(), TFLITE_SCHEMA_VERSION);
  //   while (1);
  // }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  // tflInterpreter->AllocateTensors();
  if (tflInterpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("AllocateTensors() failed");
    while (1);
  } else {
    Serial.println("Tensors Allocated");
  }

  // Print memory usage
  Serial.print("Used bytes: ");
  Serial.println(tflInterpreter->arena_used_bytes());

  // Check if the tensor arena size is sufficient
  if (tflInterpreter->arena_used_bytes() > tensorArenaSize) {
    Serial.println("ERROR: Tensor arena size is too small!");
    while (1);
  } else {
    Serial.println("Tensor arena size is sufficient.");
  }

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);

  printTensorShape(tflInputTensor);
  printTensorShape(tflOutputTensor);

  samplesRead = 0;
  invokeFlag = 0;
}

void loop() {

  if (!invokeFlag) {
    if (rawSamplesRead > 0 && samplesRead <= SAMPLES_PER_WINDOW) {
      float sum = 0.0;
      for (int num : rawSampleBuffer) {
        // Serial.println(num);
        sum += num;
      }
      sampleBuffer[samplesRead] = sum / rawSamplesRead;
      samplesRead++;

      // Reset the raw sample read count
      rawSamplesRead = 0;
    }

    if (samplesRead >= SAMPLES_PER_WINDOW) {

      /****
      * TESTING
      ****/
      float sum = 0.0;
      for (int num : sampleBuffer) {
        sum += num;
      }
      Serial.print("Minute Average: ");
      Serial.println(sum / SAMPLES_PER_WINDOW);
      features.mean = sum / SAMPLES_PER_WINDOW;
      /****
      * TESTING END
      ****/

      classifyData();
      delay(10);

      samplesRead = 0;
    }

  } else {
    // Loop through the output tensor values from the model
    for (int i = 0; i < NUM_CLASSES; i++) {
      Serial.print(CLASSES[i]);
      Serial.print(": ");
      Serial.println(tflOutputTensor->data.f[i], 6);
    }
    Serial.println("Classified");
    Serial.println();
    invokeFlag = 0;
  }

}

void onPDMdata() {
  // Query the number of available bytes
  int bytesAvailable = PDM.available();

  if (bytesAvailable > 0) {
    // Read into the sample buffer
    PDM.read(rawSampleBuffer, bytesAvailable);

    // 16-bit, 2 bytes per sample
    rawSamplesRead = bytesAvailable / 2;
  }
}

float classifyData() {
  std::vector<float> data;
  for (int i = 0; i < samplesRead; i++) {
    data.push_back((float)sampleBuffer[i]);
  }
  std::vector<float> filtered(data.size());

  filter(data, filtered, 5, 35, SAMPLE_RATE, 1);
  // normalize(filtered, data); // normalize features

  std::vector<float> rrIntervals(data.size()-1); // TODO: convert to RR intervals
  rrIntervals = {0};

  extractFeatures(rrIntervals);

  Serial.println("Invoking");
  validateTensorData(tflInputTensor);
  TfLiteStatus invokeStatus = tflInterpreter->Invoke();
  Serial.println("Invoked");
  // if (invokeStatus != kTfLiteOk) {
  //   Serial.println("Invoke failed!");
  //   while (1);
  // } else {
  //   Serial.println("Invoke success!");
  //   invokeFlag = 1;
  // }
}

void extractFeatures(std::vector<float> &rrIntervals) {
  // TODO assign functions to features

  tflInputTensor->data.f[0] = 0.846857; // mean
  tflInputTensor->data.f[1] = 0.05344003; // std
  tflInputTensor->data.f[2] = 0.835; // median
  tflInputTensor->data.f[3] = 0.0675; // iqr
  tflInputTensor->data.f[4] = 4; // nn50_1
  tflInputTensor->data.f[5] = 7; // nn50_2
  tflInputTensor->data.f[6] = 0.05714286; // pnn50_1
  tflInputTensor->data.f[7] = 0.1; // pnn50_2
  tflInputTensor->data.f[8] = 0.03589583; // sdsd
  tflInputTensor->data.f[9] = 0.03591455; // rmssd
  tflInputTensor->data.f[10] = 0.04174694; // mad

  Serial.println("Features Extracted");
  delay(10);
}

void printTensorShape(TfLiteTensor* tensor) {
  // Get the number of dimensions of the tensor
  int numDimensions = tensor->dims->size;

  Serial.print("Tensor has ");
  Serial.print(numDimensions);
  Serial.println(" dimensions:");

  // Print each dimension
  for (int i = 0; i < numDimensions; i++) {
    Serial.print("Dimension ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(tensor->dims->data[i]);
  }
}

void validateTensorData(TfLiteTensor* tensor) {
  for (int i = 0; i < tensor->dims->data[1]; ++i) {
    Serial.print("Input tensor data[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(tensor->data.f[i]);
  }
}