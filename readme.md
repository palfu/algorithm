## delaunay三角网格化
delaunay三角网格，增量法计算。注意，包络三角形如果靠近内部点，会产生凹包。
```
DTri calcBoundingDTri(vector<Point>& vecSrc, int padding=10)
{
	Rect rc = caclBoundingRect(vecSrc, padding);

	Point p1 = Point(rc.x, rc.y);
	Point p2 = Point(rc.x+rc.width*2, rc.y);
	Point p3 = Point(rc.x, rc.y+2*rc.height);
	DTri dt(p1, p2, p3);
	return dt;
}
```
凹包三角网格

```
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

```
预期输出


