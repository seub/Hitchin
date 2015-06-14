#include "h2isometry.h"
#include "h2point.h"
#include "sl2rmatrix.h"


H2Isometry::H2Isometry()
{
}

H2Isometry::H2Isometry(int i)
{
    assert(i==1);
    setIdentity();
}

H2Isometry::H2Isometry(const SL2RMatrix &A)
{
    setSL2Rmatrix(A);
}

void H2Isometry::setIdentity()
{
    u = 1.0;
    a = 0.0;
}

void H2Isometry::getDiskCoordinates(Complex &u, Complex &a) const
{
    u = this->u;
    a = this->a;
}

void H2Isometry::setDiskCoordinates(const Complex &u, const Complex &a)
{
    this->u=u;
    this->a=a;
}

void H2Isometry::setTranslationAxisAndLength(const H2Geodesic & axis, double length)
{
    H2Isometry f0;
    f0.setTranslationLengthNormalized(length);
    H2Isometry f;
    f.setByMappingToVerticalUp(axis);
    *this = f.inverse()*f0*f;
}

void H2Isometry::setTranslationLengthNormalized(double length)
{
    double L = cosh(length);
    a = -I*sqrt((L - 1.0)/(L + 1.0));
    if (length < 0)
    {
        a = -a;
    }
    u = 1.0;
}

void H2Isometry::setVerticalTranslation(double t)
{
    a = - I*t;
    u = 1.0;
}

void H2Isometry::setVerticalTranslation(double t1, double t2)
{
    double t = (t1 + t2)/(1 + t1 * t2);

    a = - I*t;
    u = 1.0;
}

void H2Isometry::setVerticalTranslation(double t1, double t2, double t3)
{
    double t = (t1 + t2 + t3 + t1*t2*t3) / (1 + t1*t2 + t1*t3 + t2*t3);

    a = - I*t;
    u = 1.0;
}

void H2Isometry::setByMappingPointInDiskModelNormalized(const Complex &zIn, const Complex &zOut)
{
    double n1 = norm(zIn), n2 = norm(zOut);
    u = 1.0;
    a = (zIn*(1 - n2) - zOut*(1 - n1)) / (1 - n1*n2);
}

void H2Isometry::setByMappingGeodesic(const H2Geodesic &L1, const H2Geodesic &L2)
{
    H2Isometry f1,f2;
    f1.setByMappingToVerticalUp(L1);
    f2.setByMappingToVerticalUp(L2);
    *this = f2.inverse()*f1;
}

void H2Isometry::setByMappingPointTo0(const H2Point &p)
{
    u = 1.0;
    a = p.getDiskCoordinate();
}

SL2CMatrix H2Isometry::getSU11Matrix() const
{
    double s = 1.0 / sqrt(1.0 - norm(a));
    Complex t = sqrt(u);
    return SL2CMatrix( s*t , -a*s*t , -conj(a)*s/t , s/t );
}

SL2RMatrix H2Isometry::getSL2RMatrix() const
{
    SL2CMatrix A(1.0/sqrt(2.0),-I/sqrt(2.0),-I/sqrt(2.0),1.0/sqrt(2.0));
    SL2CMatrix out = A*getSU11Matrix()*A.inverse();
    SL2RMatrix output;
    out.getRealPart(output);
    return output;
}

void H2Isometry::setSU11Matrix(const SL2CMatrix &A)
{
    Complex alpha,alphaBar,beta,betaBar;
    A.getCoefficients(alpha,alphaBar,beta,betaBar);
    u = alpha / alphaBar;
    a = -beta / alpha;
}

void H2Isometry::setSL2Rmatrix(const SL2RMatrix &A)
{
    double Aa,Ab,Ac,Ad;
    A.getCoefficients(Aa,Ab,Ac,Ad);
    u = (Aa + Ad + I*(Ab - Ac))/(Aa + Ad - I*(Ab - Ac));
    a = -(Ab + Ac + I*(Aa - Ad))/(Aa + Ad + I*(Ab - Ac));
}

bool H2Isometry::isElliptic() const
{
    return (traceSquared()<4.0);
}

bool H2Isometry::isParabolic() const
{
    return (traceSquared()==4.0);
}

bool H2Isometry::isHyperbolic() const
{
    return (traceSquared()>4.0);
}

