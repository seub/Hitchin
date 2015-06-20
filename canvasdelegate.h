#ifndef CANVASDELEGATE_H
#define CANVASDELEGATE_H

#include "tools.h"
#include "h2isometry.h"

#include <QColor>

class QImage;
class QPainter;
class QPen;
class QMouseEvent;
class QKeyEvent;

class H2Point;
class Canvas;
class Circle;
class ActionHandler;


class CanvasDelegate
{
    friend class Canvas;

public:
    explicit CanvasDelegate(int sizeX, int sizeY, ActionHandler* handler = 0);
    virtual ~CanvasDelegate();

    const QImage * getImageBack() const;
    const QImage * getImageTop() const;


    void drawPoint(const Complex &z, const QColor &color = "black", int width = 3, bool back = true);
    void drawSegment(const Complex &endpoint1, const Complex &endpoint2, const QColor &color = "black", int width = 1, bool back = true);

    void drawCircle(const Complex &center, double radius, const QColor &color = "black", int width = 1, bool back = true);

    void drawSmallerArc(const Complex &center, double radius,
                                 const Complex &endpoint1, const Complex &endpoint2, const QColor &color = "black", int width = 1, bool back = true);

    virtual void redrawBuffer(bool back = true, bool top = true, const H2Isometry &mobius = H2Isometry::identity()) = 0;

    void zoom(double coeff);
    void zoom(double coeff, int centerX, int centerY);
    void mouseShift(int x, int y);
    void shift(int x, int y);
    void resetView();
    double xMax() const;
    double yMin() const;

    void enableRedrawBuffer(bool back = true, bool top = true);

    virtual void mousePress(QMouseEvent *) {}
    virtual void mouseMove(QMouseEvent *) {}
    virtual void mouseRelease(QMouseEvent *) {}
    virtual void keyPress(QKeyEvent *) {}
    virtual void enter() {}
    virtual void leave() {}

private:
    CanvasDelegate(); // Dummy constructor
    CanvasDelegate(const CanvasDelegate &other); // Copy constructor

    bool isInside(const Complex &point) const;
    void rescale(int sizeX, int sizeY);
    void resetView(int sizeX, int sizeY);
    virtual void subResetView() {}
    void ComplexToPixelCoordinates(int & xOut, int & yOut, Complex z) const;

    void bestLineSegmentApproximationOfSmallerArc(const Complex &center, double radius, const Complex &endpoint1, const Complex &endpoint2,
                                                  Complex &newEndpoint1, Complex &newEndpoint2, bool &noBestApproximation, bool &arcOutsideCanvas, bool &failed);
    bool circleIntersectsCanvasBoundary(const Complex &center, double radius, std::vector<Complex> &intersectionsOut) const;
    bool dealWithExceptionalArc(const Complex &center, double radius,
                                   const Complex &endpoint1, const Complex &endpoint2, const QColor &color = "black", int width = 1, bool back = true);
    bool isAlmostSmallStraightArc(const Complex &center, double radius, const Complex &endpoint1, const Complex &endpoint2) const;
    bool isAlmostInfiniteRadius(double radius) const;
    static bool liesOnSmallerArc(const Complex &point, const Complex &center, const Complex &endpoint1, const Complex &endpoint2);


protected:
    Complex PixelToComplexCoordinates(int x, int y) const;

    QImage *imageBack, *imageTop;
    QPen *penBack, *penTop;
    QPainter *painterBack, *painterTop;

    CanvasDelegateType delegateType;
    int sizeX, sizeY;
    double scaleX, scaleY;
    double xMin, yMax;
    int nbArcs, nbStraightArcs, nbAlmostStraightArcs; // For testing
    int nbBestLineFails, nbBestLineCalls; // For testing

    double xMinSave, yMaxSave;
    int mouseX, mouseY;
    int detectionRadius;

    ActionHandler *handler;
    bool enableRedrawBufferBack, enableRedrawBufferTop;
    bool resetPenBack;

};

#endif // CANVASDELEGATE_H
