#include "minesweeper.h"
#include "ui_minesweeper.h"

#include <QMessageBox>
#include <QVector>
#include <QDebug>
#include <QMouseEvent>
#include <QStack>
#include <QInputDialog>

Minesweeper::Minesweeper(QWidget *parent) : QMainWindow(parent), ui(new Ui::Minesweeper)
{
    ui->setupUi(this);
    statusGame = false;
    rows = 9;
    columns = 9;
    nrBombs = 10;
    discoveredBombs = 0;
    seconds = 0;

    initialize();

    QPixmap pixmap("E:\\workspace - qt\\c++ tema_3\\Resources\\smile.png");
    QIcon ButtonIcon(pixmap);
    QSize iconSize(30,30);
    ui->smileBtn->setIcon(ButtonIcon);
    ui->smileBtn->setIconSize(pixmap.rect().size());
    ui->smileBtn->setIconSize(iconSize);

    connect(ui->smileBtn,SIGNAL(clicked()),this,SLOT(on_newGame_triggered()));

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()),this, SLOT(updateLCD2()));
}

Minesweeper::~Minesweeper()
{
    delete ui;
}

void Minesweeper::initialize()
{
    QPixmap pixmap("E:\\workspace - qt\\c++ tema_3\\Resources\\smile.png");
    QIcon ButtonIcon(pixmap);
    QSize iconSize(30,30);
    ui->smileBtn->setIcon(ButtonIcon);
    ui->smileBtn->setIconSize(pixmap.rect().size());
    ui->smileBtn->setIconSize(iconSize);

    updateLCD1();
    matrix = new Card*[rows];
    for(int i=0;i<rows;i++)
    {
        matrix[i] = new Card[columns];
    }

    for(int i=0;i<rows;i++)
    {
        for(int j=0;j<columns;j++)
        {
            QRightClickButton* btn = new QRightClickButton();
            btn->setMaximumHeight(30);
            btn->setMaximumWidth(30);
            btn->setDisabled(true);

            connect(btn,SIGNAL(doubleClicked()),this,SLOT(doubleClickEvent()));
            connect(btn,SIGNAL(clicked()),this,SLOT(leftClickEvent()));
            connect(btn,SIGNAL(rightClicked()),this,SLOT(rightClickEvent()));

            ui->grid->addWidget(btn,i,j);
            matrix[i][j].setButton(btn);
            matrix[i][j].setRow(i);
            matrix[i][j].setColumn(j);
        }
    }

    this->setFixedSize(this->columns*30,this->rows*30+100);


    setBombButtons();
    setDigitButtons();
    setEmptyButtons();
}

void Minesweeper::clear()
{
    timer->stop();
    discoveredBombs=0;
    seconds=-1;
    updateLCD2();

    QLayoutItem* child;
    while(ui->grid->count()!=0)
    {
        child = ui->grid->takeAt(0);
        if(child->widget() != 0)
        {
            delete child->widget();
        }
        delete child;
    }
}

void Minesweeper::on_newGame_triggered()
{
    clear();
    initialize();

    seconds=-1;
    updateLCD2();
    timer->start();

    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
        {
            matrix[i][j].getButton()->setEnabled(true);
        }

    statusGame = true;
}

void Minesweeper::on_exit_triggered()
{
    this->close();
}

void Minesweeper::on_easy_triggered()
{
    clear();

    this->rows = 9;
    this->columns = 9;
    this->nrBombs = 10;

    initialize();
}

void Minesweeper::on_medium_triggered()
{
    clear();

    this->rows = 16;
    this->columns = 16;
    this->nrBombs = 30;

    initialize();
}

void Minesweeper::on_hard_triggered()
{
    clear();

    this->rows = 16;
    this->columns = 30;
    this->nrBombs = 90;

    initialize();
}

void Minesweeper::leftClickEvent()
{
    if(statusGame == true)
    {
        QRightClickButton* parent = qobject_cast<QRightClickButton*>(sender());

        for(int i=0;i<rows;i++)
            for(int j=0;j<columns;j++)
            {
                if(matrix[i][j].getButton() == parent)
                {
                    if(matrix[i][j].getFlag()==true)
                    {
                        matrix[i][j].getButton()->setIcon(QIcon());
                        matrix[i][j].setFlag(false);
                        discoveredBombs--;
                        updateLCD1();
                    }
                    else
                    {
                        if(matrix[i][j].getType()==empty)
                        {
                            cascade(matrix[i][j]);
                        }
                        else
                        {
                            matrix[i][j].flip();
                            if(matrix[i][j].getType()==bomb)
                            {
                                lostGame();
                                matrix[i][j].setIcon("E:\\workspace - qt\\c++ tema_3\\Resources\\redBomb.png");
                                break;
                            }
                        }

                    }
                    wonGame();
                    break;
                }
            }
    }
}

