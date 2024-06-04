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

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "model.h"
#include "dsp.hpp"
#include "features.hpp"
#include <vector>

// array to map gesture index to a name
const char* CLASSES[] = {
  "normal",
  "apnea"
};

#define NUM_CLASSES (sizeof(CLASSES) / sizeof(CLASSES[0]))

#define RAW_SAMPLE_RATE 16000
#define SAMPLE_RATE 100
#define WINDOW_SIZE 30
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

namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* tflModel = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* model_input = nullptr;
  int32_t previous_time = 0;

  // Create an area of memory to use for input, output, and intermediate arrays.
  // The size of this will depend on the model you're using, and may need to be
  // determined by experimentation.
  constexpr int kTensorArenaSize = 2 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];
  float_t* model_input_buffer = nullptr;
}  // namespace

short sampleBuffer[(short)(SAMPLES_PER_WINDOW)];
short rawSampleBuffer[(int)(RAW_SAMPLE_RATE / SAMPLE_RATE)];
// short rawSampleBuffer[1024];

volatile int rawSamplesRead = 0;
volatile int samplesRead = 0;
volatile uint8_t invokeFlag = 0;

volatile Features features = {0};

void setup() {

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

  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         tflModel->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // tflite::AllOpsResolver tflOpsResolver;
  tflite::AllOpsResolver micro_op_resolver;

  static tflite::MicroInterpreter static_interpreter(
      tflModel, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Get information about the memory area to use for the model's input.
  model_input = interpreter->input(0);
  if ((model_input->dims->size != 2) || (model_input->dims->data[0] != 1) ||
      (model_input->type != kTfLiteFloat32)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
  }
  model_input_buffer = model_input->data.f;
}

void loop() {

  if (!invokeFlag) {
    if (rawSamplesRead > 0 && samplesRead <= SAMPLES_PER_WINDOW) {
      float sum = 0.0;
      for (int num : rawSampleBuffer) {
        sum += num;
      }
      sampleBuffer[samplesRead] = sum / rawSamplesRead;
      samplesRead++;

      // Reset the raw sample read count
      rawSamplesRead = 0;
    }

    if (samplesRead >= SAMPLES_PER_WINDOW) {
      Serial.println("Running inference...");

      extractFeatures();

      model_input_buffer[0] = features.mean;
      model_input_buffer[1] = features.std;
      model_input_buffer[2] = features.median;
      model_input_buffer[3] = features.iqr;
      model_input_buffer[4] = features.nn50_1;
      model_input_buffer[5] = features.nn50_2;
      model_input_buffer[6] = features.pnn50_1;
      model_input_buffer[7] = features.pnn50_2;
      model_input_buffer[8] = features.sdsd;
      model_input_buffer[9] = features.rmssd;
      model_input_buffer[10] = features.mad;

      for (int i = 0; i < 11; i++) {
        Serial.print(model_input_buffer[i]);
        Serial.print(", ");
      }
      Serial.println();

      samplesRead = 0;
    }

  } else {
    Serial.println("Inference Output:");
    // Loop through the output tensor values from the model
    TfLiteTensor* output = interpreter->output(0);

    Serial.print("Apnea Likelihood: ");
    Serial.println(output->data.f[0], 3);
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

void extractFeatures() {
  std::vector<float> data;
  for (int i = 0; i < samplesRead; i++) {
    data.push_back((float)sampleBuffer[i]);
  }
  std::vector<float> filtered(data.size());

  filter(data, filtered, 2, 50, SAMPLE_RATE, 1);
  // normalize(filtered, data); // normalize features

  std::vector<float> rrIntervals = findRRIntervals(filtered, SAMPLE_RATE);
  int numIntervals = rrIntervals.size();

  for (int i = 0; i < filtered.size(); i++) {
    Serial.print(filtered[i]);
    Serial.print(", ");
  }
  Serial.println();

  if (numIntervals > 5) {
    std::vector<int> nn50 = findNN50(rrIntervals);

    features.mean = findMean(rrIntervals);
    features.std = findStdDev(rrIntervals);
    features.nn50_1 = nn50[0]; // nn50_1
    features.nn50_2 = nn50[1]; // nn50_2
    features.pnn50_1 = (float)nn50[0] / numIntervals; // pnn50_1
    features.pnn50_2 = (float)nn50[1] / numIntervals; // pnn50_2
    features.sdsd = findSDSD(rrIntervals); // sdsd
    features.rmssd = findRMSSD(rrIntervals); // rmssd
    features.mad = findMAD(rrIntervals); // mad

    std::sort(rrIntervals.begin(), rrIntervals.end());

    for (int i = 0; i < rrIntervals.size(); i++) {
      Serial.print(rrIntervals[i]);
      Serial.print(", ");
    }
    Serial.println();

    features.median = findMedian(rrIntervals); // median
    features.iqr = findIQR(rrIntervals); // iqr

    Serial.println("Features Extracted");
  } else {
    features.mean = 0;
    features.std = 0;
    features.median = 0; // median
    features.iqr = 0; // iqr
    features.nn50_1 = 0; // nn50_1
    features.nn50_2 = 0; // nn50_2
    features.pnn50_1 = 0; // pnn50_1
    features.pnn50_2 = 0; // pnn50_2
    features.sdsd = 0; // sdsd
    features.rmssd = 0; // rmssd
    features.mad = 0; // mad

    Serial.println("Features Not Found");
  }

  invokeFlag = 1;
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