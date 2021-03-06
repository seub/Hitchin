#include "h2point.h"
#include "h2isometry.h"
#include "h2tangentvector.h"
//#include <Eigen/Dense>

H2Point::H2Point()
{
}

Complex H2Point::getUpperHalfPlaneCoordinate() const
{
    Complex I(0.0, 1.0);
    return -I*((z + I)/(z - I));
}

Complex H2Point::getDiskCoordinate() const
{
    return z;
}

Complex H2Point::getKleinCoordinate() const
{
    //    complex z = getDiskCoordinate();
    return 2.0*z / (1.0 + norm(z));
}

void H2Point::getHyperboloidCoordinate(double &x, double &y, double &z) const
{
    double s = 1.0 - norm(this->z);
    z = (2.0 / s) - 1.0;
    x = (2.0*real(this->z) / s);
    y = (2.0*imag(this->z) / s);
}

Complex H2Point::getHyperboloidProjection() const
{
    return 2.0*z / (1.0 - norm(z));
}


void H2Point::setUpperHalfPlaneCoordinate(Complex z)
{
    Complex I(0.0, 1.0);
    this->z = I*((z - I)/(z + I));
}

void H2Point::setDiskCoordinate(Complex z)
{
    this->z = z;
}

void H2Point::setHyperboloidProjection(Complex z)
{
    this->z = z / (1.0 + sqrt(1.0 + norm(z)));
}

void H2Point::setKleinCoordinate(Complex z)
{
    this->z = z / (1.0 + sqrt(1.0 - norm(z)));
}

double H2Point::distance(const H2Point & p1, const H2Point & p2)
{
    double s;
    Complex z1,z2;
    z1 = p1.z;
    z2 = p2.z;
    s = 2.0 * (norm(z1 - z2) / ((1 - norm(z1))*(1 - norm(z2))));

    return acosh(1.0 + s);
}

H2Point H2Point::midpoint(const H2Point &p1, const H2Point &p2)
{
    Complex w1 = (p2.z - p1.z)/(1.0 - conj(p1.z)*p2.z);
    Complex w2 = w1*sqrt((1.0 - sqrt(1.0 - norm(w1)))/(norm(w1)*(1.0 + sqrt(1.0 - norm(w1)))));

    H2Point res;
    res.setDiskCoordinate((w2 + p1.z)/(1.0 + conj(p1.z)*w2));
    return res;
}

H2Point H2Point::fromDiskCoordinate(const Complex & z)
{
    H2Point out;
    out.setDiskCoordinate(z);
    return out;
}

H2Point H2Point::exponentialMap(const H2Point &p0, const Complex &u, const double &t)
{
    assert(norm(u)>0);

    Complex z1 = tanh(t/2)*u/std::abs(u);
    // NB: When translating to the origin via the map (z -z0)/(1 - bar(z0) z), u is only scaled by a positive factor.
    Complex z0 = p0.getDiskCoordinate();
    Complex z2 = (z1 + z0)/(1.0 + conj(z0)*z1);
    H2Point out;
    out.setDiskCoordinate(z2);
    return out;
}

H2Point H2Point::proportionalPoint(const H2Point &p1, const H2Point &p2, const double &s)
{
    // assert that 0 < s < 1
    Complex w1 = (p2.z - p1.z)/(1.0 - conj(p1.z)*p2.z);
    double d = log ((1+std::abs(w1))/(1-std::abs(w1)));
    Complex w2 = w1*(exp(s*d) - 1.0)/(std::abs(w1)*(exp(s*d) + 1.0));

    H2Point res;
    res.setDiskCoordinate((w2 + p1.z)/(1.0 + conj(p1.z)*w2));
    return res;
}

double H2Point::angle(const H2Point &previous, const H2Point &point, const H2Point &next)
{
    Complex u = (previous.z - point.z) / (1.0 - conj(point.z)*previous.z);
    Complex v = (next.z - point.z) / (1.0 - conj(point.z)*next.z);
    return Tools::mod2Pi(arg(v*conj(u)));
}

double H2Point::tanHalfAngle(const H2Point &previous, const H2Point &point, const H2Point &next)
{
    Complex u = (previous.z - point.z) / (1.0 - conj(point.z)*previous.z);
    Complex v = (next.z - point.z) / (1.0 - conj(point.z)*next.z);
    Complex zed = v*conj(u) / std::abs(v*conj(u));
    double x = real(zed), y = imag(zed);
    return (1-x)/y;
}

