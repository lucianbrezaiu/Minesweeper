#ifndef QRIGHTCLICKBUTTON_H
#define QRIGHTCLICKBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

class QRightClickButton : public QPushButton
{
    Q_OBJECT

public:
    explicit QRightClickButton(QWidget *parent = 0);

private slots:
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent*);

signals:
    void rightClicked();
    void doubleClicked();
};

#endif // QRIGHTCLICKBUTTON_H
