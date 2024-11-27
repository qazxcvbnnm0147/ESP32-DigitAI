

/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <TensorFlowLite_ESP32.h>

#include "main_functions.h"

#include "detection_responder.h"
#include "image_provider.h"
#include "model_settings.h"
#include "model_data.h"
#include "tensorflow/lite/experimental/micro/kernels/micro_ops.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/experimental/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "app_camera_esp.h"
#include "esp_camera.h"
#include "image_util.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#define TF_LITE_REPORT_ERROR(reporter, ...) \
  do {                                      \
    reporter->Report(__VA_ARGS__);          \
  } while (false)
return_array borders;

// WiFi
const char *ssid = "WSKGOOD_2.4"; // Enter your WiFi name
const char *password = "7064070640";  // Enter WiFi password
   
// MQTT Broker
const char *mqtt_broker = "f7b584c4d64c4839b32f09165b3fb6c9.s2.eu.hivemq.cloud";
const char *topic = "test";
const char *topic_sub = "iottalk/Dummy_Device/Output_1/Dummy_Control";
const char *mqtt_username = "s1104625";
const char *mqtt_password = "Aa715468200";
const int mqtt_port = 8883;

String msgStr = "";
char json[25];
int val;  

WiFiClientSecure espClient;
PubSubClient client(espClient);

// SSL/TLS 相關設定
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;

// An area of memory to use for input, output, and intermediate arrays.
constexpr int kTensorArenaSize = 60 * 1024;
static uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
void setup() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  //
  
  static tflite::MicroMutableOpResolver micro_mutable_op_resolver;

  micro_mutable_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_CONV_2D,
        tflite::ops::micro::Register_CONV_2D(), 1, 5);
  micro_mutable_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_MAX_POOL_2D,
        tflite::ops::micro::Register_MAX_POOL_2D(), 1 ,2);
  micro_mutable_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_RESHAPE,
        tflite::ops::micro::Register_RESHAPE(),1,4);
  micro_mutable_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_FULLY_CONNECTED,
        tflite::ops::micro::Register_FULLY_CONNECTED(), 1 ,9);
  micro_mutable_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_SOFTMAX,
        tflite::ops::micro::Register_SOFTMAX(), 1 ,4);

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, micro_mutable_op_resolver, tensor_arena, kTensorArenaSize,
      error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  // Get information about the memory area to use for the model's input.
  input = interpreter->input(0);

  Serial.begin(115200);
    // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  espClient.setCACert(root_ca);
  
  while (!client.connected()) {
      char* client_id = "esp32-client-";
      //client_id += char(WiFi.macAddress());
      Serial.println("Connecting to public emqx mqtt broker.....");
      if (client.connect(client_id, mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  TfLiteStatus init_status = InitCamera(error_reporter);
  if (init_status != kTfLiteOk) {
      error_reporter->Report("InitCamera failed\n");
      }
 

}

// The name of this function is important for Arduino compatibility.
void loop() {
  
  camera_fb_t* fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    ESP_LOGE(TAG, "Camera capture failed");
  }
  error_reporter->Report( "getfb");
  TF_LITE_REPORT_ERROR(error_reporter, "");

  char str[1280];
  for (int y = 0; y < 296; y += 16) {
    int pos = 0;
    memset(str, 0, sizeof(str));
    for (int x = 0; x < 400; x += 8) {
      int getPos = y * 400 + x;
      int color = fb->buf[getPos];

      if (color > 224) {
        str[pos] = ' ';
      } else if (color > 192) {
        str[pos] = '-';
      } else if (color > 160) {
        str[pos] = '+';
      } else if (color > 128) {
        str[pos] = '=';
      } else if (color > 96) {
        str[pos] = '*';
      } else if (color > 64) {
        str[pos] = 'H';
      } else if (color > 32) {
        str[pos] = '#';
      } else {
        str[pos] = 'M';
      }

      pos++;
    }
    TF_LITE_REPORT_ERROR(error_reporter, str);
  }

  try {
    borders = findBorderHistogram(fb->buf, fb->width, fb->height);
  }
  catch(return_array)
  {
    error_reporter->Report( "findbordererroe");
  }
 
  error_reporter->Report( "bordersize=%d",borders.array_size);
  for(int i=0;i<borders.array_size ;i++)
  {
    error_reporter->Report( "getborder=%d",borders.twod_array[i][0]);
  }
  
  // Get image from provider.
  msgStr = "";
  for(int i=0 ;i<borders.array_size ;i++)
  {
    
    uint8_t* output_data = outputImg(fb->buf, fb->width, fb->height, borders.twod_array[i]);
    error_reporter->Report( "getoupimg");
    if (kTfLiteOk != GetImage(error_reporter, kNumCols, kNumRows, kNumChannels,
                              input->data.f,output_data,borders.twod_array[i] )) {
      error_reporter->Report("Image capture failed.");
    }
    free(output_data);
    // Run the model on this input and make sure it succeeds.
    if (kTfLiteOk != interpreter->Invoke()) {
      error_reporter->Report("Invoke failed.");
    }
  
    TfLiteTensor* output = interpreter->output(0);
    
    // Process the inference results.
    
    for (int i=0; i < kCategoryCount; i++)
          {
            if (output->data.f[i]>0.6)
            {
              error_reporter->Report( "Label=%s",kCategoryLabels[i]);
              msgStr=msgStr+kCategoryLabels[i]+' ';
              
            }
          }
  }
  msgStr.toCharArray(json, 25);
  client.publish(topic, json);
  client.loop();
  free(borders.twod_array);
  esp_camera_fb_return(fb);
 
}
