## delaunay三角网格化

**基本流程**
1. 生成一个包含所有点的大三角形
2. 在已Delaunay三角化的网格中加入一点P，只需要删除所有外接圆包含此点的三角形，并连接P与所有可见的点（即连接后不会与其他边相交），则形成的网格仍然满足Delaunay三角剖分的条件。
3. 删除所有与大三角形相关的边。

第2条，删除所有外接圆包含此点的三角形，连接P与所有可见的点，也可以改为[删除外接圆包含此点的三角的公共边，并连接P与所有其他的非公共边](https://github.com/Bl4ckb0ne/delaunay-triangulation)


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
![凹包三角网格](https://github.com/palfu/algorithm/blob/master/delaunayPlot0.png)

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
![预期输出](https://github.com/palfu/algorithm/blob/master/delaunayPlot.png)


