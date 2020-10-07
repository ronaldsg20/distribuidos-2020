#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <cstdint>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;
//global variables
int KERNEL, THREADS;
int width,height;
Mat output;
Mat input;

void aplyBlur(int x, int y){
    // collect the average data of neighbours 
    int blue,green,red;
    blue=green=red=0;
    int n=0;
    Vec3b pixel;

    for(int i = x - (KERNEL/2); i < x+(KERNEL/2); i++)
    {    
        for (int j = y-(KERNEL/2); j < y+(KERNEL/2); j++)
        {
            //check if the point is in the image limits
            if(0<=i && i<width-1 && 0<=j && j<height-1){
                pixel = input.at<Vec3b>(Point(i,j));
                blue += pixel.val[0];
                green += pixel.val[1];
                red += pixel.val[2];
                n++;
            }
        }
    }
    
    if(n!=0){
         //write the average on the output image
        Vec3b pixelBlur = Vec3b(blue/n, green/n, red/n);
        output.at<Vec3b>(Point(x,y))= pixelBlur;
    }
   
}



void* blur (void* arg)
{
    // aplies the blur average to every pixel on the interval
    intptr_t tn = (intptr_t)arg;
    int ini = (int)(width/THREADS)*(tn-1);
	int fin = (int)(width/THREADS)+ini;
    for (int i = ini; i < fin; i++)
    {
        for (int j = 0; j < height; j++)
        {
            aplyBlur(i,j);
        }
    }

    pthread_exit(0);
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
    VideoCapture inputVideo(argv[1]);
    if (!inputVideo.isOpened()){
        cout  << "Could not open the input video: " << argv[1] << endl;
        return -1;
    }

    //get codec type
    int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));
    
    // get size of video
    Size S = Size((int) inputVideo.get(CAP_PROP_FRAME_WIDTH),
    (int) inputVideo.get(CAP_PROP_FRAME_HEIGHT));

    // video Writter
    VideoWriter outputVideo;
    outputVideo.open(oFile, ex, inputVideo.get(CAP_PROP_FPS), S, true);
     
    int channel = 1;

    Mat src, res;
    vector<Mat> spl;
    for(;;) //Show the image captured in the window and repeat
    {
        inputVideo >> src;              // read
        if (src.empty()) break;         // check if at end
        split(src, spl);                // process - extract only the correct channel
        for (int i =0; i < 3; ++i)
            if (i != channel)
                spl[i] = Mat::zeros(S, spl[0].type());
       merge(spl, res);
       //outputVideo.write(res); //save or
       outputVideo << res;
    }



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