void Minesweeper::rightClickEvent()
{
    if(statusGame == true)
    {
        QRightClickButton* parent = qobject_cast<QRightClickButton*>(sender());
        for(int i=0;i<rows;i++)
            for(int j=0;j<columns;j++)
            {
                if(matrix[i][j].getButton() == parent && matrix[i][j].getFlipped()==false)
                {
                    if(matrix[i][j].getFlag()==true)
                    {
                        matrix[i][j].getButton()->setIcon(QIcon());
                        matrix[i][j].setFlag(false);
                        discoveredBombs--;
                        updateLCD1();
                    }
                    else
                    {
                        matrix[i][j].setIcon("E:\\workspace - qt\\c++ tema_3\\Resources\\flag.png");
                        matrix[i][j].setFlag(true);
                        discoveredBombs++;
                        updateLCD1();
                    }
                }
            }
    }

}

void Minesweeper::doubleClickEvent()
{
    if(statusGame == true)
    {
        QRightClickButton* parent = qobject_cast<QRightClickButton*>(sender());
        for(int i=0;i<rows;i++)
            for(int j=0;j<columns;j++)
            {
                if(matrix[i][j].getButton() == parent && matrix[i][j].getType()==digit && matrix[i][j].getFlipped()==true)
                {
                    int flags = numberFlagsForElement(i,j);
                    if(matrix[i][j].getdigit() == flags)
                    {
                        if(checkBombForFlags(i,j))
                        {
                            openNeighbors(i,j);
                            wonGame();
                        }
                        else
                        {
                            lostGame();
                            QMessageBox::information(this,tr("Message"),tr("You lost the game!"));
                        }

                    }
                    break;
                }
            }
    }
}


void Minesweeper::setBombButtons()
{
    for(int i=0;i< this->nrBombs;i++)
    {
        bool ok = true;
        do
        {
            int row = qrand() % rows;
            int column = qrand() % columns;
            if(matrix[row][column].getType() == none)
            {
                matrix[row][column].setType(bomb);
                ok = false;
            }
        }while(ok==true);
    }
}

void Minesweeper::setDigitButtons()
{
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
        {
            if(matrix[i][j].getType() == none)
            {
                int nr = numberBombsForElement(i,j);
                if(nr!=0)
                {
                    matrix[i][j].setType(digit);
                    matrix[i][j].setDigit(nr);
                }

            }

        }
}

int Minesweeper::numberBombsForElement(int i, int j)
{
    // functia numara cate bombe are o celula pe vecinii sai
    int count = 0;
    if(i==0 && j==0) // coltul stanga sus
    {
        if(matrix[i][j+1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j+1].getType()==bomb)
        {
            count++;
        }
    }

    else if(i==0 && j==columns-1) // coltul dreapta sus
    {
        if(matrix[i][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j-1].getType()==bomb)
        {
            count++;
        }
    }
    else if(i==rows-1 && j==0) // coltul stanga jos
    {
        if(matrix[i-1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i][j+1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i-1][j+1].getType()==bomb)
        {
            count++;
        }
    }
    else if(i==rows-1 && j==columns-1) // coltul dreapta jos
    {
        if(matrix[i-1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i-1][j-1].getType()==bomb)
        {
            count++;
        }
    }
    else if (i==0) // marginea de sus
    {
        if(matrix[i][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i][j+1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j+1].getType()==bomb)
        {
            count++;
        }
    }
    else if(i==rows-1) //  marginea de jos
    {
        if(matrix[i][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i][j+1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i-1][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i-1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i-1][j+1].getType()==bomb)
        {
            count++;
        }
    }
    else if(j==0)// marginea din stanga
    {
        if(matrix[i-1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i-1][j+1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i][j+1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j+1].getType()==bomb)
        {
            count++;
        }
    }
    else if(j==columns-1) // marginea din dreapta
    {
        if(matrix[i-1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i-1][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j-1].getType()==bomb)
        {
            count++;
        }
    }
    else
    {
        if(matrix[i-1][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i-1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i-1][j+1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i][j+1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j-1].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j].getType()==bomb)
        {
            count++;
        }
        if(matrix[i+1][j+1].getType()==bomb)
        {
            count++;
        }
    }
    return count;
}

