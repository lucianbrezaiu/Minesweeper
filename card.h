#ifndef CARD_H
#define CARD_H

#include <QPushButton>
#include "buttontype.h"
#include "qrightclickbutton.h"


class Card
{
public:
    Card();
    Card(QRightClickButton*,int,int);

    void setIcon(QString);

    QRightClickButton* getButton();
    int getRow();
    int getColumn();
    buttonType getType();
    int getdigit();
    bool getFlag();
    bool getFlipped();

    void setButton(QRightClickButton*);
    void setRow(int);
    void setColumn(int);
    void setType(buttonType);
    void setDigit(int);
    void setFlag(bool);
    void setFlipped(bool);

    void flip();

private:

    QRightClickButton* button;
    int row;
    int column;
    buttonType type;
    int _digit;
    bool flag;
    bool flipped;


};

#endif // CARD_H
