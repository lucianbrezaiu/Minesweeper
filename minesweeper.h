#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <QMainWindow>
#include <QPushButton>
#include <card.h>
#include <QTimer>

namespace Ui
{
    class Minesweeper;
}

class Minesweeper : public QMainWindow
{
    Q_OBJECT

public:
    explicit Minesweeper(QWidget *parent = 0);
    ~Minesweeper();

    void initialize();
    void clear();

    void setBombButtons();
    void setDigitButtons();
    void setEmptyButtons();

    int numberBombsForElement(int, int);
    int numberFlagsForElement(int, int);
    bool checkBombForFlags(int,int);
    void openNeighbors(int,int);
    void open(int i, int j);
    void updateLCD1();

    void cascade(Card card);
    void lostGame();
    void wonGame();
private slots:
    void on_newGame_triggered();

    void on_exit_triggered();

    void on_easy_triggered();

    void on_medium_triggered();

    void on_hard_triggered();

    void leftClickEvent();

    void rightClickEvent();

    void doubleClickEvent();



    void updateLCD2();
    void on_custom_triggered();

private:

    QTimer *timer;
    Card** matrix;
    int nrBombs;
    int discoveredBombs;
    int rows;
    int columns;
    bool statusGame;
    int seconds;

    Ui::Minesweeper *ui;
};

#endif // MINESWEEPER_H
