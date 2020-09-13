#ifndef SOLUTION_H
#define SOLUTION_H

#include <QWidget>
class Game;
#include "game.h"

class Pin : public QWidget {
    Q_OBJECT

public:
    Pin(QColor color = Qt::black, QWidget* parent = nullptr);
    void setColor(QColor newColor);
    QColor getColor();
    void paintEvent(QPaintEvent* event) override;

private:
    QColor color_;
};

class Solution : public QWidget {
    Q_OBJECT

public:
    Solution(Game* parent = nullptr);
    QSize sizeHint() const override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void reveal();

private slots:
    void afterAnimation();

private:
    QVector<QColor> code_;
    QVector<Pin*> pins_;
    QPoint pos_;
};

#endif // SOLUTION_H
