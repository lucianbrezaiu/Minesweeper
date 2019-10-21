#include "qrightclickbutton.h"

QRightClickButton::QRightClickButton(QWidget *parent) : QPushButton(parent)
{
}

void QRightClickButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::RightButton)
    {
        emit rightClicked();
    }
    else if(e->button()==Qt::LeftButton)
    {
        emit clicked();
    }
}

void QRightClickButton::mouseDoubleClickEvent(QMouseEvent *e)
{
    emit doubleClicked();
}
