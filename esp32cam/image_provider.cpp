
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
#include "image_provider.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "app_camera_esp.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "image_util.h"
#include "model_settings.h"

#define TF_LITE_REPORT_ERROR(reporter, ...) \
  do {                                      \
    reporter->Report(__VA_ARGS__);          \
  } while (false)

camera_fb_t* fb = NULL;
static const char* TAG = "app_camera";

// Get the camera module ready
TfLiteStatus InitCamera(tflite::ErrorReporter* error_reporter) {
  int ret = app_camera_init();
  if (ret != 0) {
    TF_LITE_REPORT_ERROR(error_reporter, "Camera init failed\n");
    return kTfLiteError;
  }
  TF_LITE_REPORT_ERROR(error_reporter, "Camera Initialized\n");
  return kTfLiteOk;
}

extern "C" int capture_image() {
  fb = esp_camera_fb_get();
  if (!fb) {
    ESP_LOGE(TAG, "Camera capture failed");
    return -1;
  }
  return 0;
}

// Begin the capture and wait for it to finish
TfLiteStatus PerformCapture(tflite::ErrorReporter* error_reporter,
                            float* image_data, uint8_t* input_data, int* input_border) {
  /* 2. Get one image with camera */
  int img_size = kNumCols * kNumRows * kNumChannels;
  uint8_t * tmp_buffer = (uint8_t *) malloc(img_size);
  int input_width=input_border[2]-input_border[0]+1;
  int input_height=input_border[3]-input_border[1]+1;
  image_resize_linear(tmp_buffer,input_data,kNumCols,kNumRows, kNumChannels,input_width,input_height);
  for (int i=0; i < img_size; i++)
  {
   image_data[i] = tmp_buffer[i] /255.0f;
  }

  // Debug Out
  TF_LITE_REPORT_ERROR(error_reporter, "");
  char str[128];
  for (int y = 0; y < 28; y += 2) {
    int pos = 0;
    memset(str, 0, sizeof(str));
    for (int x = 0; x < 28; x += 1) {
      int getPos = y * 28 + x;
      int color = 255-tmp_buffer[getPos];

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
  free(tmp_buffer);
  /* here the esp camera can give you grayscale image directly */
  return kTfLiteOk;
  
}

// Get an image from the camera module
TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, float* image_data, uint8_t* input_data, int* input_border) {
  static bool g_is_camera_initialized = false;
  /* Camera Captures Image of size 96 x 96  which is of the format grayscale
     thus, no need to crop or process further , directly send it to tf */
  TfLiteStatus capture_status = PerformCapture(error_reporter, image_data, input_data, input_border);
  if (capture_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "PerformCapture failed\n");
    return capture_status;
  }
  return kTfLiteOk;
}
