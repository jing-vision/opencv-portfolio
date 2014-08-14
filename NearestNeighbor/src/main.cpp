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
    Mat1f inA(5, 1);
    Mat1f inB(10, 1);
    for (int i=0;i<5;i++) inA(i, 0) = rand() % 100;
    for (int i=0;i<10;i++) inB(i, 0) = rand() % 100;
    print(inA);printf("\n\n");
    print(inB);printf("\n\n");

    Mat dist;
    batchDistance(inA, inB, dist, CV_32FC1, noArray(), NORM_L2, 0, noArray(), 0, false);
    print(dist);printf("\n\n");

    Mat nidx;
    batchDistance(inA, inB, dist, CV_32FC1, nidx, NORM_L2, 1, noArray(), 0, true);
    print(dist);printf("\n\n");
    print(nidx);printf("\n\n");

    return 0;
}
