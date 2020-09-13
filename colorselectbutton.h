#ifndef COLORSELECTBUTTON_H
#define COLORSELECTBUTTON_H

#include <QRadioButton>

class ColorSelectButton : public QRadioButton {
Q_OBJECT

public:
    ColorSelectButton(QColor color, QWidget* parent = nullptr);
    QColor getColor();
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    QSize sizeHint() const override;

private:
    QColor color_;
};

class ColorSelectButtonsHandler : public QWidget {
    Q_OBJECT

public:
    ColorSelectButtonsHandler(QVector<QColor> colors, QWidget* parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
    QColor getSelectedColor();
    QSize sizeHint() const override;

private slots:
    void selectColor();

private:
    QVector<QColor> colors_;
    QColor selectedColor_;
    QVector<ColorSelectButton*> buttons_;
};

#endif // COLORSELECTBUTTON_H
