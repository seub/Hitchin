#include "h2canvasdelegate.h"

#include <QPen>

#include "h2point.h"
#include "h2geodesic.h"
#include "circle.h"
#include "planarline.h"

H2CanvasDelegate::H2CanvasDelegate(Canvas *canvas) : CanvasDelegate(canvas)
{
    //std::cout << "Entering H2CanvasDelegate::CanvasDelegate" << std::endl;
    drawCircle(0, 1);
    //std::cout << "Leaving H2CanvasDelegate::CanvasDelegate" << std::endl;
}

void H2CanvasDelegate::drawH2Point(const H2Point &p, const QColor &color, int width)
{
    complex z = p.getDiskCoordinate();
    drawPoint(z, color, width);
    return;
}

void H2CanvasDelegate::drawH2Geodesic(const H2Geodesic &L, const QColor &color, int width)
{
    if (L.isCircleInDiskModel())
    {
        Circle C;
        double angle1, angle2;
        L.getCircleAndAnglesInDiskModel(C, angle1, angle2);
        drawArc(C, angle1, angle2, color, width);
    }
    else
    {
        complex z1, z2;
        L.getEndpointsInDiskModel(z1, z2);
        drawSegment(z1, z2, color, width);
    }
    return;
}

void H2CanvasDelegate::drawH2GeodesicArc(const H2GeodesicArc &L, const QColor &color, int width)
{
    pen->setWidth(2);
    if (L.isCircleArcInDisModel())
    {
        Circle C;
        double angle1, angle2;
        L.getCircleAndAnglesInDiskModel(C, angle1, angle2);
        drawArc(C, angle1, angle2, color, width);
    }
    else
    {
        complex z1, z2;
        L.getEndpointsInDiskModel(z1, z2);
        drawSegment(z1, z2, color, width);
    }
    return;
}

void H2CanvasDelegate::drawExample()
{
    H2Point p1, p2, p3;
    p1.setDiskCoordinate(0.3*I);
    p2.setDiskCoordinate(-.8*I);
    p3.setDiskCoordinate(.5);

    H2Geodesic L1(p1, p2), L2(p1, p3), L3(p2, p3);
    drawH2Geodesic(L1, "blue");
    drawH2Geodesic(L2, "blue");
    drawH2Geodesic(L3, "blue");

    H2GeodesicArc A1(p1, p2), A2(p1, p3), A3(p2, p3);
    drawH2GeodesicArc(A1, "red");
    drawH2GeodesicArc(A2, "red");
    drawH2GeodesicArc(A3, "red");

    drawH2Point(p1);
    drawH2Point(p2);
    drawH2Point(p3);
    return;
}