int Minesweeper::numberFlagsForElement(int i, int j)
{
    // functia numara cate steaguri are o celula pe vecinii sai
    int count = 0;
    if(i==0 && j==0) // coltul stanga sus
    {
        if(matrix[i][j+1].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j+1].getFlag())
        {
            count++;
        }
    }

    else if(i==0 && j==columns-1) // coltul dreapta sus
    {
        if(matrix[i][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j-1].getFlag())
        {
            count++;
        }
    }
    else if(i==rows-1 && j==0) // coltul stanga jos
    {
        if(matrix[i-1][j].getFlag())
        {
            count++;
        }
        if(matrix[i][j+1].getFlag())
        {
            count++;
        }
        if(matrix[i-1][j+1].getFlag())
        {
            count++;
        }
    }
    else if(i==rows-1 && j==columns-1) // coltul dreapta jos
    {
        if(matrix[i-1][j].getFlag())
        {
            count++;
        }
        if(matrix[i][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i-1][j-1].getFlag())
        {
            count++;
        }
    }
    else if (i==0) // marginea de sus
    {
        if(matrix[i][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i][j+1].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j+1].getFlag())
        {
            count++;
        }
    }
    else if(i==rows-1) //  marginea de jos
    {
        if(matrix[i][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i][j+1].getFlag())
        {
            count++;
        }
        if(matrix[i-1][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i-1][j].getFlag())
        {
            count++;
        }
        if(matrix[i-1][j+1].getFlag())
        {
            count++;
        }
    }
    else if(j==0)// marginea din stanga
    {
        if(matrix[i-1][j].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j].getFlag())
        {
            count++;
        }
        if(matrix[i-1][j+1].getFlag())
        {
            count++;
        }
        if(matrix[i][j+1].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j+1].getFlag())
        {
            count++;
        }
    }
    else if(j==columns-1) // marginea din dreapta
    {
        if(matrix[i-1][j].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j].getFlag())
        {
            count++;
        }
        if(matrix[i-1][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j-1].getFlag())
        {
            count++;
        }
    }
    else
    {
        if(matrix[i-1][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i-1][j].getFlag())
        {
            count++;
        }
        if(matrix[i-1][j+1].getFlag())
        {
            count++;
        }
        if(matrix[i][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i][j+1].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j-1].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j].getFlag())
        {
            count++;
        }
        if(matrix[i+1][j+1].getFlag())
        {
            count++;
        }
    }
    return count;
}

