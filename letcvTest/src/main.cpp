#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>

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

char* filenames[] =
{
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_480_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_483_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_486_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_489_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_492_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_495_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_498_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_501_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_504_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_507_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_510_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_513_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_516_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_519_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_522_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_531_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_534_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_537_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_540_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_552_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_555_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_558_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_561_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_567_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_570_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_573_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_576_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_579_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_582_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_585_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_588_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_612_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_615_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_618_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_621_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_624_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_627_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_630_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_633_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_636_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_639_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_642_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_645_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_648_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_651_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_654_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_657_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_660_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_663_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_666_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_669_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_672_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_675_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_678_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_681_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_690_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_693_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_696_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_702_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_705_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_708_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_711_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_717_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_720_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_723_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_726_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_729_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_732_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_735_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_738_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_741_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_744_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_747_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_750_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_753_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_756_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_759_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_762_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_765_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_768_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_771_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_774_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_777_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_780_16u_disp.bin",
    "c:/Users/vincentz/Documents/InfiniTAM/media/box/depth_783_16u_disp.bin",
};

using namespace cv;

void LoadData(const char *filename, Mat &image)
{
    const int tsize = sizeof(unsigned short);
    std::ifstream ifs(filename, std::ios::binary);
    unsigned short w, h;
    ifs.read((char*)&w, tsize);
    ifs.read((char*)&h, tsize);
    image.create(h, w, CV_16SC1);
    ifs.read((char*)image.data, h * w * tsize);
    ifs.close();
}

int main(int argc, char** argv )
{
    for (auto filename : filenames)
    {
        Mat frame;
        LoadData(filename, frame);
        namedWindow("test");
        imshow("test", frame);
        if (waitKey(-1) == 27)
        {
            break;
        }
    }
	
    return 0;
}
