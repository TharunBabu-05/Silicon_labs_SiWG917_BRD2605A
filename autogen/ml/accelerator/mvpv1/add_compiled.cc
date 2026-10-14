#include "tflite_micro_model_config.h"
#include "ml/accelerator/mvpv1/add_compiled.hpp"
#include "ml/compiled_model/compiled_model.hpp"
#include "ml/accelerator/mvpv1/compiled_registration_helper.hpp"
#include "ml/accelerator/mvpv1/mvpv1_kernel_config.hpp"
#include "ml/accelerator/mvpv1/tflite_micro_mvpv1_context.hpp"


namespace tflite {

using namespace npu_toolkit;

namespace
{

TfLiteStatus mvp_prepare(TfLiteContext* context, TfLiteNode* node)
{
  return kTfLiteOk;
}

TfLiteStatus mvp_invoke(TfLiteContext* context, TfLiteNode* node)
{
  const auto input1 = tflite::micro::GetEvalInput(context, node, kInputTensor1);
  const auto input1_data  = tflite::micro::GetTensorData<int8_t>(input1);
  const auto input2 = tflite::micro::GetEvalInput(context, node, kInputTensor2);
  const auto input2_data  = tflite::micro::GetTensorData<int8_t>(input2);
  auto output       = tflite::micro::GetEvalOutput(context, node, kOutputTensor);
  auto output_data  = tflite::micro::GetTensorData<int8_t>(output);

  auto& mvpv1_context = *TfliteMicroMvpv1Context::get(context);
  auto array_base_addresses = mvpv1_context.array_base_addresses;

  array_base_addresses[MVP_INPUT1_ARRAY] = (uintptr_t)input1_data;
  array_base_addresses[MVP_INPUT2_ARRAY] = (uintptr_t)input2_data;
  array_base_addresses[MVP_OUTPUT_ARRAY] = (uintptr_t)output_data;
  mvpv1_context.update_array_base_addresses_callback = nullptr;

  return compiled_model_process_layer_with_accelerator(context);
}

} // namespace





void* mvp_compiled_add_init(TfLiteContext* context, const char* buffer, size_t length)
{
  return compiled_registration_init(
    context,
    buffer,
    length,
    0
    #if MVPV1_KERNELS_ADD_FALLBACK_ENABLED == 1
    ,cmsis_nn_add_get_init_size()
    #endif
  );
}


TfLiteStatus mvp_compiled_add_prepare(TfLiteContext* context, TfLiteNode* node)
{
  return compiled_registration_prepare(
    context,
    node,
    mvp_prepare
    #if MVPV1_KERNELS_ADD_FALLBACK_ENABLED == 1
    ,cmsis_nn_add_prepare
    #endif
  );
}

TfLiteStatus mvp_compiled_add_invoke(TfLiteContext* context, TfLiteNode* node)
{
  return compiled_registration_invoke(
    context,
    node,
    mvp_invoke
    #if MVPV1_KERNELS_ADD_FALLBACK_ENABLED == 1
    ,cmsis_nn_add_invoke
    #endif
  );
}



// Define the this kernel's registeration if this is an embedded build
TFLMRegistration Register_ADD() {
  return tflite::micro::RegisterOp(
    mvp_compiled_add_init,
    mvp_compiled_add_prepare,
    mvp_compiled_add_invoke
  );
}

} // namespace tflite