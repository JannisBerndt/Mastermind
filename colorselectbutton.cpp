#include "colorselectbutton.h"
#include "ui_mainwindow.h"
#include "game.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

ColorSelectButton::ColorSelectButton(QColor color, QWidget* parent) : QRadioButton(parent), color_(color) {
    this->setMinimumWidth(30);
    this->setMinimumHeight(30);
    this->setStyleSheet("::indicator {width: " + QString::number(this->width()) + "; height: " + QString::number(this->height()) + ";}");
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setColor(Qt::black);
    shadow->setBlurRadius(10);
    shadow->setOffset(QPointF(0, 0));
    this->setGraphicsEffect(shadow);
    this->setCursor(Qt::PointingHandCursor);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QColor ColorSelectButton::getColor()  {
    return this->color_;
}

void ColorSelectButton::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(QRectF(2, 2, this->width()-4, this->height()-4));
    if(this->isChecked()) {
        painter.setPen(QPen(QColor(98, 255, 8), 4));
    } else {
        painter.setPen(QPen(QColor(0, 0, 0), 4));
    }
    QRadialGradient radialGradient(QPointF(this->width()/2, this->height()/2), this->height()/2, QPointF(this->width()/2.5, this->height()/2.5));
    radialGradient.setColorAt(0, Qt::white);
    radialGradient.setColorAt(1, this->color_);
    painter.fillPath(path, radialGradient);
    painter.drawPath(path);
}

void ColorSelectButton::resizeEvent(QResizeEvent *event) {
    this->setStyleSheet("::indicator {width: " + QString::number(this->width()) + "; height: " + QString::number(this->height()) + ";}");
    QRadioButton::resizeEvent(event);
}

QSize ColorSelectButton::sizeHint() const {
    int parentVerticalMargins = this->parentWidget()->layout()->contentsMargins().top() + this->parentWidget()->layout()->contentsMargins().bottom();
    int length = qMin(this->parentWidget()->height()-parentVerticalMargins, (this->parentWidget()->width() - (this->parentWidget()->layout()->spacing()*5))/6);
    return QSize(length, length);
}

ColorSelectButtonsHandler::ColorSelectButtonsHandler(QVector<QColor> colors, QWidget* parent) : QWidget(parent) {
    this->colors_ = colors;
    QHBoxLayout* colorSelectButtonsLayout = new QHBoxLayout(this);
    colorSelectButtonsLayout->setContentsMargins(0, 7, 0, 7);
    for(int i = 0; i < colors.length(); i++) {
        ColorSelectButton* button = new ColorSelectButton(this->colors_[i], this);
        if(i == 0) {
            button->setChecked(true);
            this->selectedColor_ = colors_[i];
        }
        connect(button, SIGNAL(clicked()),this, SLOT(selectColor()));
        this->buttons_.push_back(button);
        colorSelectButtonsLayout->addWidget(button);
    }
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

void ColorSelectButtonsHandler::selectColor() {
    ColorSelectButton* button = qobject_cast<ColorSelectButton*>(sender());
    this->selectedColor_ = button->getColor();
}

QColor ColorSelectButtonsHandler::getSelectedColor() {
    return this->selectedColor_;
}

void ColorSelectButtonsHandler::resizeEvent(QResizeEvent *event) {
    for(auto button : this->buttons_) {
        button->updateGeometry();
        //qInfo() << button->width() - button->height();
    }
    QWidget::resizeEvent(event);
}

QSize ColorSelectButtonsHandler::sizeHint() const {
    int height = this->layout()->contentsMargins().top() + this->layout()->contentsMargins().bottom() + qobject_cast<Game*>(this->parentWidget())->getCurrentPinHeight();
    return QSize(this->width(), height);
}
