#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Game;
#include "game.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addWidgetToPage(QString pageName, QWidget* widget);

private slots:
    void playGame();
    void returnToMenu();
    void newGame();

private:
    Ui::MainWindow *ui;
    Game* currentGame_ = nullptr;
    void removeCurrentGameIfExists();
};

#endif // MAINWINDOW_H
