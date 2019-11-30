#include <vector>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

/*
1. 生成一个包含所有点的大三角形
2. 在已Delaunay三角化的网格中加入一点P，只需要删除所有外接圆包含此点的三角形，并连接P与所有可见的点（即连接后不会与其他边相交），则形成的网格仍然满足Delaunay三角剖分的条件。
3. 删除所有与大三角形相关的边。
*/

typedef struct CircumCircle
{
	Point2f center;
	float radius;
}CCircle;

typedef struct DenaulayEdge
{
	DenaulayEdge(){}
	DenaulayEdge(Point _p1, Point _p2) :p1(_p1), p2(_p2){}
	Point p1, p2;

	float calcAngle()
	{
#define MPI 3.1415926535f
		float theta;
		int x = p2.x - p1.x;
		int y = p2.y - p1.y;
		
		if (y == 0 && x > 0)
			theta = 0.0f;

		if (y == 0 && x < 0)
			theta = MPI;

		if (x==0)
			theta = MPI / 2;
		else
			theta = atan(1.0f*y/x);
		if (theta < 0.0f)
			theta += MPI;
		
		if (y < 0)
			theta += MPI;
		return theta;
	}

	// 判断端点是否一致
	bool isSame(DenaulayEdge de)
	{
		if ((de.p1 == p1 && de.p2 == p2) || (de.p1 == p2 || de.p2 == p1))
			return true;
		else
			return false;
	}
	
	// 计算线段所在的直线
	void calcLine(float &a, float &b, float& c)
	{
		if (p1.x == p2.x)
		{
			a = 1; b = 0; c = -p1.x;
			return;
		}
		b = -1.0f;
		a = 1.0f*(p1.y - p2.y) / (p1.x - p2.x);
		c = p1.y - a*p1.x;
	}

	void calcNormal(float& a, float& b, float& c)
	{
		float midx = (p1.x + p2.x) / 2;
		float midy = (p1.y + p2.y) / 2;

		if (p1.y == p2.y)
		{
			a = 1; b = 0; c = -midx;
			return;
		}
		
		b = -1.0;
		a = -1.0f*(p1.x - p2.x) / (p1.y - p2.y);
		c = midy - a*midx;
	}

	// 判断线段是否相交，不包括端点判断
	bool cross(DenaulayEdge de)
	{
		float a1, a2, b1, b2, c1, c2;
		calcLine(a1, b1, c1);
		de.calcLine(a2, b2, c2);
		float cx = (b1*c2 - b2*c1) / (a1*b2 - a2*b1);
		//float cy = (-a1*c2 + a2*c1) / (a1*b2 - a2*b1);

		float x1 = min(p1.x, p2.x);
		float x2 = max(p1.x, p2.x);
		float x3 = min(de.p1.x, de.p2.x);
		float x4 = max(de.p1.x, de.p2.x);

		if (x1 < cx && cx < x2 && x3 < cx && cx < x4)
			return true;
		else
			return false;
	}
}DEdeg;

typedef struct DenaulayTriangle
{
	Point p1, p2, p3;
	CCircle cc;

	// 生成三角形时，直接计算外接圆
	DenaulayTriangle(){ cc.radius = -1.0; };

	DenaulayTriangle(Point _p1, Point _p2, Point _p3) :p1(_p1), p2(_p2), p3(_p3){
		cc = calcCircumCircle();
	}

	// 判断点是否是顶点
	bool isVertex(Point pt)
	{
		if (pt == p1 || pt == p2 || pt == p3)
			return true;
		else
			return false;
	}
	
	// 判断点是否在外接圆内
	bool PointInCircumCircle(Point p)
	{
		if (cc.radius<0)
			calcCircumCircle();
		float dis = sqrt(pow(p.x - cc.center.x, 2) + pow(p.y - cc.center.y, 2));
		if (dis < cc.radius)
			return true;
		else
			return false;
	}

	// 外接圆内的点，产生新的两个三角形
	vector<DenaulayTriangle> genNewDTri(Point p)
	{
		vector<DenaulayTriangle> vecAdd;
		vector<DenaulayTriangle> vecDel;
		
		DenaulayTriangle dt23(p2, p3, p);
		DEdeg pp1(p, p1);
		if (!pp1.cross(DEdeg(p2, p3)))
			vecAdd.push_back(dt23);
		else
			vecDel.push_back(dt23);
		
		DEdeg pp2(p, p2);
		DenaulayTriangle dt13(p1, p3, p);
		if (!pp2.cross(DEdeg(p1, p3)))
			vecAdd.push_back(dt13);
		else
			vecDel.push_back(dt13);

		DEdeg pp3(p, p3);
		DenaulayTriangle dt12(p2, p1, p);
		if (!pp3.cross(DEdeg(p2, p1)))
			vecAdd.push_back(dt12);
		else
			vecDel.push_back(dt12);

		return vecAdd;
	}

private:
	
	// 计算点的外接圆
	CCircle calcCircumCircle()
	{
		DEdeg de1(p1, p2);
		DEdeg de2(p2, p3);
		
		float a1, a2, b1, b2, c1, c2;
		de1.calcNormal(a1, b1, c1);
		de2.calcNormal(a2, b2, c2);

		CCircle cc;
		cc.center.x = (b1*c2 - b2*c1) / (a1*b2 - a2*b1);
		cc.center.y = (a2*c1 - a1*c2) / (a1*b2 - a2*b1);

		cc.radius = sqrt(pow(cc.center.x - p1.x, 2) + pow(cc.center.y - p1.y, 2));

		return cc;
	}

} DTri;

