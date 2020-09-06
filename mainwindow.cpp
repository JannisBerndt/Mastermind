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
    this->setStyleSheet("border:3px solid orange;"
                        "border-radius: 15px;"
                        "background-color: black;");
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->color_ = QColor(0,0,0);
    this->setMinimumSize(QSize(30,30));
    QSizePolicy sp = this->sizePolicy();
    sp.setWidthForHeight(true);
    this->setSizePolicy(sp);
}

QColor GuessButton::getColor() {
    return this->color_;
}

void GuessButton::setColor(QColor newColor) {
    this->color_ = newColor;
    this->setStyleSheet("border:3px solid orange;"
                        "border-radius: 15px;"
                        "background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.35, fy:0.35, stop:0 rgb(255, 255, 255), stop:1 " + newColor.name() + ");");
}

int GuessButton::widthForHeight(int h) const {
    return h;
}

void GuessButton::resizeEvent(QResizeEvent *event) {
    if(true) {
        int dim = qMin(this->width(), this->height());
        int oldWidth = this->width();
        qInfo() << dim << ", " << event->oldSize() << ", " << event->size() << ", " << (event->oldSize() != event->size());
        this->resize(dim, dim);
        this->move(this->x()+(oldWidth-dim)/2, this->y());
        if(this->color_ == QColor(0,0,0)) {
            this->setStyleSheet("border:3px solid orange;"
                                "border-radius: " + QString::number(dim/2) + "px;"
                                "background-color: black;");
        } else {
            this->setStyleSheet("border:3px solid orange;"
                                "border-radius: " + QString::number(dim/2) + "px;"
                                "background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.35, fy:0.35, stop:0 rgb(255, 255, 255), stop:1 " + this->color_.name() + ");");
        }
    }
    QWidget::resizeEvent(event);
}

void GuessButton::paintEvent(QPaintEvent *event) {
    QPushButton::paintEvent(event);
}

QSize GuessButton::sizeHint() const {
    QSize originalHint = QWidget::sizeHint();
    int dim = qMax(this->width(), this->height());
    qInfo() << "GuessButton::sizeHint, dim = " << dim;
    return originalHint;
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
    this->setObjectName("color");
    this->setStyleSheet("::indicator {height: 30px; width: 30px; border: 3px solid black; border-radius: 18px; background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.35, fy:0.35, stop:0 rgb(255, 255, 255), stop:1 " + color_.name() + ");}"
                        "::indicator:checked {border: 3px solid #62ff08;}"
                        "#color {border: 2px solid black;}");
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setColor(Qt::black);
    shadow->setBlurRadius(10);
    shadow->setOffset(QPointF(0, 0));
    this->setGraphicsEffect(shadow);
    this->setCursor(Qt::PointingHandCursor);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum));
}

QColor ColorSelectButton::getColor()  {
    return this->color_;
}

void ColorSelectButton::paintEvent(QPaintEvent *event) {
    QRadioButton::paintEvent(event);
}

void ColorSelectButton::resizeEvent(QResizeEvent *event) {
    qInfo() << "ColorSelectButton::resizeEvent, height = " << this->height();
    //this->resize(this->height_, this->height_);
    QWidget::resizeEvent(event);
}

QSize ColorSelectButton::sizeHint() const {
    qInfo() << "ColorSelectButton::sizeHint";
    return QWidget::sizeHint();
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
