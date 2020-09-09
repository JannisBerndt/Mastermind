#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "game.h"
#include <QDebug>
#include <QPainter>
#include <QGraphicsEffect>
#include <QLabel>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Mastermind");
    this->ui->stackedWidget->setCurrentIndex(0);
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setColor(QColor(255, 156, 8));
    shadow->setBlurRadius(10);
    shadow->setOffset(QPointF(0, 0));
    this->ui->playButton->setGraphicsEffect(shadow);
    QGraphicsDropShadowEffect* shadow2 = new QGraphicsDropShadowEffect;
    shadow2->setColor(QColor(255, 156, 8));
    shadow2->setBlurRadius(10);
    shadow2->setOffset(QPointF(0, 0));
    this->ui->settingsButton->setGraphicsEffect(shadow2);
    connect(this->ui->playButton, SIGNAL(clicked()), this, SLOT(playGame()));
    this->setObjectName("mainwindow");
}

MainWindow::~MainWindow()
{
    delete ui;
}

GuessButton::GuessButton(QWidget *parent) : QPushButton(parent) {
    this->color_ = QColor(0,0,0);
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

void GuessButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(QRectF(2, 2, this->width()-4, this->height()-4));
    if(this->isDown()) {
        painter.setPen(QPen(QColor(98, 255, 8), 4));
    } else {
        painter.setPen(QPen(QColor(255, 157, 10), 4));
    }
    if(this->color_ != QColor(0, 0, 0)) {
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

GuessButtonContainer::GuessButtonContainer(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    for(int i = 0; i < 4; i++) {
        GuessButton* button = new GuessButton(this);
        this->buttons_.push_back(button);
        layout->addWidget(button);
    }
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
}

QVector<GuessButton*> GuessButtonContainer::getButtons() {
    return this->buttons_;
}

void GuessButtonContainer::resizeEvent(QResizeEvent *event) {
    for(auto button : this->buttons_) {
        button->updateGeometry();
    }
    QWidget::resizeEvent(event);
}

void GuessButtonContainer::forEveryButtonDo(void (*func)(GuessButton *)) {
    for(auto it = this->buttons_.begin(); it != this->buttons_.end(); it++) {
        func(*it);
    }
}

QSize GuessButtonContainer::sizeHint() const {
    QSize originalSizeHint = QWidget::sizeHint();
    int height = (this->parentWidget()->parentWidget()->height() - this->parentWidget()->layout()->contentsMargins().top()*2 - this->parentWidget()->layout()->spacing()*11)/12;
    //qInfo() << height;
    return QSize(originalSizeHint.width(), height);
}

QVector<QColor> GuessButtonContainer::getFullGuess() {
    QVector<QColor> colors;
    for(auto button : this->buttons_) {
        colors.push_back(button->getColor());
    }
    return colors;
}

bool GuessButtonContainer::isCurrentGuessValid() {
    auto colors = this->getFullGuess();
    bool valid = std::accumulate(colors.begin(), colors.end(), true, [](bool rest, QColor color) { return rest && color != QColor(0,0,0); } );
    return valid;
}

void MainWindow::playGame() {
    this->ui->stackedWidget->setCurrentIndex(1);
    Game* currentGame = new Game(this, this->ui->page_2);
    addWidgetToPage("page_2", currentGame);
}

void MainWindow::addWidgetToPage(QString pageName, QWidget* widget) {
    qobject_cast<QVBoxLayout*>(this->findChild<QWidget*>(pageName)->layout())->addWidget(widget);
}

ColorSelectButton::ColorSelectButton(QColor color, QWidget* parent) : QRadioButton(parent), color_(color) {
    this->setMinimumWidth(30);
    this->setMinimumHeight(30);
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

void ColorSelectButton::paintEvent(QPaintEvent *event) {
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

QSize ColorSelectButton::sizeHint() const {
    int parentVerticalMargins = this->parentWidget()->layout()->contentsMargins().top() + this->parentWidget()->layout()->contentsMargins().bottom();
    int length = qMin(this->parentWidget()->height()-parentVerticalMargins, (this->parentWidget()->width() - (this->parentWidget()->layout()->spacing()*5))/6);
    return QSize(length, length);
}

HintViewer::HintViewer(QVector<int> hint, QWidget* parent) : QWidget(parent), hint_(hint) {

}

void HintViewer::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    QColor neutral = Qt::green;
    int width = size().width();
    int height = size().height();
    int length = qMin(width, height);
    painter.fillRect((width-length)/2, 0, length, length, neutral);
    qInfo() << this->hint_[0] << ", " << this->hint_[1] << ", " << this->hint_[2] << ", " << this->hint_[3];
    for(int i = 0; i < this->hint_.length()/2; i++) {
        for(int j = 0; j < this->hint_.length()/2; j++) {
            QColor color;
            int index = i + ((i>0)? j+1 : j);
            switch(this->hint_[index]) {
            case -1: color = neutral; break;
            case 0: color = Qt::white; break;
            case 1: color = Qt::black; break;
            default: color = Qt::red; break;
            }
            painter.setBrush(color);
            painter.setPen(color);
            if (color != neutral) painter.drawEllipse(QPointF((width-length)/2 + length/4 + length/2*i, 0 + length/4 + length/2*j), length/4 - 2, length/4 - 2);
        }
    }
}
