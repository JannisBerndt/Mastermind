#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "game.h"
#include <QDebug>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

GuessButton::GuessButton(QWidget *parent) : QPushButton(parent) {
    this->setFixedWidth(30);
    this->setFixedHeight(30);
    this->setStyleSheet("border:3px solid orange;"
                        "border-radius: 15px;"
                        "background-color: black;");
    this->color_ = QColor(0,0,0);
}

QColor GuessButton::getColor() {
    return this->color_;
}

void GuessButton::setColor(QColor newColor) {
    this->color_ = newColor;
    this->setStyleSheet("border:3px solid orange;"
                        "border-radius: 15px;"
                        "background-color: " + newColor.name() + ";");
}

int GuessButton::heightForWidth(int w) const {
    return w;
}

void GuessButton::resizeEvent(QResizeEvent *event) {
    QPushButton::resizeEvent(event);
    //this->setStyleSheet(QStringLiteral("border-radius: %1px;"
    //                                   "background-color: orange;").arg(this->width()/2));
}

void MainWindow::on_pushButton_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(1);
    Game* currentGame = new Game(this);
    addWidgetToPage("page_2", currentGame->getContent());
}

void MainWindow::addWidgetToPage(QString pageName, QWidget* widget) {
    this->findChild<QWidget*>(pageName)->layout()->addWidget(widget);
}

ColorSelectButton::ColorSelectButton(QColor color, QWidget* parent) : QRadioButton(parent), color_(color) {
    this->setFixedWidth(36);
    this->setFixedHeight(36);
    this->setStyleSheet("::indicator {height: 30px; width: 30px; padding: 3px; border-radius: 18px; background-color: " + color_.name() + ";}"
                        "::indicator:checked {border: 3px solid green; padding: 0px;}");
}

QColor ColorSelectButton::getColor()  {
    return this->color_;
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
    painter.fillRect(0, 0, length, length, neutral);
    qInfo() << this->hint_[0] << ", " << this->hint_[1] << ", " << this->hint_[2] << ", " << this->hint_[3];
    for(int i = 0; i < this->hint_.length()/2; i++) {
        for(int j = 0; j < this->hint_.length()/2; j++) {
            QColor color;
            int index = i + ((i>0)? j+1 : j);
            qInfo() << index;
            switch(this->hint_[index]) {
            case -1: color = neutral; break;
            case 0: color = Qt::white; break;
            case 1: color = Qt::black; break;
            default: color = Qt::red; break;
            }
            painter.setBrush(color);
            painter.setPen(color);
            if (color != neutral) painter.drawEllipse(QPointF(length/4 + length/2*i, length/4 + length/2*j), length/4 - 2, length/4 - 2);
        }
    }
}