Rect caclBoundingRect(vector<Point>& vecSrc, int padding=10)
{
	Point pt = vecSrc[0];
	int minx, maxx, miny, maxy;
	minx = maxx = pt.x;
	miny = maxy = pt.y;

	int sz = vecSrc.size();
	for (int i = 1; i < sz; i++)
	{
		minx = min(vecSrc[i].x, minx);
		miny = min(vecSrc[i].y, miny);
		maxx = max(vecSrc[i].x, maxx);
		maxy = max(vecSrc[i].y, maxy);
	}

	minx -= padding;
	miny -= padding;
	maxx += padding;
	maxy += padding;
	Rect rc(Point(minx, miny), Point(maxx, maxy));
	return rc;
}

DTri calcBoundingDTri(vector<Point>& vecSrc, int padding=10)
{
	Rect rc = caclBoundingRect(vecSrc, padding);
	int maxDelta = max(rc.width, rc.height);

	Point p1 = Point(rc.x-20*maxDelta, rc.y-maxDelta);
	Point p2 = Point(rc.x+20*maxDelta, rc.y-maxDelta);
	Point p3 = Point(rc.x+rc.width/2, rc.y+20*maxDelta);
	DTri dt(p1, p2, p3);
	return dt;
}

void plotDTri(string filename, vector<DTri>& vecDTri, Rect rc)
{
	Mat matDst = Mat(rc.height, rc.width, CV_8UC3);
	for (auto dt : vecDTri)
	{
		line(matDst, Point(dt.p1.x - rc.x, dt.p1.y - rc.y), Point(dt.p2.x - rc.x, dt.p2.y - rc.y), Scalar(0, 0, 255));
		line(matDst, Point(dt.p1.x - rc.x, dt.p1.y - rc.y), Point(dt.p3.x - rc.x, dt.p3.y - rc.y), Scalar(0, 0, 255));
		line(matDst, Point(dt.p3.x - rc.x, dt.p3.y - rc.y), Point(dt.p2.x - rc.x, dt.p2.y - rc.y), Scalar(0, 0, 255));
	}
	imwrite(filename, matDst);
}

bool smallDEdge(DEdeg de1, DEdeg de2)
{
	float theta1 = de1.calcAngle();
	float theta2 = de2.calcAngle();
	return theta1 < theta2;
}

vector<DTri> genNewTriAccordingToVec(vector<Point>& vecPt, Point pt)
{
	if (vecPt.empty())
		return vector<DTri>();
	vector<DEdeg> vecDe;
	for (auto p : vecPt)
		vecDe.push_back(DEdeg(pt, p));

	sort(vecDe.begin(), vecDe.end(), smallDEdge);

	vector<DTri> vecDTri;
	int sz = vecDe.size();
	for (int i = 1; i < sz; i++)
	{
		DTri dt(vecDe[i - 1].p2, vecDe[i].p2, pt);
		vecDTri.push_back(dt);
	}
	vecDTri.push_back(DTri(vecDe[sz - 1].p2, vecDe[0].p2, pt));
	return vecDTri;
}

bool Point2Vec(vector<Point>& vecPt, Point p)
{
	for (auto pt:vecPt)
	{
		if (pt.x==p.x && pt.y==p.y)
			return true;
	}
	return false;
}

