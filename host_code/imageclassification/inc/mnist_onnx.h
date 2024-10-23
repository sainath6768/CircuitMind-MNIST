#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
#include <string>
#include "logger.h"
namespace fs = std::filesystem;
int OnnxInference(std::string, std::string);
