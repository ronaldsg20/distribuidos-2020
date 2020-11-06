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
int *inputVideoArray, *outputVideoArray;
int **videoMatrix, **matrixOutput;
Size S;
float KERNEL[3][3] = {{1,0,-1},{0,0,0},{-1,0,1}};


void applySharpen(int frameIndex){
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
                    // printf("x: %d - y: %d loc_conv: %d \n",xloc,yloc, loc_conv);
                    if(0<=xloc && xloc<S.width-1 && 0<=yloc && yloc<S.height-1){
                        coeficent = KERNEL[i][j];
                        // pixel = input.at<Vec3b>(loc_conv);
                        blue += inputVideoArray[(frameIndex*S.width*S.height*3)+(xloc*S.height*3)+(yloc*3)+0] *coeficent;
                        green += inputVideoArray[(frameIndex*S.width*S.height*3)+(xloc*S.height*3)+(yloc*3)+1] *coeficent;
                        red += inputVideoArray[(frameIndex*S.width*S.height*3)+(xloc*S.height*3)+(yloc*3)+2] *coeficent;
                    }
                }
            }
            outputVideoArray[(frameIndex*S.width*S.height*3)+(x*S.height*3)+(y*3)+0] = blue;
            outputVideoArray[(frameIndex*S.width*S.height*3)+(x*S.height*3)+(y*3)+1] = green;
            outputVideoArray[(frameIndex*S.width*S.height*3)+(x*S.height*3)+(y*3)+2] = red;
            // printf("Sharpen applied pixel x: %d - y: %d \n", x, y);
        }
    }
}

void processFrames(int nThread) {
    // sabemos el hilo, numero de frames total
    unsigned long int ini = (int)(video_totalFrames/THREADS)*(nThread);
    unsigned long int fin = (int)(video_totalFrames/THREADS) + ini;
    printf("Hilo: %d Inicio: %ld , Fin: %ld \n", nThread, ini, fin);
    for(int i = ini; i < fin; i++){
        applySharpen(i);

    }
}
void setVideoFrame(Mat frameInput, int frameIndex);

void setVideoMatrix(VideoCapture inputVideo){
    Mat frameInput;
    for (int frameIndex = 0; frameIndex < video_totalFrames; frameIndex++){
        inputVideo.set(1,frameIndex);
        inputVideo.read(frameInput);
        if (frameInput.empty()){
            printf("Src empty");
        }
        setVideoFrame(frameInput, frameIndex);
    }
}

void setVideoFrame(Mat frameInput, int frameIndex){
    for(int i=0;i<S.width;i++){
       for(int j=0;j<S.height;j++){
        inputVideoArray[(frameIndex*S.width*S.height*3)+(i*S.height*3)+(j*3)+0] = frameInput.at<Vec3b>(i,j)[0];
        inputVideoArray[(frameIndex*S.width*S.height*3)+(i*S.height*3)+(j*3)+1] = frameInput.at<Vec3b>(i,j)[1];
        inputVideoArray[(frameIndex*S.width*S.height*3)+(i*S.height*3)+(j*3)+2] = frameInput.at<Vec3b>(i,j)[2];
       }
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

    printf("Frame size width: %d - height: %d \n", S.width, S.height);

    video_fps = inputVideo.get(CV_CAP_PROP_FPS);
    video_totalFrames = 75; //inputVideo.get(7);

    // videoMatrix = (int **)malloc(video_totalFrames * sizeof(int *));
    // matrixOutput = (int **)malloc(video_totalFrames * sizeof(int *));
    inputVideoArray = (int *)malloc(video_totalFrames * S.width * S.height * 3 * sizeof(int));
    outputVideoArray = (int *)malloc(video_totalFrames * S.width * S.height * 3 * sizeof(int));

    // for (int i = 0; i < video_totalFrames; i++){
    //     videoMatrix[i] = (int *)malloc(S.width* S.height * 3 * sizeof(int));
    //     matrixOutput[i] = (int *)malloc(S.width* S.height * 3 * sizeof(int));
    // }

    printf("Processing video ... \n FPS: %d ,  Total Frames: %d \n", video_fps, video_totalFrames);

    // fill the matrix of array frames
    setVideoMatrix(inputVideo);

    #pragma omp parallel num_threads(THREADS)
    {
        int j = omp_get_thread_num();
        processFrames(j);
    }

    printf("done \n");
    outputVideo.open(oFile, ex, inputVideo.get(CAP_PROP_FPS), S, true);

    // fill the output video with the matrix of array frames
    Mat res;
    inputVideo.set(1,1);
    inputVideo.read(res);
    if (res.empty()){
        printf("Src empty");
    }
    printf("mat Rows: %d, Cols: %d", res.rows, res.cols);
    for (int frameIndex = 0; frameIndex < video_totalFrames; frameIndex++){
        Mat it = res.clone();
        for(int i=0;i<S.width;i++){
            for(int j=0;j<S.height;j++){
                res.at<Vec3b>(j,i)[0] = outputVideoArray[(frameIndex*S.width*S.height*3)+(i*S.height*3)+(j*3)+0];
                res.at<Vec3b>(j,i)[1] = outputVideoArray[(frameIndex*S.width*S.height*3)+(i*S.height*3)+(j*3)+1];
                res.at<Vec3b>(j,i)[2] = outputVideoArray[(frameIndex*S.width*S.height*3)+(i*S.height*3)+(j*3)+2];
            }
            printf("Process row: %d \n", i);
        }
        outputVideo << it;
    }

    free(inputVideoArray);
    free(outputVideoArray);
    free(matrixOutput);
    free(videoMatrix);
    return 0;

}
