#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>

class Game : public QObject {
    Q_OBJECT

public:
    Game(QWidget *parent = nullptr);
    QWidget* getContent();

private slots:
    void putColorGuess();
    void selectColor();
    void submitGuess();

private:
    QVector<QColor> colors_;
    QColor selectedColor_;
    int numCurrentGuess_;
    QVector<QColor> code_;
    QWidget* uiContent_;
    QGridLayout* uiGridLayout_;

    void generateCode();
    QVector<int> checkGuess(QVector<QColor> guess);
    void wonGame();
    void lostGame();
};

#endif // GAME_H
