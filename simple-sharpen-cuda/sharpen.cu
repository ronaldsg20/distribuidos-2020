
/**
* Sharpen
*/

#include <stdio.h>
#include <math.h>
// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda.h>
#include <helper_cuda.h>
#include <cuda_runtime.h>

#include <stdlib.h>
#include <cstdint>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>


using namespace cv;
using namespace std;

int video_fps;
int h_video_totalFrames;
int h_threads;
int h_width;
int h_height;

int *h_kernel;
unsigned char *h_inputVideoArray;
unsigned char *h_outputVideoArray;

VideoCapture inputVideo;
VideoWriter outputVideo;

// function aviable only on the device
__device__ void applySharpen(int rowStart,int rowEnd, unsigned char *input, unsigned char *output, int *kernel,int *w, int *h, int *totalFrames){
    int frameIndex = 0;
    for(int y=rowStart; y< rowEnd; y++){
        frameIndex = (y/((int) *h));
        int yframe = y - (frameIndex * ((int)*h));
        for(int x =0; x< *w; x++){
            int blue,green,red;
            blue=green=red=0;
            int coeficent = 0;
            for (int i = 0 ; i < 3; i++){
                for (int j = 0; j < 3; j++){
                    // para un frameIndex = a (1)   =>  row0 = h * a (1080 * 1) = 1080
                    // y = 1100   frameIndex = 1  y'= 20 = 1100 - (frameIndex * 1080)
                    int xloc = x+i;
                    int yloc = yframe+j;
                    if(0<=xloc && xloc<((int)*w)-1 && 0<=yloc && yloc<((int)*h)-1){
                        coeficent = kernel[(i*3)+j];
                        blue += input[(frameIndex* ((int)*w) *((int)*h)*3)+(xloc*((int)*h)*3)+(yloc*3)+0] *coeficent;
                        green += input[(frameIndex* ((int)*w) *((int)*h)*3)+(xloc*((int)*h)*3)+(yloc*3)+1] *coeficent;
                        red += input[(frameIndex* ((int)*w) *((int)*h)*3)+(xloc*((int)*h)*3)+(yloc*3)+2] *coeficent;
                    }
                }
            }
            output[(frameIndex*((int)*w)*((int)*h)*3)+(x*((int)*h)*3)+(yframe*3)+0] = blue;
            output[(frameIndex*((int)*w)*((int)*h)*3)+(x*((int)*h)*3)+(yframe*3)+1] = green;
            output[(frameIndex*((int)*w)*((int)*h)*3)+(x*((int)*h)*3)+(yframe*3)+2] = red;
            // printf("Sharpen applied pixel x: %d - y: %d \n", x, y);
        }
    }
}

 /**
  * CUDA Kernel Device code
  * 
  */ 
 /*****************************************************************************/
 __global__ void sharpen(unsigned char *input,unsigned char *output, int *kernel, int *totalThreads, int *width, int *height, int *totalFrames){
    int tn = (blockDim.x * blockIdx.x) + threadIdx.x;
    // int ini = (int)(((int) *totalFrames)/((int) *totalThreads))*(tn);
    // int fin = (int)(((int) *totalFrames)/((int) *totalThreads)) + ini;
    int ri = (int)((((int) *totalFrames) * ((int) *height))/((int) *totalThreads))*(tn);
    int rf = (int)((((int) *totalFrames) * ((int) *height))/((int) *totalThreads)) + ri;
    // printf("[%d] %d - %d \n", tn, ((int) *totalFrames), ((int) *totalThreads));
    // printf("[%d] ini: %d - fin: %d \n",tn, ri, rf);
    if(tn < (int) *totalFrames * (int) *height ){
        // printf("[%d] ini: %d - fin: %d \n",tn, ri, rf);
        applySharpen(ri, rf, input, output, kernel,width, height, totalFrames);
        // for(int i = ini; i < fin; i++){
        //     applySharpen(i, input, output, kernel,width, height);
        // }
    }
 }

 
