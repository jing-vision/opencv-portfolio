#include "opencv2/opencv.hpp"
#include "c:/Users/vincentz/Documents/InfiniTAM/letcv/Include/letcv_stereo.h"

#define OPENCV_VERSION CVAUX_STR(CV_MAJOR_VERSION)""CVAUX_STR(CV_MINOR_VERSION)""CVAUX_STR(CV_SUBMINOR_VERSION)

#ifdef _DEBUG
#pragma comment(lib,"opencv_core"OPENCV_VERSION"d.lib")
#pragma comment(lib,"opencv_imgproc"OPENCV_VERSION"d.lib")
#pragma comment(lib,"opencv_highgui"OPENCV_VERSION"d.lib")
#else	//_DEBUG
#pragma comment(lib,"opencv_core"OPENCV_VERSION".lib")
#pragma comment(lib,"opencv_imgproc"OPENCV_VERSION".lib")
#pragma comment(lib,"opencv_highgui"OPENCV_VERSION".lib")
#endif	//_DEBUG

using namespace cv;

void frameArriveCallback(letcv::ILetcvStereoCam *cam, void * userParam)
{
    printf("haha\n");
}

int main(int argc, char** argv)
{
    const char *kDepthWinName = "depth";
    const char *kIRWinName = "ir";
    const char *kConfigName = "app_config.txt";
    auto cam = letcv::CreateUsbCamera();
    auto ret = cam->Init(kConfigName);
    if (letcv::SCAM_SUCCESS != ret) {
        printf("got error code :%d\n", ret);
        letcv::ReleaseUsbCamera(&cam);
        return -1;
    }
    char k = 0;
    letcv::ImageBuffer buff_depth, buff_ir;
    std::vector<letcv::Point3f> pnts;
    cv::namedWindow(kIRWinName);
    cv::namedWindow(kDepthWinName);

    //cam->SetCallback(frameArriveCallback, NULL);

    while (k != 'q') {
        auto ret1 = cam->GetFrame(letcv::IMG_DEPTH, &buff_depth);
        auto ret2 = cam->GetFrame(letcv::IMG_LEFT_IR, &buff_ir);
        if (letcv::SCAM_SUCCESS == ret1 && letcv::SCAM_SUCCESS == ret2) {
            cv::Mat img(buff_depth.height, buff_depth.width, CV_16UC1, buff_depth.data);
            cv::Mat ir(buff_ir.height, buff_ir.width, CV_8UC1, buff_ir.data);
            double minDepth, maxDepth;
            cv::minMaxIdx(img, &minDepth, &maxDepth);
            printf("[%1.f, %.1f] ", minDepth, maxDepth);
            unsigned short vmin, vmax;
            vmin = 1000;
            vmax = 5000;
            img = (img - vmin) * 255 / (vmax - vmin);
            //img = 255 - img;
            img.convertTo(img, CV_8UC1);
            cv::minMaxIdx(img, &minDepth, &maxDepth);
            printf("[%1.f, %.1f]\n", minDepth, maxDepth);

            cv::imshow(kDepthWinName, img);
            cv::imshow(kIRWinName, ir);
            cam->NextFrame();
        }
        k = cv::waitKey(1);
    }//while
    letcv::ReleaseImageBuffer(&buff_depth);
    letcv::ReleaseUsbCamera(&cam);
    return 0;
}
