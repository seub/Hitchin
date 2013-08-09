#include "h2mesh.h"
#include "h2geodesic.h"
#include "grouprepresentation.h"
#include "h2isometry.h"

H2mesh::H2mesh(double step, const IsomH2Representation &rho): step(step), rho(rho)
{
    polygon = rho.generatePolygon(100);
    polygon.getExtremalCoordinatesInHyperboloidProjection(xMin,xMax,yMin,yMax);

    listOfIsoms = rho.getSidePairingsNormalizedAroundVertices();

    xMin -= 1.5*step;
    yMin -= 1.5*step;

    std::cout << "xMin = " << xMin << ", xMax = " << xMax << ", yMin = " << yMin << ", yMax = " << yMax << std::endl;

    nbPointsX = 3 + int((xMax - xMin)/step);
    nbPointsY = 3 + int((yMax - yMin)/step);
    nbMeshPoints = nbPointsX*nbPointsY;
    firstMeshPoint = complex( xMin , yMin );

    xMax = xMin + (nbPointsX-1)*step;
    yMax = yMin + (nbPointsY-1)*step;


    meshPoints = new complex[nbMeshPoints];
    isInside = new bool[nbMeshPoints];
    leftNeighborIndices = new int[nbMeshPoints];
    rightNeighborIndices = new int[nbMeshPoints];
    upNeighborIndices = new int[nbMeshPoints];
    downNeighborIndices = new int[nbMeshPoints];

    fillMeshPoints();
    fillIsInside();
    fillNeighbors();
}

void H2mesh::fillMeshPoints()
{
    int i, j;
    double x = real(firstMeshPoint), y = imag(firstMeshPoint);
    for (j=0; j<nbPointsY; j++)
    {
        x = real(firstMeshPoint);
        for (i=0; i<nbPointsX; i++)
        {
            meshPoints[i + j*nbPointsX] = complex(x,y);
            x += step;
        }
        y += step;
    }
    return;
}

void H2mesh::fillIsInside()
{
    H2Point p;
    for (int i=0; i<nbMeshPoints; i++)
    {
        p.setHyperboloidProjection(meshPoints[i]);
        isInside[i] = polygon.contains(p);
    }
    return;
}

void H2mesh::fillNeighbors()
{
    for (int i=0; i<nbMeshPoints; i++)
    {
        if (!isInside[i])
        {
            leftNeighborIndices[i]=-1;
        }
        else if (i%nbPointsX==0)
        {
            std::cout << "ERROR in H2mesh::fillNeighbors: point inside polygon is a mesh boundary point" << std::endl;
        }
        else if (isInside[i-1])
        {
            leftNeighborIndices[i]=i-1;
        }
        else
        {
            H2Point pIn,pOut;
            pIn.setHyperboloidProjection(meshPoints[i-1]);

            unsigned int j=0;
            pOut = pIn;
            while (!polygon.contains(pOut) && j<listOfIsoms.size())
            {
                pOut = listOfIsoms[j]*pIn;
                j++;
            }
            if (!polygon.contains(pOut))
            {
                std::cout << "Error in H2mesh::fillNeighbors: Leftmost neighbor was not found" << std::endl;
                leftNeighborIndices[i] = -1;
            }
            else
            {
                leftNeighborIndices[i] = getClosestMeshIndex(pOut.getHyperboloidProjection());
            }
        }
    }

    for (int i=0; i<nbMeshPoints; i++)
    {
        if (!isInside[i])
        {
            rightNeighborIndices[i]=-1;
        }
        else if (i%nbPointsX==nbPointsX-1)
        {
            std::cout << "ERROR in H2mesh::fillNeighbors: point inside polygon is a mesh boundary point" << std::endl;
        }
        else if (isInside[i+1])
        {
            rightNeighborIndices[i]=i+1;
        }
        else
        {
            H2Point pIn,pOut;
            pIn.setHyperboloidProjection(meshPoints[i+1]);

            unsigned int j=0;
            pOut = pIn;
            while (!polygon.contains(pOut) && j<listOfIsoms.size())
            {
                pOut = listOfIsoms[j]*pIn;
                j++;
            }
            if (!polygon.contains(pOut))
            {
                std::cout << "Error in H2mesh::fillNeighbors: Rightmost neighbor was not found" << std::endl;
                rightNeighborIndices[i] = -1;
            } else
            {
                rightNeighborIndices[i] = getClosestMeshIndex(pOut.getHyperboloidProjection());
            }
        }
    }

    for (int i=0; i<nbMeshPoints; i++)
    {
        if (!isInside[i])
        {
            upNeighborIndices[i]=-1;
        }
        else if (i>nbPointsX*(nbPointsY-1))
        {
            std::cout << "ERROR in H2mesh::fillNeighbors: point inside polygon is a mesh boundary point" << std::endl;
        }
        else if (isInside[i+nbPointsX])
        {
            upNeighborIndices[i]=i+nbPointsX;
        }
        else
        {
            H2Point pIn,pOut;
            pIn.setHyperboloidProjection(meshPoints[i+nbPointsX]);

            unsigned int j=0;
            pOut = pIn;
            while (!polygon.contains(pOut) && j<listOfIsoms.size())
            {
                pOut = listOfIsoms[j]*pIn;
                j++;
            }
            if (!polygon.contains(pOut))
            {
                std::cout << "Error in H2mesh::fillNeighbors: Upmost neighbor was not found" << std::endl;
                upNeighborIndices[i] = -1;
            } else
            {
                upNeighborIndices[i] = getClosestMeshIndex(pOut.getHyperboloidProjection());
            }
        }
    }

    for (int i=0; i<nbMeshPoints; i++)
    {
        if (!isInside[i])
        {
            downNeighborIndices[i]=-1;
        }
        else if (i<nbPointsX)
        {
            std::cout << "ERROR in H2mesh::fillNeighbors: point inside polygon is a mesh boundary point" << std::endl;
        }
        else if (isInside[i-nbPointsX])
        {
            downNeighborIndices[i]=i-nbPointsX;
        }
        else
        {
            H2Point pIn,pOut;
            pIn.setHyperboloidProjection(meshPoints[i-nbPointsX]);

            unsigned int j=0;
            pOut = pIn;
            while (!polygon.contains(pOut) && j<listOfIsoms.size())
            {
                pOut = listOfIsoms[j]*pIn;
                j++;
            }
            if (!polygon.contains(pOut))
            {
                std::cout << "Error in H2mesh::fillNeighbors: Downmost neighbor was not found" << std::endl;
                downNeighborIndices[i] = -1;
            } else
            {
                downNeighborIndices[i] = getClosestMeshIndex(pOut.getHyperboloidProjection());
            }
        }
    }
    return;
}