double H2Point::cotangentAngle(const H2Point &previous, const H2Point &point, const H2Point &next)
{
    Complex u = (previous.z - point.z) / (1.0 - conj(point.z)*previous.z);
    Complex v = (next.z - point.z) / (1.0 - conj(point.z)*next.z);
    Complex zed = v*conj(u) / std::abs(v*conj(u));
    double x = real(zed), y = imag(zed);
    //return x/std::abs(y);
    return x/y;
}

void H2Point::computeWeightsCentroid(const std::vector<H2Point> &neighbors, std::vector<double> &outputWeights) const
{
    H2Point previous, current, next;
    std::vector<double> preNeighborWeights;
    double r, tan1, tan2, sum;
    std::vector<double> distances;

    outputWeights.clear();
    previous = neighbors.back();
    current = neighbors.front();
    next = neighbors[1];
    for(std::vector<H2Point>::size_type j=1; j+1<neighbors.size(); ++j)
    {
        r=H2Point::distance(*this, current);
        distances.push_back(r);
        tan1=H2Point::tanHalfAngle(current, *this, next);
        tan2=H2Point::tanHalfAngle(previous, *this, current);
        preNeighborWeights.push_back((1.0/(3*M_PI*r))*(tan1 + tan2));
        previous = current;
        current = next;
        next = neighbors[j+1];
    }
    r=H2Point::distance(*this, current);
    distances.push_back(r);
    tan1=H2Point::tanHalfAngle(current, *this, next);
    tan2=H2Point::tanHalfAngle(previous, *this, current);
    preNeighborWeights.push_back((1.0/(3*M_PI*r))*(tan1 + tan2));
    previous = current;
    current = next;
    next = neighbors.front();
    r=H2Point::distance(*this, current);
    distances.push_back(r);
    tan1=H2Point::tanHalfAngle(current, *this, next);
    tan2=H2Point::tanHalfAngle(previous, *this, current);
    preNeighborWeights.push_back((1.0/(3*M_PI*r))*(tan1 + tan2));

    sum = 0.0;
    for (auto x : preNeighborWeights)
    {
        sum += x;
    }
    double epsilon = 1/sum;

    for (auto preweight : preNeighborWeights)
    {
        outputWeights.push_back(epsilon*preweight);
    }
}

void H2Point::computeWeightsCentroidNaive(const std::vector<H2Point> &neighbors, std::vector<double> &outputWeights) const
{
    H2Point previous, current, next;
    double r, sum;
    std::vector<double> distances, angles;

    outputWeights.clear();
    previous = neighbors.back();
    current = neighbors.front();
    next = neighbors[1];
    for(std::vector<H2Point>::size_type j=1; j+1<neighbors.size(); ++j)
    {
        r=H2Point::distance(*this, current);
        distances.push_back(r);
        angles.push_back(H2Point::angle(previous, *this, next)/2.0);

        previous = current;
        current = next;
        next = neighbors[j+1];
    }
    r=H2Point::distance(*this, current);
    distances.push_back(r);
    angles.push_back(H2Point::angle(previous, *this, next)/2.0);

    previous = current;
    current = next;
    next = neighbors.front();
    r=H2Point::distance(*this, current);
    distances.push_back(r);
    angles.push_back(H2Point::angle(previous, *this, next)/2.0);

    for (std::vector<double>::size_type l=0; l<angles.size(); ++l)
    {
        outputWeights.push_back(angles[l]/(2*M_PI*distances[l]));
    }

    sum = 0.0;
    for (auto neighborWeight : outputWeights)
    {
        sum += neighborWeight;
    }
    for (auto & neighborWeight : outputWeights)
    {
        neighborWeight = neighborWeight/sum;
    }
}

