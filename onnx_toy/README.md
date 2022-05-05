# 模型

官方下载地址：
- https://digital-standard.com/threedpose/models/Resnet34_3inputs_448x448_20200609.onnx
- https://digital-standard.com/threedpose/models/HighQualityTrainedModel.nn
- 

# 工具

pip install netron
https://github.com/lutzroeder/netron

# 依赖

- pip install opencv-python
- pip install onnxruntime-gpu
- CUDA SDK 11.4
- TensorRT SDK 8.2
- CUDNN 8.2.2

CUDNN 的子文件夹，复制进  c:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.4\
TensorRT-8.2.4.2\lib\*.dll，复制进  c:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.4\bin