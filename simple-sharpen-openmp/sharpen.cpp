#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;
//global variables
int THREADS;
int video_fps, video_totalFrames;
VideoCapture inputVideo;
VideoWriter outputVideo;
VideoWriter *outputVideos;
Size size;
float KERNEL[3][3] = {{1,0,-1},{0,0,0},{-1,0,1}};


Mat applySharpen(Mat input){
    Mat output = input.clone();
    for(int x =0; x< size.width; x++){
        for(int y=0; y< size.height; y++){
            int loc = (x + y * size.width);
            float blue,green,red;
            blue=green=red=0;
            Vec3b pixel;
            int coeficent = 0;
            for (int i = 0 ; i < 3; i++){
                for (int j = 0; j < 3; j++){
                    int xloc = x+i;
                    int yloc = y+j;
                    int loc_conv = (xloc + size.width*yloc);
                    if(0<=i && i<size.width-1 && 0<=j && j<size.height-1){
                        coeficent = KERNEL[i][j];
                        pixel = input.at<Vec3b>(loc_conv);
                        blue += pixel.val[0] * coeficent;
                        green += pixel.val[1] * coeficent;
                        red += pixel.val[2] * coeficent;
                    }
                }
            }
            Vec3b pixelSharpen = Vec3b(blue, green, red);
            output.at<Vec3b>(loc)= pixelSharpen;
        }
    }
    return output;
}


void processFrames(int nThread) {
    // sabemos el hilo, numero de frames total
    unsigned long int ini = (int)(video_totalFrames/THREADS)*(nThread);
    unsigned long int fin = (int)(video_totalFrames/THREADS) + ini;
    printf("Hilo: %d Inicio: %ld , Fin: %ld \n", nThread, ini, fin);

    for(int i = ini; i < fin; i++){
        Mat src, res;
        // inputVideo >> src;              // read
        if (src.empty()) break;         // check if at end
        res = applySharpen(src);
       //outputVideo.write(res); //save or
       outputVideos[nThread] << res;
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
    size = Size((int) inputVideo.get(CAP_PROP_FRAME_WIDTH),
    (int) inputVideo.get(CAP_PROP_FRAME_HEIGHT));

    video_fps = inputVideo.get(CV_CAP_PROP_FPS);
    video_totalFrames = inputVideo.get(CV_CAP_PROP_FRAME_COUNT);

    printf("Processing video ... \n FPS: %d ,  Total Frames: %d \n", video_fps, video_totalFrames);

    // video Writter
    outputVideos = new *VideoWriter[THREADS];
    for (int i=1;i<THREADS+1;i++)
    {
        outputVideos[i-1] = new VideoWriter();
    }
    outputVideo.open(oFile, ex, inputVideo.get(CAP_PROP_FPS), size, true);
     
    #pragma omp parallel num_threads(THREADS)
	{
        int j = omp_get_thread_num();
        processFrames(j);
    }




    Mat src, res;
/*     for(int i=0;i<video_totalFrames; i++) //Show the image captured in the window and repeat
    {
        inputVideo >> src;              // read
        if (src.empty()) break;         // check if at end
        res = applySharpen(src);
       //outputVideo.write(res); //save or
       outputVideo << res;
    } */



    // width = input.cols;
    // height =input.rows;
    // // define the output as a clone of input image
    // output = input.clone();


    // printf(" Processing image %s \n width: %d  - Heigh : %d \n",argv[1],width,height);

    // printf("Kernel : %d   Number of Threads: %d \n",KERNEL,THREADS);
    //  //launch threads
    // pthread_t tids[THREADS+1];

    // for(long i = 1; i<=THREADS; i++)
    // {
    //     pthread_create(&tids[i], NULL, blur, (void*)i);
    // }

    // //wait for threads...
    // for(int k = 1; k<=THREADS; k++)
    // {
    //     pthread_join(tids[k], NULL);
    // }

    // //write the output image 
    // imwrite( oFile, output );


    return 0;

}