void H2Point::computeWeightsEnergy(const std::vector<H2Point> &neighbors, std::vector<double> &outputWeights) const
{
    //assume the neighbors are in cyclic order
    H2Point previous, current, next;

    outputWeights.clear();
    previous = neighbors.back();
    current = neighbors.front();
    next = neighbors[1];

    double cot1,cot2;
    for(std::vector<H2Point>::size_type j=1; j+1<neighbors.size(); ++j)
    {
        cot1 = cotangentAngle(current,previous,*this);
        cot2 = cotangentAngle(*this,next,current);
        outputWeights.push_back(cot1+cot2);

        previous = current;
        current = next;
        next = neighbors[j+1];
    }

    cot1 = cotangentAngle(current,previous,*this);
    cot2 = cotangentAngle(*this,next,current);
    outputWeights.push_back(cot1+cot2);

    previous = current;
    current = next;
    next = neighbors.front();
    cot1 = cotangentAngle(current,previous,*this);
    cot2 = cotangentAngle(*this,next,current);
    outputWeights.push_back(cot1+cot2);
}



std::ostream & operator<<(std::ostream & out, const H2Point &p)
{
    out << "H2Point with disk coordinate " << p.z;
    return out;
}

bool operator ==(const H2Point & p1, const H2Point & p2)
{
    return p1.z == p2.z;
}

H2Point H2Point::centroid(const std::vector<H2Point> &points, const std::vector<double> &weights)
{
    if (points.size() != weights.size())
    {
        throw(QString("Error in  H2Point::centroid: number of points does not match number of weights"));
    }

    std::vector<double> X, Y, Z;
    double a, b, c, xOut=0.0, yOut=0.0, zOut=0.0;
    H2Point out;
    X.reserve(points.size());
    Y.reserve(points.size());
    Z.reserve(points.size());

    for(const auto & p: points)
    {
        p.getHyperboloidCoordinate(a,b,c);
        X.push_back(a);
        Y.push_back(b);
        Z.push_back(c);
    }

    for(unsigned int j=0; j<points.size(); ++j)
    {
        xOut += weights[j]*X[j];
        yOut += weights[j]*Y[j];
        zOut += weights[j]*Z[j];
    }

    double s = 1/ sqrt(zOut*zOut - xOut*xOut - yOut*yOut);
    xOut = s*xOut;
    yOut = s*yOut;
    out.setHyperboloidProjection(Complex(xOut,yOut));
    return out;
}

void H2Point::weightedLogSum(const std::vector<H2Point> &points, const std::vector<double> &weights, H2TangentVector &output) const
{
    assert (points.size() == weights.size());
    output = H2TangentVector(*this);

    std::vector<double> distancesToNeighbors;
    std::vector<Complex> directionsToNeighbors;


    for (uint i=0; i<points.size(); ++i)
    {
        output = output + weights[i]*H2TangentVector(*this,points[i]);
        distancesToNeighbors.push_back(H2Point::distance(*this, points[i]));
        directionsToNeighbors.push_back((H2TangentVector(*this,points[i])).getVector());
    }


    std::vector<double> affineWeights, energyWeights;
    this->computeWeightsEnergy(points, energyWeights);
    this->computeWeightsCentroid(points, affineWeights);

/*
    std::cout << std::endl;
    std::cout << "point =" << *this << std::endl;

    std::cout << "weights = " << weights << std::endl;
    std::cout << "affine weights = " << affineWeights << std::endl;
    std::cout << "energy weights = " << energyWeights << std::endl;

    std::cout << "distances to neighbors = " << distancesToNeighbors << std::endl;
    std::cout << "directions to neighbors = " << directionsToNeighbors << std::endl;

    std::cout << "output length = " << output.length() << std::endl;
    std::cout << "output direction = " << output.getVector() << std::endl;

    H2Point C = centroid(points, affineWeights);
    H2TangentVector toC(*this, C);
    std::cout << "distance to centroid = " << toC.length() << std::endl;
    std::cout << "direction to centroid = " << toC.getVector() << std::endl;
    */

}

bool H2Point::compareAngles(const H2Point &p1, const H2Point &p2)
{
    Complex z1 = p1.z - z;
    Complex z2 = p2.z - z;
    double x1,x2,y1,y2;
    x1 = real(z1); y1 = imag(z1);
    x2 = real(z2); y2 = imag(z2);
    if (x1>=0.0)
    {
        if (y1>=0.0)
        {
            if (x2>=0.0 && y2>=0.0)
            {
                return (y1*x2 < x1*y2);
            }
            else return true;
        }
        else return (x2>0.0 && y2<0.0 && (y1*x2 < x1*y2));
    }
    else if (x2<0.0)
    {
        return (y1*x2 < x1*y2);
    }
    else return (y2<0.0);
}
