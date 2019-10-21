#include "card.h"
#include <QMessageBox>
#include <QDebug>

Card::Card()
{
    _digit = 0;
    flag = false;
    type = none;
    flipped = false;
}

Card::Card(QRightClickButton* button, int row, int column)
{
    this->button = button;
    this->row = row;
    this->column = column;
}

void Card::setIcon(QString path)
{
    QPixmap pixmap(path);
    QIcon ButtonIcon(pixmap);
    QSize iconSize(30,30);
    button->setIcon(ButtonIcon);
    button->setIconSize(pixmap.rect().size());
    button->setIconSize(iconSize);
}

QRightClickButton* Card::getButton()
{
    return this->button;
}

int Card::getRow()
{
    return this->row;
}

int Card::getColumn()
{
    return this->column;
}

buttonType Card::getType()
{
    return this->type;
}

int Card::getdigit()
{
    return this->_digit;
}

bool Card::getFlag()
{
    return this->flag;
}

bool Card::getFlipped()
{
    return this->flipped;
}

void Card::setButton(QRightClickButton* button)
{
    this->button = button;

}

void Card::setRow(int row)
{
    this->row = row;
}

void Card::setColumn(int column)
{
    this->column = column;
}

void Card::setType(buttonType type)
{
    this->type = type;
}

void Card::setDigit(int digit)
{
    this->_digit = digit;
}

void Card::setFlag(bool flag)
{
    this->flag = flag;
}

void Card::setFlipped(bool)
{
    this->flipped = flipped;
}

void Card::flip()
{
    flipped = true;
    switch(type)
    {
    case bomb:
        setIcon(QString::fromStdString("E:\\workspace - qt\\c++ tema_3\\Resources\\bomb.png"));
        break;
    case digit:
        setIcon(QString::fromStdString("E:\\workspace - qt\\c++ tema_3\\Resources\\") + QString::number(_digit) + QString::fromStdString(".png"));
        break;
    case empty:
        QColor col = QColor(Qt::gray);
        QString qss = QString("background-color: %1").arg(col.name());
        button->setStyleSheet(qss);
        button->update();
        break;
    }
}