void setVideoFrame(Mat frameInput, int frameIndex){
    for(int i=0;i<h_width;i++){
       for(int j=0;j<h_height;j++){
        h_inputVideoArray[(frameIndex*h_width*h_height*3)+(i*h_height*3)+(j*3)+0] = frameInput.at<Vec3b>(j,i)[0];
        h_inputVideoArray[(frameIndex*h_width*h_height*3)+(i*h_height*3)+(j*3)+1] = frameInput.at<Vec3b>(j,i)[1];
        h_inputVideoArray[(frameIndex*h_width*h_height*3)+(i*h_height*3)+(j*3)+2] = frameInput.at<Vec3b>(j,i)[2];
       }
     }
}

 void setVideoMatrix(VideoCapture inputVideo){
    Mat frameInput;
    for (int frameIndex = 0; frameIndex < h_video_totalFrames; frameIndex++){
        inputVideo.set(1,frameIndex);
        inputVideo.read(frameInput);
        if (frameInput.empty()){
            printf("Src empty");
        }
        setVideoFrame(frameInput, frameIndex);
    }
}



 /******************************************************************************
  * Host main routine
  */
 int main(int argc, char **argv)
 {   
     // define variables

     //global variabl

    Size S;

    int *d_video_totalFrames;
    int *d_threads;
    int *d_kernel;
    int *d_width;
    int *d_height;
    
    // Mat output;
    // Mat input;
    // handle errors

    cudaError_t error = cudaSuccess;
     //********************read parameters**********************
     if ( argc != 4 )
    {
        printf("usage: ./sharpen <Video_Path> <Video_out_Path> <ThreadsPerBlock>n");
        return -1;
    }

    String oFile = argv[2];

    //read the video and set width and height
    inputVideo = VideoCapture(argv[1]);
    if (!inputVideo.isOpened()){
        cout  << "Could not open the input video: " << argv[1] << endl;
        return -1;
    }

    //get codec type
    int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));

    // get size of video
    S = Size((int) inputVideo.get(CAP_PROP_FRAME_WIDTH),
    (int) inputVideo.get(CAP_PROP_FRAME_HEIGHT));

    printf("Frame size width: %d - height: %d \n", S.width, S.height);

    video_fps = inputVideo.get(CV_CAP_PROP_FPS);
    h_video_totalFrames = 50; //inputVideo.get(7);
    h_height = S.height;
    h_width = S.width;
    
    // h_kernel = 

    // getting device information
    cudaSetDevice(0);
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp,0);

   int blocks = deviceProp.multiProcessorCount;

   int h_threadsPerBlock = atoi(argv[3]);//(h_video_totalFrames*h_height)/blocks;
   h_threads = h_threadsPerBlock * blocks;
   //printf("Blocks : %d   -  threads per block %d  - TOTAL threads: %d",blocks,h_threads,blocks*h_threads);

    printf(" Processing video %s \n width: %d  - Heigh : %d \n",argv[1],h_width,h_height);

    // ************************ video pointers ***********************************
    // int *d_input;
    // int *d_output;
    unsigned char *d_inputVideoArray;
    unsigned char *d_outputVideoArray;
     // malloc and cudaMalloc
     error = cudaMalloc((void **)&d_height,sizeof(int));
     if (error != cudaSuccess){
        fprintf(stderr, "Failed to allocate mem for d_height (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
     error = cudaMalloc(&d_kernel,sizeof(int) * 9);
     if (error != cudaSuccess){
        fprintf(stderr, "Failed to allocate mem for d_kernel (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
     error = cudaMalloc(&d_width,sizeof(int));
     if (error != cudaSuccess){
        fprintf(stderr, "Failed to allocate mem for d_width (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
     error = cudaMalloc(&d_threads,sizeof(int));
     if (error != cudaSuccess){
        fprintf(stderr, "Failed to allocate mem for d_threads (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
    error = cudaMalloc(&d_video_totalFrames,sizeof(int));
        if (error != cudaSuccess){
        fprintf(stderr, "Failed to allocate mem for d_threads (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }

     error = cudaMalloc(&d_inputVideoArray,h_width*h_height*sizeof(char)*3*h_video_totalFrames);
     if (error != cudaSuccess){
        fprintf(stderr, "Failed to allocate mem for d_input (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
     error = cudaMalloc(&d_outputVideoArray,h_width*h_height*sizeof(char)*3*h_video_totalFrames);
     if (error != cudaSuccess){
        fprintf(stderr, "Failed to allocate mem for d_output (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
    printf("cudaMalloc Done");

     size_t size = h_width * h_height * 3 * sizeof(char) * h_video_totalFrames;
     h_inputVideoArray = (unsigned char *)malloc(size);
     h_outputVideoArray = (unsigned char *)malloc(size);

     // allocate kernel space

     h_kernel = (int *)malloc(sizeof(int)*9);
     h_kernel[0] = 1;
     h_kernel[1] = 0;
     h_kernel[2] = -1;
     h_kernel[3] = 0;
     h_kernel[4] = 0;
     h_kernel[5] = 0;
     h_kernel[6] = -1;
     h_kernel[7] = 0;
     h_kernel[8] = 1;

     // set initial values

     setVideoMatrix(inputVideo);

     // MemCpy: host to device

     error = cudaMemcpy(d_inputVideoArray, h_inputVideoArray, size, cudaMemcpyHostToDevice);
     if (error != cudaSuccess){
        fprintf(stderr, "Failed to copy on device (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
    error = cudaMemcpy(d_kernel, h_kernel, sizeof(int)*9, cudaMemcpyHostToDevice);
    if (error != cudaSuccess){
        fprintf(stderr, "Failed to  to copy on device(error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
    error = cudaMemcpy(d_threads, &h_threads, sizeof(int), cudaMemcpyHostToDevice);
    if (error != cudaSuccess){
        fprintf(stderr, "Failed to  to copy on device (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
    error = cudaMemcpy(d_video_totalFrames, &h_video_totalFrames, sizeof(int), cudaMemcpyHostToDevice);
    if (error != cudaSuccess){
        fprintf(stderr, "Failed to  to copy on device (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
    error = cudaMemcpy(d_width, &h_width, sizeof(int), cudaMemcpyHostToDevice);
    if (error != cudaSuccess){
        fprintf(stderr, "Failed to  to copy on device (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
    error = cudaMemcpy(d_height, &h_height, sizeof(int), cudaMemcpyHostToDevice);
    if (error != cudaSuccess){
        fprintf(stderr, "Failed to  to copy on device (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }
    printf("CudaMemcpy host to device done.\n");

    printf("Blocks : %d , threads per block: %d\n",blocks, h_threadsPerBlock);

    // begin clock
    clock_t begin = clock();
     // Launch kernel 
     sharpen<<<blocks,h_threadsPerBlock>>>(d_inputVideoArray,d_outputVideoArray, d_kernel, d_threads, d_width, d_height, d_video_totalFrames);

    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("Threads: %d, Time: %2.4f \n", h_threads, time_spent);

    //last error
     error = cudaGetLastError();
    if (error != cudaSuccess){
        fprintf(stderr, "Failed to launch sharpen (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }

     // MemCpy: device to host
     error = cudaMemcpy(h_outputVideoArray, d_outputVideoArray, size, cudaMemcpyDeviceToHost);
     if (error != cudaSuccess){
        fprintf(stderr, "Failed to  to copy from device (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }


    outputVideo.open(oFile, ex, inputVideo.get(CAP_PROP_FPS), S, true);
 // fill the output video with the matrix of array frames
    Mat res;
    inputVideo.set(1,1);
    inputVideo.read(res);
    if (res.empty()){
        printf("Src empty");
    }
    for (int frameIndex = 0; frameIndex < h_video_totalFrames; frameIndex++){
        Mat it = res.clone();
        for(int i=0;i<h_width;i++){
            for(int j=0;j<h_height;j++){
                res.at<Vec3b>(j,i)[0] = h_outputVideoArray[(frameIndex*h_width*h_height*3)+(i*h_height*3)+(j*3)+0];
                res.at<Vec3b>(j,i)[1] = h_outputVideoArray[(frameIndex*h_width*h_height*3)+(i*h_height*3)+(j*3)+1];
                res.at<Vec3b>(j,i)[2] = h_outputVideoArray[(frameIndex*h_width*h_height*3)+(i*h_height*3)+(j*3)+2];
            }
        }
        outputVideo << it;
    }

     // save data


     // free memory

     cudaFree(d_height);
     cudaFree(d_width);
     cudaFree(d_outputVideoArray);
     cudaFree(d_inputVideoArray);
     cudaFree(d_kernel);
     cudaFree(d_threads);

     free(h_kernel);
     free(h_outputVideoArray);
     free(h_inputVideoArray);

     return 0;
 }
 
 