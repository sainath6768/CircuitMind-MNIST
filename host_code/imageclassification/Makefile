CXX = g++
CXXFLAGS = -Wall -Iinc -Iinclude 
LDFLAGS = -Wl,-rpath,./lib -lonnxruntime -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
OBJECT_DIR = object
BUILD_DIR = build
TARGETS = $(BUILD_DIR)/imageClassifier

all: $(OBJECT_DIR) $(BUILD_DIR) $(TARGETS)

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGETS): $(OBJECT_DIR)/main.o $(OBJECT_DIR)/image_classifier.o $(OBJECT_DIR)/serial_port_handler.o $(OBJECT_DIR)/sd_data_handler.o $(OBJECT_DIR)/onnx_inference.o
	$(CXX) $(CXXFLAGS) $(OBJECT_DIR)/main.o $(OBJECT_DIR)/serial_port_handler.o $(OBJECT_DIR)/image_classifier.o $(OBJECT_DIR)/sd_data_handler.o $(OBJECT_DIR)/onnx_inference.o -o $(TARGETS) -L lib $(LDFLAGS)

$(OBJECT_DIR)/main.o: src/main.cpp inc/image_classifier.h
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o $(OBJECT_DIR)/main.o

$(OBJECT_DIR)/image_classifier.o: src/image_classifier.cpp inc/image_classifier.h
	$(CXX) $(CXXFLAGS) -c src/image_classifier.cpp -o $(OBJECT_DIR)/image_classifier.o

$(OBJECT_DIR)/serial_port_handler.o: src/serial_port_handler.cpp inc/serial_port_handler.h
	$(CXX) $(CXXFLAGS) -c src/serial_port_handler.cpp -o $(OBJECT_DIR)/serial_port_handler.o

$(OBJECT_DIR)/sd_data_handler.o: src/sd_data_handler.cpp inc/sd_data_handler.h
	$(CXX) $(CXXFLAGS) -c src/sd_data_handler.cpp -o $(OBJECT_DIR)/sd_data_handler.o

$(OBJECT_DIR)/onnx_inference.o: src/mnist_onnx.cpp inc/mnist_onnx.h
	$(CXX) $(CXXFLAGS) -c src/mnist_onnx.cpp -o $(OBJECT_DIR)/onnx_inference.o

clean:
	@rm -rf $(OBJECT_DIR)/*.o $(BUILD_DIR)/*
