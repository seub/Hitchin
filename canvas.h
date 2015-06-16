#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

#include "tools.h"

class CanvasDelegate;
class Window;
class ActionHandler;

class Canvas : public QWidget
{
    Q_OBJECT

    friend class Window;

public:
    explicit Canvas(CanvasDelegateType delegateType, Window *const window = 0, bool left = false, bool right = false, ActionHandler* handler = 0);
    ~Canvas();

    void changeDelegate(CanvasDelegateType delegateType, bool left = false, bool right = false, ActionHandler *handler = 0);
    CanvasDelegateType getDelegateType() const;

signals:


private:
public:
    CanvasDelegate *delegate;

    void paintEvent(QPaintEvent *event);
//public slots:
    void resizeEvent(QResizeEvent *resizeEvent);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void wheelEvent(QWheelEvent *wheelEvent);
    void keyPressEvent(QKeyEvent *keyEvent);
    void enterEvent(QEvent *);

private:
    Canvas(); // Dummy constructor
    Canvas(const Canvas &other); // Copy constructor
    Canvas & operator=(Canvas other); // Copy-assignment operator

    void rescale();

    Window* window;
};

#endif // CANVAS_H
