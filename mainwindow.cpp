#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QRandomGenerator>
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
    // Fill all 12 rows
    for(int i = 0; i < 12; i++) {
        QLabel* label = new QLabel();
        label->setText(QString::number(12-i));
        label->setFixedWidth(15);
        this->ui->gridLayout->addWidget(label, i, 0);

        for(int j = 1; j < 5; j++) {
            GuessButton* button = new GuessButton();
            button->sizePolicy().setWidthForHeight(true);
            if(i != 11) button->setEnabled(false);
            connect(button, SIGNAL(clicked()),this, SLOT(fillColor()));
            this->ui->gridLayout->addWidget(button, i, j, Qt::AlignCenter);
        }
        QWidget* placeholder = new QWidget();
        placeholder->setFixedWidth(50);
        this->ui->gridLayout->addWidget(placeholder, i, 5);
    }

    this->colors_ = {QColor(222, 222, 222), QColor(168, 17, 17), QColor(245, 241, 10), QColor(11, 128, 37), QColor(11, 32, 219), QColor(219, 11, 181)};
    for(int i = 0; i < 6; i++) {
        ColorSelectButton* button = new ColorSelectButton(colors_[i], this);
        if(i == 0) {
            button->setChecked(true);
            this->currentColor_ = colors_[i];
        }
        connect(button, SIGNAL(clicked()),this, SLOT(pickColor()));
        this->ui->horizontalLayout_2->addWidget(button);
    }
    this->currentTry_ = 1;
    this->generateCode();
}

void MainWindow::fillColor() {
    GuessButton* button = qobject_cast<GuessButton*>(sender());
    button->setColor(this->currentColor_);
    // check if all four are filled
    QVector<QColor> colors;
    for(int i = 1; i < 5; i++) {
        colors.push_back(qobject_cast<GuessButton*>(this->ui->gridLayout->itemAtPosition(12 - this->currentTry_, i)->widget())->getColor());
    }
    bool valid = std::accumulate(colors.begin(), colors.end(), true, [](bool rest, QColor color) { return rest && color != QColor(0,0,0); } );
    if(valid) {
        // create "OK" Button
        QPushButton* button = new QPushButton("OK");
        button->setFixedWidth(50);
        connect(button, SIGNAL(clicked()),this, SLOT(submitGuess()));
        this->ui->gridLayout->addWidget(button, 12 - this->currentTry_, 5);
    }
}

void MainWindow::pickColor() {
    ColorSelectButton* button = qobject_cast<ColorSelectButton*>(sender());
    this->currentColor_ = button->getColor();
}

void MainWindow::submitGuess() {
    if(this->currentTry_ == 12) {
        // Submitted the last Try

    } else {
        QVector<QColor> guess;
        for(int i = 1; i < 5; i++) {
            guess.push_back(qobject_cast<GuessButton*>(this->ui->gridLayout->itemAtPosition(12 - this->currentTry_, i)->widget())->getColor());
        }
        for(int i = 1; i < 5; i++) {
            this->ui->gridLayout->itemAtPosition(12 - this->currentTry_, i)->widget()->setEnabled(false);
            this->ui->gridLayout->itemAtPosition(12 - this->currentTry_ - 1, i)->widget()->setEnabled(true);
        }
        auto hint = this->checkGuess(guess);
        this->ui->gridLayout->removeWidget(qobject_cast<QWidget*>(sender()));
        delete sender();
        // print hint
        HintViewer* hints = new HintViewer(hint);
        //hints->setFixedWidth(32);
        //hints->setFixedHeight(32);
        this->ui->gridLayout->addWidget(hints, 12 - this->currentTry_, 5);
        this->currentTry_++;
    }
}

void MainWindow::generateCode() {
    for(int i = 0; i < 4; i++) {
        this->code_.push_back(this->colors_[QRandomGenerator::global()->generate()%6]);
    }
}

QVector<int> MainWindow::checkGuess(QVector<QColor> guess) {
    // -1 is no match, 0 is inside code but not the correct position, 1 correct position
    assert(guess.length() == this->code_.length());
    QVector<int> hint;
    QVector<QColor> code_copy(this->code_);
    for(int i = 0; i < guess.length(); i++) {
        qInfo() << guess[i].name() << " | " << code_copy[i].name();
        if(code_copy[i] == guess[i]) {
            code_copy.removeAt(i);
            guess.removeAt(i);
            hint.push_back(1);
            i--;
        }
    }
    qInfo() << "-----------------------------------";
    for(int i = 0; i < guess.length(); i++) {
        if(code_copy.contains(guess[i])) {
            code_copy.removeOne(guess[i]);
            guess.removeAt(i);
            hint.push_back(0);
            i--;
        }
    }
    assert(this->code_.length() - hint.length() == guess.length());
    for(int i = 0; i < guess.length(); i++) {
        hint.push_back(-1);
    }
    return hint;
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