double H2Isometry::traceSquared() const
{
    return 2.0*(1.0 + real(u))/(1.0 - norm(a));
}

double H2Isometry::translationLength() const
{
    return acosh(.5*traceSquared() - 1.0);
}

double H2Isometry::error() const
{
    return norm(a) + norm(u - 1.0);
}

H2Isometry H2Isometry::inverse() const
{
    H2Isometry fOut;
    fOut.setDiskCoordinates(conj(u),-u*a);
    return fOut;
}

void H2Isometry::fixedPointsInDiskModel(CP1Point &c1, CP1Point &c2) const
{
    if (a == 0.0)
    {
        c1 = CP1Point(0.0,1.0);
        c2 = CP1Point(1.0,0.0);
        return;
    }
    Complex D = sqrt((u - 1.0)*(u - 1.0) + 4.0*norm(a)*u);
    c1 = CP1Point((1.0 - u + D),(2.0*conj(a)));
    c2 = CP1Point((1.0 - u - D),(2.0*conj(a)));
}

bool H2Isometry::axis(H2Geodesic & L) const
{
    if (!isHyperbolic())
    {
        return false;
    }
    else
    {
        Complex c1,c2;
        CP1Point C1,C2;
        fixedPointsInDiskModel(C1,C2);
        c1 = C1.getComplexCoordinate();
        c2 = C2.getComplexCoordinate();
        if(norm(1.0 - conj(a)*c2) < norm(1.0 - conj(a)*c1))
        {
            L.setEndpointsInDiskModel(c2,c1);
            return true;
        }
        else
        {
            L.setEndpointsInDiskModel(c1,c2);
            return true;
        }
    }
}

Complex H2Isometry::hitComplexInDiskModel(const Complex &z)
{
    return u*(z - a)/(1.0 - conj(a)*z);
}

H2Isometry operator *(const H2Isometry &f1, const H2Isometry &f2)
{
    H2Isometry fOut;
    Complex temp = 1.0 + f1.a*conj(f2.u*f2.a);
    fOut.setDiskCoordinates(f1.u*f2.u*(temp*temp)/norm(temp), (f2.a + (f1.a*conj(f2.u)))/temp);
    return fOut;
}

H2Point operator *(const H2Isometry & f, const H2Point & p)
{
    Complex z = p.getDiskCoordinate();
    H2Point pOut;
    pOut.setDiskCoordinate(f.u*((z-f.a)/(1.0 - (conj(f.a)*z))));
    return pOut;
}

H2Polygon operator*(const H2Isometry &f, const H2Polygon &P)
{
    H2Polygon res;
    std::vector<H2Point> vertices = P.getVertices();
    for(unsigned int i=0; i<vertices.size(); i++)
    {
        res.addVertex(f*vertices[i]);
    }
    return res;
}


H2Geodesic operator*(const H2Isometry &f, const H2Geodesic &L)
{
    Complex z1,z2,w1,w2;
    L.getEndpointsInDiskModel(z1,z2);
    w1 = f.u*((z1-f.a)/(1.0 - (conj(f.a)*z1)));
    w2 = f.u*((z2-f.a)/(1.0 - (conj(f.a)*z2)));

    H2Geodesic res;
    res.setEndpointsInDiskModel(w1, w2);
    return res;
}

H2GeodesicArc operator*(const H2Isometry &f, const H2GeodesicArc &L)
{
    Complex z1,z2,w1,w2;
    L.getEndpointsInDiskModel(z1,z2);
    w1 = f.u*((z1-f.a)/(1.0 - (conj(f.a)*z1)));
    w2 = f.u*((z2-f.a)/(1.0 - (conj(f.a)*z2)));
    H2Point p1, p2;
    p1.setDiskCoordinate(w1);
    p2.setDiskCoordinate(w2);
    return H2GeodesicArc(p1,p2);
}

void H2Isometry::setByMappingToVerticalUp(const H2Geodesic &L)
{
    Complex a1,a2;
    L.getEndpointsInDiskModel(a1,a2);
    a = L.closestPointToOriginInDiskModel();
    u = I*((conj(a)*(a1 + a2) - 2.0)/(a1 - a2));
    return;
}

