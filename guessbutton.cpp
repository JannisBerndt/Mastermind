#include "guessbutton.h"

#include <QPainter>
#include <QHBoxLayout>

const QColor GuessButton::colorEmpty = QColor(0, 0, 0);

GuessButton::GuessButton(QWidget *parent) : QPushButton(parent) {
    this->color_ = this->colorEmpty;
    this->setMinimumSize(QSize(30,30));
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QColor GuessButton::getColor() {
    return this->color_;
}

void GuessButton::setColor(QColor newColor) {
    this->color_ = newColor;
    this->repaint();
}

void GuessButton::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(QRectF(2, 2, this->width()-4, this->height()-4));
    if(this->isDown()) {
        painter.setPen(QPen(QColor(98, 255, 8), 4));
    } else {
        painter.setPen(QPen(QColor(255, 157, 10), 4));
    }
    if(this->color_ != this->colorEmpty) {
        QRadialGradient radialGradient(QPointF(this->width()/2, this->height()/2), this->height()/2, QPointF(this->width()/2.5, this->height()/2.5));
        radialGradient.setColorAt(0, Qt::white);
        radialGradient.setColorAt(1, this->color_);
        painter.fillPath(path, radialGradient);
    } else {
        QRadialGradient radialGradient(QPointF(this->width()/2, this->height()/2), this->height()/2);
        radialGradient.setColorAt(0, this->color_);
        radialGradient.setColorAt(1, Qt::white);
        painter.fillPath(path, radialGradient);
    }
    painter.drawPath(path);
}

QSize GuessButton::sizeHint() const {
    int length = qMin(this->parentWidget()->height(), (this->parentWidget()->width() - (this->parentWidget()->layout()->spacing()*3))/4);
    return QSize(length, length);
}

GuessButtonsHandler::GuessButtonsHandler(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    for(int i = 0; i < 4; i++) {
        GuessButton* button = new GuessButton(this);
        this->buttons_.push_back(button);
        layout->addWidget(button);
    }
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
}

QVector<GuessButton*> GuessButtonsHandler::getButtons() {
    return this->buttons_;
}

void GuessButtonsHandler::resizeEvent(QResizeEvent *event) {
    for(auto button : this->buttons_) {
        button->updateGeometry();
    }
    QWidget::resizeEvent(event);
}

void GuessButtonsHandler::forEveryButtonDo(void (*func)(GuessButton *)) {
    for(auto it = this->buttons_.begin(); it != this->buttons_.end(); it++) {
        func(*it);
    }
}

QSize GuessButtonsHandler::sizeHint() const {
    QSize originalSizeHint = QWidget::sizeHint();
    int height = (this->parentWidget()->parentWidget()->height() - this->parentWidget()->layout()->contentsMargins().top()*2 - this->parentWidget()->layout()->spacing()*11)/12;
    //qInfo() << height;
    return QSize(originalSizeHint.width(), height);
}

QVector<QColor> GuessButtonsHandler::getFullGuess() {
    QVector<QColor> colors;
    for(auto button : this->buttons_) {
        colors.push_back(button->getColor());
    }
    return colors;
}

bool GuessButtonsHandler::isCurrentGuessValid() {
    auto colors = this->getFullGuess();
    bool valid = std::accumulate(colors.begin(), colors.end(), true, [](bool rest, QColor color) { return rest && color != GuessButton::colorEmpty; } );
    return valid;
}
