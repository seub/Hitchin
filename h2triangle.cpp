#include "h2triangle.h"
#include "h2polygon.h"
#include "h2isometry.h"

H2Triangle::H2Triangle()
{
}

H2Triangle::H2Triangle(const H2Point &a, const H2Point &b, const H2Point &c): a(a), b(b), c(c)
{
}

void H2Triangle::getPoints(H2Point &a, H2Point &b, H2Point &c) const
{
    a = this->a;
    b = this->b;
    c = this->c;
}

std::vector<H2Point> H2Triangle::getPoints() const
{
    return {a, b, c};
}

std::vector<H2GeodesicArc> H2Triangle::getSides() const
{
    return {H2GeodesicArc(a, b), H2GeodesicArc(b, c), H2GeodesicArc(c, a)};
}

double H2Triangle::area() const
{    
    Complex zA = a.getDiskCoordinate(), zB = b.getDiskCoordinate(), zC = c.getDiskCoordinate();
    Complex ZA, ZB, ZC;

    ZB = (zB - zA)/(1.0 - conj(zA)*zB);
    ZC = (zC - zA)/(1.0 - conj(zA)*zC);
    double angleA = std::abs(std::arg(ZC*conj(ZB)));

    ZC = (zC - zB)/(1.0 - conj(zB)*zC);
    ZA = (zA - zB)/(1.0 - conj(zB)*zA);
    double angleB = std::abs(std::arg(ZA*conj(ZC)));

    ZA = (zA - zC)/(1.0 - conj(zC)*zA);
    ZB = (zB - zC)/(1.0 - conj(zC)*zB);
    double angleC = std::abs(std::arg(ZB*conj(ZA)));

    return M_PI - (angleA + angleB + angleC);
}

void H2Triangle::getSideLengths(double &A, double &B, double &C) const
{
    A = H2Point::distance(b,c);
    B = H2Point::distance(a,c);
    C = H2Point::distance(a,b);
}

bool H2Triangle::contains(const H2Point &point) const
{
    H2Polygon P;
    getAsPolygon(P);
    return P.contains(point);
}

void H2Triangle::getAsPolygon(H2Polygon & outputPolygon) const
{
    outputPolygon.setVertices({a, b, c});
}

void H2Triangle::getAngles(double &angleA, double &angleB, double &angleC) const
{
    Complex z1 = a.getDiskCoordinate();
    Complex z2 = b.getDiskCoordinate();
    Complex z3 = c.getDiskCoordinate();

    Complex u,v;

    u = (z3 - z1) / (1.0 - conj(z1)*z3);
    v = (z2 - z1) / (1.0 - conj(z1)*z2);
    angleA = Tools::mod2Pi(arg(v*conj(u)));

    u = (z1 - z2) / (1.0 - conj(z2)*z1);
    v = (z3 - z2) / (1.0 - conj(z2)*z3);
    angleB = Tools::mod2Pi(arg(v*conj(u)));

    u = (z2 - z3) / (1.0 - conj(z3)*z2);
    v = (z1 - z3) / (1.0 - conj(z3)*z1);
    angleC = Tools::mod2Pi(arg(v*conj(u)));
}

bool H2Triangle::isVertexCloseInDiskModel(const H2Point &point, double detectionRadiusSquared, uint &vertexIndex) const
{
    bool res = false;

    Complex z = point.getDiskCoordinate();
    double min = 0;
    double A = norm(z - a.getDiskCoordinate());
    double B = norm(z - b.getDiskCoordinate());
    double C = norm(z - c.getDiskCoordinate());

    if (A < detectionRadiusSquared)
    {
        min = A;
        vertexIndex = 0;
        res = true;
    }
    if (B < detectionRadiusSquared)
    {
        if (res)
        {
            if (B<min)
            {
                min = B;
                vertexIndex = 1;
            }
        }
        else
        {
            min = B;
            vertexIndex = 1;
            res = true;
        }

    }
    if (C < detectionRadiusSquared)
    {
        if (res)
        {
            if (C<min)
            {
                min = C;
                vertexIndex = 2;
            }
        }
        else
        {
            min = C;
            vertexIndex = 2;
            res = true;
        }
    }
    return res;
}

H2Point H2Triangle::getVertex(uint index) const
{
    switch (index)
    {
    case 0:
        return a;
        break;
    case 1:
        return b;
        break;
    case 2:
        return c;
        break;
    default:
        std::cerr << "ERROR in H2Triangle::getVertex: wrong index" << std::endl;
        return a;
        break;
    }
}

void H2Triangle::getVertices(H2Point &Aout, H2Point &Bout, H2Point &Cout) const
{
    Aout = a;
    Bout = b;
    Cout = c;
}



std::ostream & operator<<(std::ostream &out, const H2Triangle &T)
{
    out << "Triangle with vertices a = " << T.a << ", b = " << T.b << ", c = " << T.c << std::endl;
    return out;
}

H2Triangle operator*(const H2Isometry &f, const H2Triangle &T)
{
    return H2Triangle(f*(T.a), f*(T.b), f*(T.c));
}