void H2Isometry::setByMappingToVerticalDown(const H2Geodesic &L)
{
    Complex a1,a2;
    L.getEndpointsInDiskModel(a1,a2);
    a = L.closestPointToOriginInDiskModel();
    u = -I*((conj(a)*(a1 + a2) - 2.0)/(a1 - a2));
    return;
}

void H2Isometry::setByNormalizingPairOnLeftHandSide(const H2Isometry &f1, const H2Isometry &f1left)
{
    H2Geodesic L1, L1left, L1leftNew;
    f1.axis(L1);
    f1left.axis(L1left);
    H2Isometry fFirst, fSecond;
    fFirst.setByMappingToVerticalDown(L1);
    L1leftNew = fFirst*L1left;
    double t = H2Isometry::geodesicNormalizer(L1leftNew);
    fSecond.setVerticalTranslation(t);
    *this = fSecond*fFirst;
    return;
}

void H2Isometry::setByNormalizingPairOnRightHandSide(const H2Isometry &f1, const H2Isometry &f1left)
{
    H2Geodesic L1, L1left, L1leftNew;
    f1.axis(L1);
    f1left.axis(L1left);
    H2Isometry fFirst, fSecond;
    fFirst.setByMappingToVerticalUp(L1);
    L1leftNew = fFirst*L1left;
    double t = H2Isometry::geodesicNormalizer(L1leftNew);
    fSecond.setVerticalTranslation(t);
    *this = fSecond*fFirst;
    return;
}

H2Isometry H2Isometry::findConjugatorForGluing(const H2Isometry & f1, const H2Isometry & f1left,
                                           const H2Isometry & f2, const H2Isometry &f2left, double twistNormalized)
{
    if (std::abs(f1.traceSquared()-f2.traceSquared()) > ERROR)
    {
        //std::cout << f1 << std::endl;
        //std::cout << f2 << std::endl;
        std::cout << "ERROR in H2Isometry::findConjugatorForGluing: not the same translation lengths!" << std::endl;
    }

    H2Isometry c1, c2, c;

    H2Geodesic L1, L1left, L1leftNew;
    f1.axis(L1);
    f1left.axis(L1left);
    c1.setByMappingToVerticalUp(L1);
    L1leftNew = c1*L1left;
    double t1 = H2Isometry::geodesicNormalizer(L1leftNew);


    f2.axis(L1);
    f2left.axis(L1left);
    c2.setByMappingToVerticalDown(L1);
    L1leftNew = c2*L1left;
    double t2 = H2Isometry::geodesicNormalizer(L1leftNew);

    c.setVerticalTranslation(-t1, twistNormalized, t2);

    return c1.inverse()*c*c2;
}



bool operator ==(const H2Isometry & f1, const H2Isometry & f2)
{
    return (norm(f1.u -f2.u) + norm(f1.a - f2.a)<ERROR);
}

std::ostream & operator<<(std::ostream & out, const H2Isometry &f)
{
    if (f.isHyperbolic())
    {
        H2Geodesic axis;
        f.axis(axis);
        out << "Hyperbolic,  axis = " << axis << ", translation length = " << f.translationLength();
    }
    if (f.isParabolic())
    {
        CP1Point Z1,Z2;
        Complex z1;
        f.fixedPointsInDiskModel(Z1,Z2);
        z1 = Z1.getComplexCoordinate();
        out << "Parabolic, fixed point = " << z1;
    }
    if (f.isElliptic())
    {
        CP1Point Z1,Z2;
        Complex z1,z2;
        f.fixedPointsInDiskModel(Z1,Z2);
        z1 = Z1.getComplexCoordinate();
        z2 = Z2.getComplexCoordinate();
        out << "Elliptic, fixed points z1= " << z1 << " and z2= " << z2;
    }
    out << "   {u= " << f.u << ", a= " << f.a << "}";
    return out;
}

H2Isometry H2Isometry::identity()
{
    H2Isometry identity;
    identity.setIdentity();
    return identity;
}

double H2Isometry::geodesicNormalizer(const H2Geodesic &L)
{
    Complex z1, z2;
    L.getEndpointsInDiskModel(z1, z2);


    double b = -2 * (imag(z1) + imag(z2)) / norm(1.0 - z1*z2);

    double delta = sqrt(b*b - 1);

    double t1 = -b - delta;
    double t2 = -b + delta;
    return std::abs(t1) < std::abs(t2) ? -t1 : -t2;
}
