#include "opencv2/opencv.hpp"
#include <vector>

using namespace cv;
using namespace std;

class LaplacianBlending {
private:
  Mat_<Vec3f> left;
  Mat_<Vec3f> right;
  Mat_<float> blendMask;

  vector<Mat_<Vec3f> > leftLapPyr,rightLapPyr,resultLapPyr;
  Mat leftSmallestLevel, rightSmallestLevel, resultSmallestLevel;
  vector<Mat_<Vec3f> > maskGaussianPyramid; 

  int levels;


  void buildPyramids() {
    buildLaplacianPyramid(left,leftLapPyr,leftSmallestLevel);
    buildLaplacianPyramid(right,rightLapPyr,rightSmallestLevel);
    buildGaussianPyramid();
  }

  void buildGaussianPyramid() {
    assert(leftLapPyr.size()>0);

    maskGaussianPyramid.clear();
    Mat currentImg;
    cvtColor(blendMask, currentImg, CV_GRAY2BGR);
    maskGaussianPyramid.push_back(currentImg); //highest level

    currentImg = blendMask;
    for (int l=1; l<levels+1; l++) {
      Mat _down;
      if (leftLapPyr.size() > l) {
        pyrDown(currentImg, _down, leftLapPyr[l].size());
      } else {
        pyrDown(currentImg, _down, leftSmallestLevel.size()); //smallest level
      }

      Mat down;
      cvtColor(_down, down, CV_GRAY2BGR);
      maskGaussianPyramid.push_back(down);
      currentImg = _down;
    }
  }

  void buildLaplacianPyramid(const Mat& img, vector<Mat_<Vec3f> >& lapPyr, Mat& smallestLevel) {
    lapPyr.clear();
    Mat currentImg = img;
    for (int l=0; l<levels; l++) {
      Mat down,up;
      pyrDown(currentImg, down);
      pyrUp(down, up, currentImg.size());
      Mat lap = currentImg - up;
      lapPyr.push_back(lap);
      currentImg = down;
    }
    currentImg.copyTo(smallestLevel);
  }

  Mat_<Vec3f> reconstructImgFromLapPyramid() {
    Mat currentImg = resultSmallestLevel;
    for (int l=levels-1; l>=0; l--) {
      Mat up;

      pyrUp(currentImg, up, resultLapPyr[l].size());
      currentImg = up + resultLapPyr[l];
    }
    return currentImg;
  }

  void blendLapPyrs() {
    resultSmallestLevel = leftSmallestLevel.mul(maskGaussianPyramid.back()) +
    rightSmallestLevel.mul(Scalar(1.0,1.0,1.0) - maskGaussianPyramid.back());
    for (int l=0; l<levels; l++) {
      Mat A = leftLapPyr[l].mul(maskGaussianPyramid[l]);
      Mat antiMask = Scalar(1.0,1.0,1.0) - maskGaussianPyramid[l];
      Mat B = rightLapPyr[l].mul(antiMask);
      Mat_<Vec3f> blendedLevel = A + B;

      resultLapPyr.push_back(blendedLevel);
    }
  }

public:
  LaplacianBlending(const Mat_<Vec3f>& _left, const Mat_<Vec3f>& _right, const Mat_<float>& _blendMask, int _levels):
  left(_left),right(_right),blendMask(_blendMask),levels(_levels)
{
  assert(_left.size() == _right.size());
  assert(_left.size() == _blendMask.size());
  buildPyramids();
  blendLapPyrs();
};

Mat_<Vec3f> blend() {
  return reconstructImgFromLapPyramid();
}
};

Mat_<Vec3f> LaplacianBlend(const Mat_<Vec3f>& l, const Mat_<Vec3f>& r, const Mat_<float>& m) {
  LaplacianBlending lb(l,r,m,4);
  return lb.blend();
}


int main(int argc, char** argv) {
  Mat left = imread("apple.jpg");
  Mat right = imread("orange.jpg");
  
  imshow("left",left);
  imshow("right",right);

  Mat_<Vec3f> l; left.convertTo(l,CV_32F,1.0/255.0);
  Mat_<Vec3f> r; right.convertTo(r,CV_32F,1.0/255.0);

  Mat_<float> m(l.rows,l.cols,0.0);
  m(Range::all(),Range(0,m.cols/2)) = 1.0;

  Mat_<Vec3f> blend = LaplacianBlend(l, r, m);
  imshow("blended",blend);
  waitKey(0);
}
