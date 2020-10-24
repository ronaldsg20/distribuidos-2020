#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <string>

using namespace cv;
using namespace std;
//global variables
int THREADS;
int video_fps, video_totalFrames;
VideoCapture inputVideo;
VideoWriter outputVideo;
VideoWriter *outputVideos;
int **videoMatrix, **matrixOutput;
Size S;
float KERNEL[3][3] = {{1,0,-1},{0,0,0},{-1,0,1}};


void applySharpen(int *input, int frameIndex){
    for(int x =0; x< S.width; x++){
        for(int y=0; y< S.height; y++){
            int loc = (x + y * S.width);
            int blue,green,red;
            blue=green=red=0;
            Vec3b pixel;
            int coeficent = 0;
            for (int i = 0 ; i < 3; i++){
                for (int j = 0; j < 3; j++){
                    int xloc = x+i;
                    int yloc = y+j;
                    int loc_conv = (xloc + S.width*yloc);
                    if(0<=i && i<S.width-1 && 0<=j && j<S.height-1){
                        coeficent = KERNEL[i][j];
                        // pixel = input.at<Vec3b>(loc_conv);
                        blue += input[(yloc*S.width*3)+(xloc * 3) + 0] * coeficent;
                        green += input[(yloc*S.width*3)+(xloc * 3) + 1] * coeficent;
                        red += input[(yloc*S.width*3)+(xloc * 3) + 2] * coeficent;
                    }
                }
            }
            matrixOutput[frameIndex][(y*S.width*3)+(x * 3) + 0] = blue;
            matrixOutput[frameIndex][(y*S.width*3)+(x * 3) + 1] = green;
            matrixOutput[frameIndex][(y*S.width*3)+(x * 3) + 2] = red;
        }
    }
}

void processFrames(int nThread) {
    // sabemos el hilo, numero de frames total
    unsigned long int ini = (int)(video_totalFrames/THREADS)*(nThread);
    unsigned long int fin = (int)(video_totalFrames/THREADS) + ini;
    // printf("Hilo: %d Inicio: %ld , Fin: %ld \n", nThread, ini, fin);

    Mat src, res;
    for(int i = ini; i < fin; i++){
        // printf("indice %d \n", i);
        // inputVideo >> src;              // read
        // inputVideo.set(1,i);
        // inputVideo.read(src);
        // if (src.empty()){
        //     printf("Src empty");
        // }
        
        // imwrite("/home/elmar/Documents/Distribuidos/distribuidos-2020/files/myframe"+std::to_string(i)+".jpg", src);
        applySharpen(videoMatrix[i], i);
        // outputVideo.write(res); //save or
        // outputVideo << res;
    }
}

int main(int argc, char **argv)
{

   // read arguments
    if ( argc != 4 )
    {
        printf("usage: ./sharpen-video <Video_Path> <Video_out_Path> <THREADS>\n");
        return -1;
    }
    THREADS = atoi(argv[3]);
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

    video_fps = inputVideo.get(CV_CAP_PROP_FPS);
    video_totalFrames = inputVideo.get(7);

    videoMatrix = (int **)malloc(video_totalFrames * sizeof(int *));
    matrixOutput = (int **)malloc(video_totalFrames * sizeof(int *));

    for (int i = 0; i < video_totalFrames; i++){
        videoMatrix[i] = (int *)malloc(S.width* S.height * 3 * sizeof(int));
        matrixOutput[i] = (int *)malloc(S.width* S.height * 3 * sizeof(int));
    }

    printf("Processing video ... \n FPS: %d ,  Total Frames: %d \n", video_fps, video_totalFrames);

    outputVideo.open(oFile, ex, inputVideo.get(CAP_PROP_FPS), S, true);

    #pragma omp parallel num_threads(THREADS)
	{
        int j = omp_get_thread_num();
        processFrames(j);
        #pragma omp barrier
    }
    return 0;

}

void setVideoFrame(Mat frameInput, int frameIndex);

int setVideoMatrix(VideoCapture inputVideo){
    Mat src;
    for (int i = 0; i < video_totalFrames; i++){
        printf("indice %d \n", i);
        inputVideo.set(1,i);
        inputVideo.read(src);
        if (src.empty()){
            printf("Src empty");
        }
        setVideoFrame(src, i);
    }
}

void setVideoFrame(Mat frameInput, int frameIndex){
    for(int i=0;i<S.width;i++){
       for(int j=0;j<S.height;j++){
        videoMatrix[frameIndex][(j*S.width*3)+(i*3)+0]= frameInput.at<Vec3b>(i,j)[0];
        videoMatrix[frameIndex][(j*S.width*3)+(i*3)+1]= frameInput.at<Vec3b>(i,j)[1];
        videoMatrix[frameIndex][(j*S.width*3)+(i*3)+2]= frameInput.at<Vec3b>(i,j)[2];
       }
     }
}