H2mesh::~H2mesh()
{
    delete [] meshPoints;
    delete [] isInside;
    delete [] leftNeighborIndices;
    delete [] rightNeighborIndices;
    delete [] upNeighborIndices;
    delete [] downNeighborIndices;
}

const complex * H2mesh::getMeshPoints() const
{
    return meshPoints;
}

const bool * H2mesh::getIsInside() const
{
    return isInside;
}

int H2mesh::getNbMeshPoints() const
{
    return nbMeshPoints;
}

H2Polygon H2mesh::getPolygon() const
{
    return polygon;
}

IsomH2Representation H2mesh::getRho() const
{
    return rho;
}

std::vector<int> H2mesh::getSpecialPoints() const
{
    return specialPoints;
}

int H2mesh::getClosestMeshIndex(const complex &z)
{
    if (real(z) < xMin || real(z) > xMax  || imag(z) < yMin || imag(z) > yMax)
    {
        std::cout << "Error in H2mesh::getClosestMeshIndex: Point is not within mesh boundaries" << std::endl;
        return -1;
    }


    double x=real(z), y=imag(z);
    int NcloseX = int((x - real(firstMeshPoint))/step);
    int NcloseY = int((y - imag(firstMeshPoint))/step);
    int index = NcloseX + nbPointsX*NcloseY;

    H2Point p;
    p.setHyperboloidProjection(z);
    if (!polygon.contains(p))
    {
        std::cout << "Error in H2mesh::getClosestMeshIndex: Point is not inside polygon" << std::endl;
        return index;
    }


    double bestStep = step*step*2.0;
    int output;

    bool pointFound = false;
    if (index < nbMeshPoints)
    {
        if (norm(z - meshPoints[index]) < bestStep && isInside[index])
        {
            output = index;
            bestStep = norm(z - meshPoints[index]);
            pointFound = true;
        }
    }
    if (index % nbPointsX != nbPointsX -1 && index+1<nbMeshPoints)
    {
        if (norm(z - meshPoints[index + 1]) < bestStep && isInside[index + 1])
        {
            output = index + 1;
            bestStep = norm(z - meshPoints[index + 1]);
            pointFound = true;
        }
    }
    if (index+nbPointsX < nbMeshPoints)
    {
        if (norm(z - meshPoints[index + nbPointsX]) < bestStep && isInside[index + nbPointsX])
        {
            output = index + nbPointsX;
            bestStep = norm(z - meshPoints[index + nbPointsX]);
            pointFound = true;
        }
    }
    if (index + nbPointsX +1 < nbMeshPoints && index % nbPointsX != nbPointsX -1)
    {
        if (norm(z - meshPoints[index + nbPointsX + 1]) < bestStep && isInside[index + nbPointsX + 1])
        {
            output = index + nbPointsX + 1;
            pointFound = true;
        }
    }

    if (!pointFound)
    {
        std::cout << "Error in H2mesh::getClosestMeshIndex: no point found" << std::endl;
        specialPoints.push_back(index);
    }
    else if (!isInside[output])
    {
        std::cout << "Error in H2mesh::getClosestMeshIndex: Close mesh points are all outside polygon" << std::endl;
    }
    return output;
}
















