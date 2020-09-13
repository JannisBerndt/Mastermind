#include <QPainter>
#include <QPropertyAnimation>
#include <QMoveEvent>

#include "solution.h"
#include "game.h"

Pin::Pin(QColor color, QWidget* parent) : QWidget(parent), color_(color) {
    this->setMinimumSize(30, 30);
}

void Pin::setColor(QColor newColor) {
    this->color_ = newColor;
}

QColor Pin::getColor() {
    return this->color_;
}

void Pin::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(QRectF(0, 0, this->width(), this->height()));
    QRadialGradient radialGradient(QPointF(this->width()/2, this->height()/2), this->height()/2, QPointF(this->width()/2.5, this->height()/2.5));
    radialGradient.setColorAt(0, Qt::white);
    radialGradient.setColorAt(1, this->color_);
    painter.fillPath(path, radialGradient);
}

Solution::Solution(Game* parent) : QWidget(parent), code_(parent->getCode()) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 7, 0, 7);
    for(auto color : this->code_) {
        Pin* pin = new Pin(color, this);
        pin->show();
        this->pins_.push_back(pin);
        layout->addWidget(pin);
    }
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    this->show();
    this->setMaximumHeight(0);
    this->pos_ = this->pos();
}

QSize Solution::sizeHint() const {
    int height = this->layout()->contentsMargins().top() + this->layout()->contentsMargins().bottom() + qobject_cast<Game*>(this->parentWidget())->getCurrentPinHeight();
    return QSize(QWidget::sizeHint().width(), height);
}

void Solution::resizeEvent(QResizeEvent *event) {
    int height = qobject_cast<Game*>(this->parentWidget())->getCurrentPinHeight();
    for(auto pin : this->pins_) {
        pin->setFixedSize(height, height);
    }
    QWidget::resizeEvent(event);
}

void Solution::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.drawRect(0, 0, this->width()-1, this->height()-1);
}

void Solution::moveEvent(QMoveEvent *event) {
    QWidget::moveEvent(event);
    if(!(this->pos_ == event->pos())) {
        this->move(this->pos_);
    }
}

void Solution::reveal() {
    int height = this->layout()->contentsMargins().top() + this->layout()->contentsMargins().bottom() + qobject_cast<Game*>(this->parentWidget())->getCurrentPinHeight();
    QPropertyAnimation* animation = new QPropertyAnimation(this, "minimumHeight");
    animation->setDuration(1500);
    animation->setStartValue(this->height());
    animation->setEndValue(height);
    animation->start();
    connect(animation, SIGNAL(finished()), this, SLOT(afterAnimation()));
}

void Solution::afterAnimation() {
    this->setMaximumHeight(16777215);
}
