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
    connect(this->ui->menuButton, SIGNAL(clicked()), this, SLOT(returnToMenu()));
    connect(this->ui->newGameButton, SIGNAL(clicked()), this, SLOT(newGame()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::playGame() {
    this->ui->stackedWidget->setCurrentIndex(1);
    removeCurrentGameIfExists();
    if(!this->currentGame_) {
        this->currentGame_ = new Game(this, this->ui->page_2);
        addWidgetToPage("page_2", this->currentGame_);
    }
}

void MainWindow::returnToMenu() {
    this->ui->stackedWidget->setCurrentIndex(0);
    removeCurrentGameIfExists();
}

void MainWindow::newGame() {
    assert(this->currentGame_);
    delete this->currentGame_;
    this->currentGame_ = new Game(this, this->ui->page_2);
    addWidgetToPage("page_2", this->currentGame_);
}

void MainWindow::addWidgetToPage(QString pageName, QWidget* widget) {
    qobject_cast<QVBoxLayout*>(this->findChild<QWidget*>(pageName)->layout())->addWidget(widget);
}

void MainWindow::removeCurrentGameIfExists() {
    if(this->currentGame_) {
        if(this->currentGame_->hasEnded) {
            delete this->currentGame_;
            this->currentGame_ = nullptr;
        }
    }
}
