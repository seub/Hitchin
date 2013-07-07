#include "cp1point.h"

CP1Point::CP1Point()
{
}

CP1Point::CP1Point(const complex &z)
{
    z1 = z;
    z2 = 1.0;
}

CP1Point::CP1Point(const complex &z1, const complex &z2) : z1(z1), z2(z2)
{
}

bool CP1Point::isInfinity() const
{
    return z2 == 0.0;
}

complex CP1Point::getCCoordinate() const
{
    if (!isInfinity())
    {
        return z1 / z2;
    }
    else std::cout << "Infinity is not in C" << std::endl;
    throw(0);
}

bool operator==(const CP1Point & p1, const CP1Point & p2)
{
    complex test = p1.z1*p2.z2 - p1.z2*p2.z1;
    return test == 0.0;
}

std::ostream & operator<<(std::ostream & out, const CP1Point &p)
{
    if (p.z2 == 0.0)
    {
        out << "infinity";
    }
    else
    {
        out << p.z1 / p.z2 ;
    }
    return out;
}