vector<DTri> delaunayTest(vector<Point>& vecSrc)
{
	DTri boundingDTri = calcBoundingDTri(vecSrc);
	Rect rc = caclBoundingRect(vecSrc);
	rc.width += rc.width;
	rc.height += rc.height;
	vector<DTri> vecDTri = { boundingDTri };
	int index = 0;
	for (auto pt:vecSrc)
	{
		// 计算外接圆包含pt的三角形
		vector<int> vecDelDtriIndex;
		vector<DTri> vecNewDtri;
		vector<Point> vecPt;
		for (int i = 0; i < vecDTri.size(); i++)
		{
			if (vecDTri[i].PointInCircumCircle(pt))
			{
				vecDelDtriIndex.push_back(i);
				if (!Point2Vec(vecPt, vecDTri[i].p1))
					vecPt.push_back(vecDTri[i].p1);
				if (!Point2Vec(vecPt, vecDTri[i].p2))
					vecPt.push_back(vecDTri[i].p2);
				if (!Point2Vec(vecPt, vecDTri[i].p3))
					vecPt.push_back(vecDTri[i].p3);
			}
		}

		// 删除外接圆包含pt的三角形
		std::sort(vecDelDtriIndex.begin(), vecDelDtriIndex.end());
		auto bgIter = vecDTri.begin();
		for (int i = vecDelDtriIndex.size() - 1; i >= 0; i--)
			vecDTri.erase(bgIter + vecDelDtriIndex[i]);

		// 插入重新生成的三角形
		vecNewDtri = genNewTriAccordingToVec(vecPt, pt);
		vecDTri.insert(vecDTri.end(), vecNewDtri.begin(), vecNewDtri.end());

		//plotDTri("delaunayTri_" + to_string(index++) + ".png", vecDTri, rc);
	}

	// 删除包含BoundingDTri顶点的三角形
	vector<int> vecIndex;
	int sz = vecDTri.size();
	for (int i = 0; i < sz; i++)
		if (vecDTri[i].isVertex(boundingDTri.p1) || vecDTri[i].isVertex(boundingDTri.p2) || vecDTri[i].isVertex(boundingDTri.p3))
			vecIndex.push_back(i);

	sort(vecIndex.begin(), vecIndex.end());
	sz = vecIndex.size();
	auto bgIter = vecDTri.begin();
	for (int i = sz - 1; i >= 0; i--)
		vecDTri.erase(bgIter + vecIndex[i]);

	return vecDTri;
}


vector<Point> initPoints()
{
	vector<Point> vecRes = { Point(100, 2), Point(39, 79), Point(49, 19), Point(11, 18), Point(23, 69), Point(8, 45), Point(111, 233), Point(121, 72), Point(8, 29) };
	return vecRes;
}

// void test()
// {
// 	Point p1(5, 77);
// 	Point p3(445, 277);
// 	Point p2(666, 7);
// 	DTri dt(p1, p2, p3);

// 	int w = 1000, h = 1000;
// 	Mat matDst = Mat(h, w, CV_8UC3);
// 	Point rc(0, 0);
// 	line(matDst, Point(dt.p1.x - rc.x, dt.p1.y - rc.y), Point(dt.p2.x - rc.x, dt.p2.y - rc.y), Scalar(0, 0, 255), 3);
// 	line(matDst, Point(dt.p1.x - rc.x, dt.p1.y - rc.y), Point(dt.p3.x - rc.x, dt.p3.y - rc.y), Scalar(0, 0, 255), 3);
// 	line(matDst, Point(dt.p3.x - rc.x, dt.p3.y - rc.y), Point(dt.p2.x - rc.x, dt.p2.y - rc.y), Scalar(0, 0, 255), 3);

// 	DEdeg de1(p3, p2);
// 	float a, b, c;
// 	de1.calcNormal(a, b, c);
// 	int wy = (a*w + c) / (-b);
// 	int wy0 = (a*0 + c) / (-b);
// 	line(matDst, Point(w, wy), Point(0, wy0), Scalar(0, 255, 0), 3);

// 	circle(matDst, dt.cc.center, dt.cc.radius, Scalar(255, 0, 0), 1);
// 	imwrite("test.png", matDst);
// }



int main(int argc, char** argv)
{

	vector<Point> vecSrc = initPoints();
	Rect rc = caclBoundingRect(vecSrc);

	vector<DTri> vecDTri = delaunayTest(vecSrc);
	Mat matSrc = Mat(rc.height, rc.width, CV_8UC3);
	for (auto pt:vecSrc)
	{
		circle(matSrc, Point(pt.x - rc.x, pt.y - rc.y), 2, Scalar(0, 0, 255), 2);
	}
	imwrite("delaunaySrc.png", matSrc);

	Mat matDst = Mat(rc.height, rc.width, CV_8UC3);
	for (auto dt:vecDTri)
	{
		line(matDst, Point(dt.p1.x - rc.x, dt.p1.y - rc.y), Point(dt.p2.x - rc.x, dt.p2.y - rc.y), Scalar(0, 0, 255));
		line(matDst, Point(dt.p1.x - rc.x, dt.p1.y - rc.y), Point(dt.p3.x - rc.x, dt.p3.y - rc.y), Scalar(0, 0, 255));
		line(matDst, Point(dt.p3.x - rc.x, dt.p3.y - rc.y), Point(dt.p2.x - rc.x, dt.p2.y - rc.y), Scalar(0, 0, 255));
	}
	imwrite("delaunayPlot.png", matDst);
	return 0;
}