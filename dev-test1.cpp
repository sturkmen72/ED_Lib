#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/ximgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    const char* keys =
    {
        "{lc    |9999 | loop count}"
        "{pf    |false| indicates if params.PFmode will be true or false}"
        "{uc    |false| indicates if color image will be used}"
    };

    CommandLineParser parser( argc, argv, keys );

    int loop_count = parser.get< int >( "lc" );
    bool PFmode = parser.get< bool >( "pf" );
    bool use_color = parser.get< bool >( "uc" );

    cv::Ptr<cv::ximgproc::EdgeDrawing> ed = cv::ximgproc::createEdgeDrawing();
    ed->params.PFmode = PFmode;
    Mat img(500, 500, use_color ? CV_8UC3 : CV_8UC1, Scalar::all(0));
    RNG& rng = theRNG();
    TickMeter tm;
    tm.start();

    for( int j = 0; j < loop_count; j++ )
    {
        int i, count = rng.uniform(1, 20);
        vector<Point> points;

        // Generate a random set of points
        for( i = 0; i < count+5; i++ )
        {
            Point pt;
            pt.x = rng.uniform(img.cols/4, img.cols*3/4);
            pt.y = rng.uniform(img.rows/4, img.rows*3/4);

            points.push_back(pt);
        }

        // Find the minimum area enclosing bounding box
        Point2f vtx[4];
        RotatedRect box = minAreaRect(points);
        box.points(vtx);

        // Find the minimum area enclosing triangle
        vector<Point2f> triangle;
        minEnclosingTriangle(points, triangle);

        // Find the minimum area enclosing circle
        Point2f center;
        float radius = 0;
        minEnclosingCircle(points, center, radius);

        img = Scalar::all(0);

        // Draw the circle
        circle(img, center, cvRound(radius+4), Scalar(255, 255, 255), 1, LINE_AA);
        /*
        circle(img, center, cvRound(radius / 2), Scalar(0, 255, 255), 1, LINE_AA);
        circle(img, center, cvRound(radius / 3), Scalar(0, 255, 255), 1, LINE_AA);
        circle(img, center, cvRound(radius / 4), Scalar(0, 255, 255), 1, LINE_AA);
        circle(img, center, cvRound(radius / 7), Scalar(0, 255, 255), 1, LINE_AA);
        circle(img, center, cvRound(radius / 9), Scalar(0, 255, 255), 1, LINE_AA);
        */

        ed->detectEdges(img);
        std::vector<cv::Vec6d> ellipses;
        ed->detectEllipses(ellipses);

        if (ellipses.size()<1)
            cout << j << "    " << center << "  " << cvRound(radius) << endl;
        for (const auto& ellipse : ellipses)
        {
            cv::Point icenter((int)ellipse[0], (int)ellipse[1]);
            center = icenter;
            cv::Size axes((int)ellipse[2] + (int)ellipse[3], (int)ellipse[2] + (int)ellipse[4]);
            double angle(ellipse[5]);
            cv::Scalar color = (ellipse[2] == 0) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
            cv::ellipse(img, center, axes, angle, 0, 360, color, 2, cv::LINE_AA);
        }
        /*
        imshow( "Rectangle, triangle & circle", img );

        char key = (char)waitKey(1);
        if( key == 27 || key == 'q' || key == 'Q' ) // 'ESC'
            break;
        */
    }

    tm.stop();
    cout << tm << endl;
    return 0;
}
