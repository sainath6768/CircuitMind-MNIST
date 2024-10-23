#include "mnist_onnx.h"
Ort::Session session(nullptr);

std::vector<float> softmax(const std::vector<float>& logits) 
{
    std::vector<float> softmax_output(logits.size());
    float sum = 0.0;
    for (float logit : logits)
    {
        sum += std::exp(logit);
    }
    for (size_t i = 0; i < logits.size(); ++i) {
        softmax_output[i] = std::exp(logits[i]) / sum;
    }
    return softmax_output;
}

int run_model(std::string image_path) {
    int predicted_class;
    // Load and preprocess image
    cv::Mat image = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
    if (image.empty()) 
    {
        LOG(LOGGER_LEVEL_ERROR,"Could not load image in the path ");
        std::cerr << "Could not load image: " << image_path << std::endl;
        
        return 1;
    }
    // Check if image size is correct (28x28)
    if (image.rows != 28 || image.cols != 28) 
    {
        std::cerr << "Invalid image size: " << image.size() << ". Expected 28x28." << std::endl;
        LOG(LOGGER_LEVEL_ERROR,"Invalid image size ");
        return 1;
    }
    cv::resize(image, image, cv::Size(28, 28));
    image.convertTo(image, CV_32F);

    // Prepare input tensor
    std::vector<float> input_tensor_values(image.begin<float>(), image.end<float>());
    std::vector<int64_t> input_shape = {1, 1, 28, 28};
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor(memory_info, input_tensor_values.data(), input_tensor_values.size(), input_shape.data(), input_shape.size());
    // Run inference
    const char* input_names[] = {"input"};
    const char* output_names[] = {"output"};
    
    try 
    {
        auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_names, &input_tensor, 1, output_names, 1);
        // Get results
        float* output = output_tensors[0].GetTensorMutableData<float>();
        std::vector<float> logits(output, output + 10); 
        // Apply softmax
        std::vector<float> softmax_output = softmax(logits);
        float threshold = 0.9;
        auto max_iter = std::max_element(softmax_output.begin(), softmax_output.end());
        float max_prob = *max_iter;
        predicted_class = std::distance(softmax_output.begin(), max_iter);

        // // Check if the maximum probability is above the threshold
        if (max_prob < threshold) {
            // std::cerr << "Invalid input image. Prediction confidence (" << max_prob << ") is below the threshold (" << threshold << ")." << std::endl;
            LOG(LOGGER_LEVEL_ERROR,"Invalid input image");
            return EXIT_FAILURE;
        }
        return predicted_class;
    }
    catch (const Ort::Exception& e) 
    {
        LOG(LOGGER_LEVEL_ERROR,"Inference failed to predict the image ");
        std::cerr << "Inference failed for image " << image_path << ": " << e.what() << std::endl;
        return 1;
    }
}



int OnnxInference(std::string type,std::string image_path) 
{
    // Initialize ONNX Runtime
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "mnist-inference");
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);
    
    if (strcmp(type.c_str(), "modelpath") == 0)
    {
        const std::string model_path = image_path;
        try 
        {
            session = Ort::Session(env, model_path.c_str(), session_options);
            LOG(LOGGER_LEVEL_INFO,"Model loaded successfully");
            // Testing model accuracy
            std::string test_dir = "data/test";
            int total = 0, count = 0;
            for (const auto& folder : fs::directory_iterator(test_dir)) {
                for (const auto& entry : fs::directory_iterator(folder.path().string())) {
                    std::string test_path = entry.path().string();
                    total++;
                    if (stoi(std::string(1,folder.path().string().back())) == run_model(test_path)) {
                        count++;
                    }
                }
            }
            float accuracy = ((float)count/total)*100;
            if (accuracy < 98) {
                LOG(LOGGER_LEVEL_INFO,"Model accuracy is below threashld");
                return EXIT_FAILURE;
            }
            LOG(LOGGER_LEVEL_INFO, "Accuracy of test data : %f", accuracy);
            return 0;
        }
        catch (const Ort::Exception& e) 
        {
            LOG(LOGGER_LEVEL_ERROR,"Failed to load model ");
            std::cerr << "Failed to load model: " << e.what() << std::endl;
            return 1;
        }
    }
    else if (strcmp(type.c_str(), "imagepath") == 0)
    {
         if (!session) 
        {
            std::cerr << "ONNX model session is not initialized!" << std::endl;
            LOG(LOGGER_LEVEL_ERROR,"ONNX model session is not intialized...!");
            return 1;
        }
        LOG(LOGGER_LEVEL_INFO,"Predicted digit for %s : %d",image_path.c_str(), run_model(image_path));
    }
    return 0;
}