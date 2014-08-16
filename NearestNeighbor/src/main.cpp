#include "OpenCV/OpenCV.h"

//CV_EXPORTS_W void batchDistance(InputArray src1, InputArray src2,
//                                OutputArray dist, int dtype, OutputArray nidx,
//                                int normType = NORM_L2, int K = 0,
//                                InputArray mask = noArray(), int update = 0,
//                                bool crosscheck = false);

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    namedWindow("nearest");
    const int W = 400;
    const int H = 400;
    const int NA = 20;
    const int NB = 40;
    Mat3b canvas(H, W);
    Mat1f inA(NA, 2);
    Mat1f inB(NB, 2);

    while (true)
    {
        canvas.setTo(CV_BLACK);

        for (int i=0;i<NA;i++)
        {
            inA(i, 0) = rand() % W;
            inA(i, 1) = rand() % H;
            circle(canvas, Point(inA(i, 0), inA(i, 1)), 4, CV_RED);
        }

        for (int i=0;i<NB;i++)
        {
            inB(i, 0) = rand() % W;
            inB(i, 1) = rand() % H;
            circle(canvas, Point(inB(i, 0), inB(i, 1)), 4, CV_BLUE);
        }
        Mat dist;
        Mat nidx;
        batchDistance(inA, inB, dist, CV_32FC1, nidx, NORM_L2, 1, noArray(), 0, false);
        print(dist);printf("\n\n");
        print(nidx);printf("\n\n");

        for (int i=0;i<NA;i++)
        {
            int nn = nidx.at<int>(i);
            if (nn != -1)
            {
                line(canvas, Point(inA(i,0), inA(i, 1)),
                    Point(inB(nn,0), inB(nn,1)),
                    CV_WHITE);
            }
        }

        imshow("nearest", canvas);

        if (waitKey() == 0x1B)
            break;
    }

    return 0;
}
