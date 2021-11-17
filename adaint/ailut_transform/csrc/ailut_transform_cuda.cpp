#include <torch/extension.h>

/* CUDA Forward Declarations */

void AiLutTransformForwardCUDAKernelLauncher(
    const torch::Tensor &input, const torch::Tensor &lut,
    const torch::Tensor &vertices, torch::Tensor output);


void AiLutTransformBackwardCUDAKernelLauncher(
    const torch::Tensor &grad_output, const torch::Tensor &input,
    const torch::Tensor &lut, const torch::Tensor &vertices,
    torch::Tensor grad_inp, torch::Tensor grad_lut, torch::Tensor grad_ver);


void ailut_transform_cuda_forward(
    const torch::Tensor &input,
    const torch::Tensor &lut,
    const torch::Tensor &vertices,
    torch::Tensor output) {

    AiLutTransformForwardCUDAKernelLauncher(input, lut, vertices, output);
}


void ailut_transform_cuda_backward(
    const torch::Tensor &grad_output,
    const torch::Tensor &input,
    const torch::Tensor &lut,
    const torch::Tensor &vertices,
    torch::Tensor grad_inp,
    torch::Tensor grad_lut,
    torch::Tensor grad_ver) {

    AiLutTransformBackwardCUDAKernelLauncher(
        grad_output, input, lut, vertices, grad_inp, grad_lut, grad_ver);
}


/* C++ Interfaces */

#define CHECK_CUDA(x) TORCH_CHECK(x.device().is_cuda(), #x " must be a CUDA tensor")
#define CHECK_CONTIGUOUS(x) TORCH_CHECK(x.is_contiguous(), #x " must be contiguous")
#define CHECK_INPUT(x) CHECK_CUDA(x); CHECK_CONTIGUOUS(x)


void ailut_transform_forward(
    const torch::Tensor &input,
    const torch::Tensor &lut,
    const torch::Tensor &vertices,
    torch::Tensor output) {
    
    if (input.device().is_cuda()) {
        CHECK_INPUT(input);
        CHECK_INPUT(lut);
        CHECK_INPUT(vertices);
        CHECK_INPUT(output);

        ailut_transform_cuda_forward(input, lut, vertices, output);
    } else {
        AT_ERROR("AiLutTransform is not implemented on CPU");
    }
}


void ailut_transform_backward(
    const torch::Tensor &grad_output,
    const torch::Tensor &input,
    const torch::Tensor &lut,
    const torch::Tensor &vertices,
    torch::Tensor grad_inp,
    torch::Tensor grad_lut,
    torch::Tensor grad_ver) {

    if (input.device().is_cuda()) {
        CHECK_INPUT(grad_output);
        CHECK_INPUT(input);
        CHECK_INPUT(lut);
        CHECK_INPUT(vertices);
        CHECK_INPUT(grad_inp);
        CHECK_INPUT(grad_lut);
        CHECK_INPUT(grad_ver);

        ailut_transform_cuda_backward(grad_output, input, lut, vertices, grad_inp, grad_lut, grad_ver);
    } else {
        AT_ERROR("AiLutTransform is not implemented on CPU");
    }
}


/* Interfaces Binding */

PYBIND11_MODULE(TORCH_EXTENSION_NAME, m) {
  m.def("forward", &ailut_transform_forward, "AiLut-Transform forward");
  m.def("backward", &ailut_transform_backward, "AiLut-Transform backward");
}

