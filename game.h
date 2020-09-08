#ifndef GAME_H
#define GAME_H

#include "mainwindow.h"
#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>

class EndScreen : public QWidget {
    Q_OBJECT

public:
    EndScreen(bool hasWon, QWidget* parent = nullptr);
    QSize sizeHint() const override;
    void paintEvent(QPaintEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    QWidget* getBlocker();

private:
    QWidget* blocker_;
};

class ColorSelectButtonsHandler : public QWidget {
    Q_OBJECT

public:
    ColorSelectButtonsHandler(QVector<QColor> colors, QWidget* parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
    QColor getSelectedColor();
    QSize sizeHint() const override;
    void setHeight(int newHeight);

private slots:
    void selectColor();

private:
    QVector<QColor> colors_;
    QColor selectedColor_;
    QVector<ColorSelectButton*> buttons_;
    int height_;
};

class Game : public QWidget {
    Q_OBJECT

public:
    Game(QMainWindow* mainwindow, QWidget *parent = nullptr);
    QLayout* getContentLayout();
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void putColorGuess();
    void submitGuess();

private:
    QVector<QColor> colors_;
    int numCurrentGuess_;
    QVector<QColor> code_;
    QLayout* uiContentLayout_;
    QGridLayout* uiGridLayout_;
    EndScreen* endScreen_ = nullptr;
    QMainWindow* mainwindow_;
    ColorSelectButtonsHandler* colorButtonsHandler_;

    void generateCode();
    QVector<int> checkGuess(QVector<QColor> guess);
    void endGame(bool hasWon);
    void paintEvent(QPaintEvent *event) override;
};

#endif // GAME_H
