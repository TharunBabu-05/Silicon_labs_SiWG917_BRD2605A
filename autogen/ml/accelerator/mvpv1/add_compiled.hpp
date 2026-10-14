#include "tflite_micro_model_config.h"
#pragma once

#include "ml/third_party/tflm/common.h"



// Tensor indices as defined by Tensorflow
// https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/micro/kernels/add.cc
constexpr int kInputTensor1 = 0;
constexpr int kInputTensor2 = 1;
constexpr int kOutputTensor = 0;


constexpr const unsigned MVP_OUTPUT_ARRAY   = 0; // must be position 0, since fused relu reuses
constexpr const unsigned MVP_INPUT1_ARRAY   = 1;
constexpr const unsigned MVP_INPUT2_ARRAY   = 2;


namespace tflite {

extern int          cmsis_nn_add_get_init_size();
extern TfLiteStatus cmsis_nn_add_prepare(TfLiteContext* context, TfLiteNode* node);
extern TfLiteStatus cmsis_nn_add_invoke(TfLiteContext* context, TfLiteNode* node);

void* mvp_compiled_add_init(TfLiteContext* context, const char* buffer, size_t length);
TfLiteStatus mvp_compiled_add_prepare(TfLiteContext* context, TfLiteNode* node);
TfLiteStatus mvp_compiled_add_invoke(TfLiteContext* context, TfLiteNode* node);


} // namespace tflite