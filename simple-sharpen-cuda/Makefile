all:
	nvcc sharpen.cu `pkg-config opencv --cflags --libs opencv` -I "/usr/local/cuda/samples/common/inc" -std=c++11 -o sharpen