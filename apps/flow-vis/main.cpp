#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>

using namespace cv;
using namespace std;

const float FLOW_TAG_FLOAT = 202021.25f;
const char *FLOW_TAG_STRING = "PIEH";

Mat readOpticalFlow( const String& path )
{
//    CV_Assert(sizeof(float) == 4);
    //FIXME: ensure right sizes of int and float - here and in writeOpticalFlow()

    Mat_<Point2f> flow;
    std::ifstream file(path.c_str(), std::ios_base::binary);
    if ( !file.good() )
        return flow; // no file - return empty matrix

    float tag;
    file.read((char*) &tag, sizeof(float));
    if ( tag != FLOW_TAG_FLOAT )
        return flow;

    int width, height;

    file.read((char*) &width, 4);
    file.read((char*) &height, 4);

    flow.create(height, width);

    for ( int i = 0; i < flow.rows; ++i )
    {
        for ( int j = 0; j < flow.cols; ++j )
        {
            Point2f u;
            file.read((char*) &u.x, sizeof(float));
            file.read((char*) &u.y, sizeof(float));
            if ( !file.good() )
            {
                flow.release();
                return flow;
            }

            flow(i, j) = u;
        }
    }
    file.close();
    return flow;
}

int main()
{
    Mat_<Point2f> flow = readOpticalFlow("../media/frame_0001.flo");
    Mat_<Vec3b> canvas(flow.rows, flow.cols);
    int step = 16;
    for (int i = 0; i < flow.rows; i += step){
        for (int j = 0; j < flow.cols; j += step){
            const Point2f& u = flow(i, j);
            cv::line(canvas, { j, i }, { int(j + u.x), int(i + u.y) }, CV_RGB(255, 0, 0));
        }
    }
    imshow("flow", canvas);
    waitKey();

    return 0;
}