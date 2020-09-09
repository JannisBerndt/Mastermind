#ifndef GUESSBUTTON_H
#define GUESSBUTTON_H

#include <QPushButton>

class GuessButton : public QPushButton {
    Q_OBJECT

public:
    GuessButton(QWidget *parent = nullptr);
    QColor getColor();
    void setColor(QColor newColor);
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;
    static const QColor colorEmpty;

private:
    QColor color_;
};

class GuessButtonsHandler : public QWidget {
    Q_OBJECT

public:
    GuessButtonsHandler(QWidget* parent = nullptr);
    QVector<GuessButton*> getButtons();
    void resizeEvent(QResizeEvent *event) override;
    void forEveryButtonDo(void (*func) (GuessButton*));
    QSize sizeHint() const override;
    QVector<QColor> getFullGuess();
    bool isCurrentGuessValid();

private:
    QVector<GuessButton*> buttons_;
};

#endif // GUESSBUTTON_H