bool Minesweeper::checkBombForFlags(int i, int j)
{
    // functia verifica pentru o celula daca toate bombele vecine au steaguri

    if(i==0 && j==0) // coltul stanga sus
    {
        if(matrix[i][j+1].getType()==bomb && !matrix[i][j+1].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j].getType()==bomb && !matrix[i+1][j].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j+1].getType()==bomb && !matrix[i+1][j+1].getFlag())
        {
            return false;
        }
    }

    else if(i==0 && j==columns-1) // coltul dreapta sus
    {
        if(matrix[i][j-1].getType()==bomb && !matrix[i][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j].getType()==bomb && !matrix[i+1][j].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j-1].getType()==bomb && !matrix[i+1][j-1].getFlag())
        {
            return false;
        }
    }
    else if(i==rows-1 && j==0) // coltul stanga jos
    {
        if(matrix[i-1][j].getType()==bomb && !matrix[i-1][j].getFlag())
        {
            return false;
        }
        if(matrix[i][j+1].getType()==bomb && !matrix[i][j+1].getFlag())
        {
            return false;
        }
        if(matrix[i-1][j+1].getType()==bomb && !matrix[i-1][j+1].getFlag())
        {
            return false;
        }
    }
    else if(i==rows-1 && j==columns-1) // coltul dreapta jos
    {
        if(matrix[i-1][j].getType()==bomb && !matrix[i-1][j].getFlag())
        {
            return false;
        }
        if(matrix[i][j-1].getType()==bomb && !matrix[i][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i-1][j-1].getType()==bomb && !matrix[i-1][j-1].getFlag())
        {
            return false;
        }
    }
    else if (i==0) // marginea de sus
    {
        if(matrix[i][j-1].getType()==bomb && !matrix[i][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i][j+1].getType()==bomb && !matrix[i][j+1].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j-1].getType()==bomb && !matrix[i+1][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j].getType()==bomb && !matrix[i+1][j].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j+1].getType()==bomb && !matrix[i+1][j+1].getFlag())
        {
            return false;
        }
    }
    else if(i==rows-1) //  marginea de jos
    {
        if(matrix[i][j-1].getType()==bomb && !matrix[i][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i][j+1].getType()==bomb && !matrix[i][j+1].getFlag())
        {
            return false;
        }
        if(matrix[i-1][j-1].getType()==bomb && !matrix[i-1][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i-1][j].getType()==bomb && !matrix[i-1][j].getFlag())
        {
            return false;
        }
        if(matrix[i-1][j+1].getType()==bomb && !matrix[i-1][j+1].getFlag())
        {
            return false;
        }
    }
    else if(j==0)// marginea din stanga
    {
        if(matrix[i-1][j].getType()==bomb && !matrix[i-1][j].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j].getType()==bomb && !matrix[i+1][j].getFlag())
        {
            return false;
        }
        if(matrix[i-1][j+1].getType()==bomb && !matrix[i-1][j+1].getFlag())
        {
            return false;
        }
        if(matrix[i][j+1].getType()==bomb && !matrix[i][j+1].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j+1].getType()==bomb && !matrix[i+1][j+1].getFlag())
        {
            return false;
        }
    }
    else if(j==columns-1) // marginea din dreapta
    {
        if(matrix[i-1][j].getType()==bomb && !matrix[i-1][j].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j].getType()==bomb && !matrix[i+1][j].getFlag())
        {
            return false;
        }
        if(matrix[i-1][j-1].getType()==bomb && !matrix[i-1][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i][j-1].getType()==bomb && !matrix[i][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j-1].getType()==bomb && !matrix[i+1][j-1].getFlag())
        {
            return false;
        }
    }
    else
    {
        if(matrix[i-1][j-1].getType()==bomb && !matrix[i-1][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i-1][j].getType()==bomb && !matrix[i-1][j].getFlag())
        {
            return false;
        }
        if(matrix[i-1][j+1].getType()==bomb && !matrix[i-1][j+1].getFlag())
        {
            return false;
        }
        if(matrix[i][j-1].getType()==bomb && !matrix[i][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i][j+1].getType()==bomb && !matrix[i][j+1].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j-1].getType()==bomb && !matrix[i+1][j-1].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j].getType()==bomb && !matrix[i+1][j].getFlag())
        {
            return false;
        }
        if(matrix[i+1][j+1].getType()==bomb && !matrix[i+1][j+1].getFlag())
        {
            return false;
        }
    }
    return true;
}

void Minesweeper::openNeighbors(int i, int j)
{
    //functia deschide toti vecinii elementului (i,j)

    if(i==0 && j==0) // coltul stanga sus
    {
        open(i,j+1);
        open(i+1,j);
        open(i+1,j+1);
    }

    else if(i==0 && j==columns-1) // coltul dreapta sus
    {
        open(i,j-1);
        open(i+1,j);
        open(i+1,j-1);
    }
    else if(i==rows-1 && j==0) // coltul stanga jos
    {
        open(i-1,j);
        open(i,j+1);
        open(i-1,j+1);
    }
    else if(i==rows-1 && j==columns-1) // coltul dreapta jos
    {
        open(i-1,j);
        open(i,j-1);
        open(i-1,j-1);
    }
    else if (i==0) // marginea de sus
    {
        open(i,j-1);
        open(i,j+1);
        open(i+1,j-1);
        open(i+1,j);
        open(i+1,j+1);
    }
    else if(i==rows-1) //  marginea de jos
    {
        open(i,j-1);
        open(i,j+1);
        open(i-1,j-1);
        open(i-1,j);
        open(i-1,j+1);
    }
    else if(j==0)// marginea din stanga
    {
        open(i-1,j);
        open(i+1,j);
        open(i-1,j+1);
        open(i,j+1);
        open(i+1,j+1);
    }
    else if(j==columns-1) // marginea din dreapta
    {
        open(i-1,j);
        open(i+1,j);
        open(i-1,j-1);
        open(i,j-1);
        open(i+1,j-1);
    }
    else
    {
        open(i-1,j-1);
        open(i-1,j);
        open(i-1,j+1);
        open(i,j-1);
        open(i,j+1);
        open(i+1,j-1);
        open(i+1,j);
        open(i+1,j+1);
    }
}

void Minesweeper::open(int i, int j)
{
    // functia deschide elementul (i,j)
    if(matrix[i][j].getType()==empty)
    {
        cascade(matrix[i][j]);
    }
    else if(matrix[i][j].getType()==digit)
    {
        matrix[i][j].flip();
    }

}

void Minesweeper::setEmptyButtons()
{
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
        {
            if(matrix[i][j].getType() == none)
            {
                matrix[i][j].setType(empty);
            }
        }
}

void Minesweeper::updateLCD1()
{
    ui->lcd1->display(this->nrBombs - this->discoveredBombs);
}

void Minesweeper::cascade(Card card)
{
    QStack<Card> stack;
    stack.push(card);

    while(stack.size()!=0)
    {
        Card top = stack.top();
        int i = top.getRow();
        int j = top.getColumn();
        stack.pop();

        if(matrix[i][j].getFlag()==true)
        {
            continue;
        }


        if(matrix[i][j].getType()!=empty || matrix[i][j].getFlipped()==true)
        {
            matrix[i][j].flip();
            continue;
        }

        matrix[i][j].flip();

        if(i>0 && j>0)
        {
            stack.push(matrix[i-1][j-1]);
        }
        if(i>0)
        {
            stack.push(matrix[i-1][j]);
        }
        if(i>0 && j<this->columns-1)
        {
            stack.push(matrix[i-1][j+1]);
        }
        if(j>0)
        {
            stack.push(matrix[i][j-1]);
        }
        if(j<this->columns-1)
        {
            stack.push(matrix[i][j+1]);
        }
        if(i<this->rows-1 && j>0)
        {
            stack.push(matrix[i+1][j-1]);
        }
        if(i<this->rows-1)
        {
            stack.push(matrix[i+1][j]);
        }
        if(i<this->rows-1 && j <this->rows-1)
        {
            stack.push(matrix[i+1][j+1]);
        }
    }


}

void Minesweeper::updateLCD2()
{
    seconds++;
    ui->lcd2->display(seconds);

}

void Minesweeper::lostGame()
{
    QPixmap pixmap("E:\\workspace - qt\\c++ tema_3\\Resources\\sad.png");
    QIcon ButtonIcon(pixmap);
    QSize iconSize(30,30);
    ui->smileBtn->setIcon(ButtonIcon);
    ui->smileBtn->setIconSize(pixmap.rect().size());
    ui->smileBtn->setIconSize(iconSize);

    statusGame = false;
    timer->stop();
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
        {
            if(matrix[i][j].getType() == bomb)
            {
                matrix[i][j].flip();
            }
        }
}

void Minesweeper::wonGame()
{
    bool ok = true;


    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
        {
            if(matrix[i][j].getType()!=bomb)
            {
                if(matrix[i][j].getFlipped()==false)
                {
                    ok = false;
                }
            }
        }

    if(ok)
    {
        statusGame = false;
        timer->stop();
        for(int i=0;i<rows;i++)
            for(int j=0;j<columns;j++)
            {
                if(matrix[i][j].getType()==bomb)
                {
                    matrix[i][j].setIcon("E:\\workspace - qt\\c++ tema_3\\Resources\\flag.png");
                    matrix[i][j].setFlag(true);
                }
            }

        discoveredBombs=nrBombs;
        updateLCD1();

        QPixmap pixmap("E:\\workspace - qt\\c++ tema_3\\Resources\\love.png");
        QIcon ButtonIcon(pixmap);
        QSize iconSize(30,30);
        ui->smileBtn->setIcon(ButtonIcon);
        ui->smileBtn->setIconSize(pixmap.rect().size());
        ui->smileBtn->setIconSize(iconSize);
    }

}

void Minesweeper::on_custom_triggered()
{
    clear();
    rows = QInputDialog::getInt(this, tr("Custom"),tr("Rows:"), QLineEdit::Normal,5,30);
    columns = QInputDialog::getInt(this, tr("Custom"),tr("Columns:"), QLineEdit::Normal,5,30);
    nrBombs = QInputDialog::getInt(this, tr("Custom"),tr("Bombs:"), QLineEdit::Normal,1,rows*columns-1);
    // crapa daca rows > columns
    initialize();
}
