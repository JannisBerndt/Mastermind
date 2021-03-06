#include "game.h"
#include "guessbutton.h"
#include "mainwindow.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

Game::Game(QMainWindow* mainwindow, QWidget *parent) : QWidget(parent) {
    QVector<QColor> colors = {QColor(222, 222, 222), QColor(168, 17, 17), QColor(245, 241, 10), QColor(11, 128, 37), QColor(11, 32, 219), QColor(219, 11, 181)};
    this->colors_ = colors;
    this->generateCode();
    this->setObjectName("game");
    this->mainwindow_ = mainwindow;
    QVBoxLayout* contentLayout = new QVBoxLayout(this); // already does this->setLayout(contentLayout);
    QScrollArea* scrollArea = new QScrollArea(this);
    QWidget* scrollAreaWidget = new QWidget(scrollArea);
    QGridLayout* gridLayout = new QGridLayout(scrollAreaWidget); // already does scrollAreaContentWidget->setLayout(gridLayout);
    Solution* solution = new Solution(this);
    scrollArea->setWidget(scrollAreaWidget);
    scrollArea->setWidgetResizable(true);
    scrollAreaWidget->setVisible(true);
    contentLayout->addWidget(solution);
    contentLayout->addWidget(scrollArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    this->uiContentLayout_ = contentLayout;
    this->uiGridLayout_ = gridLayout;
    this->solution_ = solution;
    //solution->hide();

    for(int i = 0; i < 12; i++) {
        QLabel* label = new QLabel(scrollAreaWidget);
        label->setText(QString::number(12-i));
        label->setFixedWidth(15);
        gridLayout->addWidget(label, i, 0);

        GuessButtonsHandler* buttonContainer = new GuessButtonsHandler(scrollAreaWidget);
        if(i != 11) buttonContainer->forEveryButtonDo([](GuessButton* button) {button->setEnabled(false);} );
        for(auto button : buttonContainer->getButtons()) {
            connect(button, SIGNAL(clicked()), this, SLOT(putColorGuess()));
        }
        gridLayout->addWidget(buttonContainer, i, 1);

        QPushButton* button = new QPushButton(scrollAreaWidget);
        button->setText("Submit");
        button->setFixedWidth(50);
        button->setFixedHeight(30);
        button->setStyleSheet("QPushButton {"
                                "padding: 5px;"
                                "font: 8pt 'MS Shell Dlg 2';"
                                "border: 2px solid #ff9c08;"
                                "background-color: #ffb108;"
                                "border-radius: 15px;}"
                              ":pressed {"
                                "background-color: #ffa908;"
                              "}");
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
        shadow->setColor(QColor(255, 156, 8));
        shadow->setBlurRadius(10);
        shadow->setOffset(QPointF(0, 0));
        button->setGraphicsEffect(shadow);
        connect(button, SIGNAL(clicked()),this, SLOT(submitGuess()));
        this->uiGridLayout_->addWidget(button, i, 2, Qt::AlignCenter);
        QSizePolicy sp = button->sizePolicy();
        sp.setRetainSizeWhenHidden(true);
        button->setSizePolicy(sp);
        button->setVisible(false);
    }

    this->colorButtonsHandler_ = new ColorSelectButtonsHandler(colors, this);
    contentLayout->addWidget(this->colorButtonsHandler_);
    this->numCurrentGuess_ = 1;
}

QLayout* Game::getContentLayout() {
    return this->uiContentLayout_;
}

void Game::putColorGuess() {
    GuessButton* button = qobject_cast<GuessButton*>(sender());
    button->setColor(this->colorButtonsHandler_->getSelectedColor());
    // check if all four are filled

    if(qobject_cast<GuessButtonsHandler*>(this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_, 1)->widget())->isCurrentGuessValid()) {
        // show "Submit" Button
        this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_, 2)->widget()->setVisible(true);
    }
}

void Game::submitGuess() {
    assert(this->numCurrentGuess_ <= 12);
    // gather current guess
    QVector<QColor> guess = qobject_cast<GuessButtonsHandler*>(this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_, 1)->widget())->getFullGuess();
    // produce hint
    auto hint = this->checkGuess(guess);
    // remove Submit button
    this->uiGridLayout_->removeWidget(qobject_cast<QWidget*>(sender()));
    delete sender();
    // print hint
    HintViewer* hints = new HintViewer(hint);
    this->uiGridLayout_->addWidget(hints, 12 - this->numCurrentGuess_, 2);
    // guess correct? if not, check whether more trys possible
    if(hint == QVector<int> {1, 1, 1, 1}) {
        this->endGame(true);
    } else if(this->numCurrentGuess_ == 12) {
        // Submitted the last Try and lost
        this->endGame(false);
    } else {
        qobject_cast<GuessButtonsHandler*>(this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_, 1)->widget())->forEveryButtonDo([](GuessButton* button) {button->setEnabled(false);} );
        qobject_cast<GuessButtonsHandler*>(this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_ - 1, 1)->widget())->forEveryButtonDo([](GuessButton* button) {button->setEnabled(true);} );
        this->numCurrentGuess_++;
    }
}

void Game::endGame(bool hasWon) {
    //qInfo() << "endGame: " << this->mainwindow_->width() << ", " << this->mainwindow_->height() << ", " << this->parentWidget()->objectName();
    this->solution_->reveal();
    for(int i = 1; i < 5; i++) {
        qobject_cast<GuessButtonsHandler*>(this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_, 1)->widget())->forEveryButtonDo([](GuessButton* button) {button->setEnabled(false);} );
    }
    EndScreen* end = new EndScreen(hasWon, this->mainwindow_);
    this->endScreen_ = end;
    this->hasEnded = true;
}

void Game::generateCode() {
    for(int i = 0; i < 4; i++) {
        this->code_.push_back(this->colors_[QRandomGenerator::global()->generate()%6]);
    }
}

QVector<int> Game::checkGuess(QVector<QColor> guess) {
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

void Game::resizeEvent(QResizeEvent *event) {
    if(this->endScreen_ != nullptr) {
        float sizeFactor = 0.6;
        int newWidth = this->width()*sizeFactor;
        int newHeight = this->height()*sizeFactor;
        int xpos = (this->mainwindow_->width()-newWidth)/2;
        int ypos = (this->mainwindow_->height()-newHeight)/2;
        this->endScreen_->move(xpos, ypos);
        this->endScreen_->resize(newWidth, newHeight);
        this->endScreen_->getBlocker()->resize(this->mainwindow_->width(), this->mainwindow_->height());
        qInfo() << "Game::resizeEvent. Game width = " << this->width() << " height = " << this->height() << "\nWindow width = " << newWidth << " height = " << newHeight << "\nx = " << xpos << " y = " << ypos;
        qInfo() << this->parentWidget()->width() << ", " << this->parentWidget()->height();
    }
    this->colorButtonsHandler_->updateGeometry();
    this->solution_->updateGeometry();
    QWidget::resizeEvent(event);
}

void Game::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
}

void Game::setEndscreen(EndScreen *newEndScreen = nullptr) {
    this->endScreen_ = newEndScreen;
}

QVector<QColor> Game::getCode() {
    return this->code_;
}

int Game::getCurrentPinHeight() {
    return qobject_cast<GuessButtonsHandler*>(this->uiGridLayout_->itemAtPosition(0, 1)->widget())->getButtons()[0]->height();
}

EndScreen::EndScreen(bool hasWon, QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    QHBoxLayout* buttonsLayout2 = new QHBoxLayout;
    QSpacerItem* spacer = new QSpacerItem(50, 30, QSizePolicy::Expanding, QSizePolicy::Ignored);
    QLabel* heading = new QLabel((hasWon) ? "You have Won!" : "You Lost!");
    QPushButton* newGame = new QPushButton("Neues Spiel");
    QPushButton* back = new QPushButton("Zurück zum Menü");
    QPushButton* close = new QPushButton("X");
    layout->addLayout(buttonsLayout2);
    layout->addWidget(heading);
    layout->addLayout(buttonsLayout);
    buttonsLayout2->addItem(spacer);
    buttonsLayout2->addWidget(close);
    buttonsLayout->addWidget(newGame);
    buttonsLayout->addWidget(back);
    this->setVisible(true);

    buttonsLayout2->setMargin(10);
    buttonsLayout2->setSpacing(15);
    close->setFixedSize(30, 30);
    close->setStyleSheet("QPushButton {"
                            "padding: 5px;"
                            "font: 8pt 'MS Shell Dlg 2';"
                            "border: 3px solid #e63939;"
                            "background-color: #e64e4e;"
                            "border-radius: 15px;}"
                          ":pressed {"
                            "background-color: #d64545;"
                          "}");
    QGraphicsDropShadowEffect* shadow4 = new QGraphicsDropShadowEffect;
    shadow4->setColor(QColor(230, 57, 57));
    shadow4->setBlurRadius(10);
    shadow4->setOffset(QPointF(0, 0));
    close->setGraphicsEffect(shadow4);

    buttonsLayout->setMargin(10);
    buttonsLayout->setSpacing(15);
    newGame->setFixedHeight(30);
    newGame->setStyleSheet("QPushButton {"
                            "padding: 5px;"
                            "font: 8pt 'MS Shell Dlg 2';"
                            "border: 3px solid #60d904;"
                            "background-color: #6df505;"
                            "border-radius: 15px;}"
                          ":pressed {"
                            "background-color: #65e305;"
                          "}");
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setColor(QColor(96, 217, 4));
    shadow->setBlurRadius(10);
    shadow->setOffset(QPointF(0, 0));
    newGame->setGraphicsEffect(shadow);
    back->setFixedHeight(30);
    back->setStyleSheet("QPushButton {"
                            "padding: 5px;"
                            "font: 8pt 'MS Shell Dlg 2';"
                            "border: 3px solid #e63939;"
                            "background-color: #e64e4e;"
                            "border-radius: 15px;}"
                          ":pressed {"
                            "background-color: #d64545;"
                          "}");
    QGraphicsDropShadowEffect* shadow2 = new QGraphicsDropShadowEffect;
    shadow2->setColor(QColor(230, 57, 57));
    shadow2->setBlurRadius(10);
    shadow2->setOffset(QPointF(0, 0));
    back->setGraphicsEffect(shadow2);

    QGraphicsDropShadowEffect* shadow3 = new QGraphicsDropShadowEffect;
    shadow3->setColor(QColor(255, 156, 8));
    shadow3->setBlurRadius(10);
    shadow3->setOffset(QPointF(0, 0));
    this->setGraphicsEffect(shadow3);

    QWidget* blocker  = new QWidget(parent);
    blocker->setObjectName("blocker");
    blocker->setStyleSheet("QWidget#blocker {background-color: rgba(128, 128, 128, 50);}");
    blocker->setVisible(true);
    blocker->setGeometry(0, 0, parent->width(), parent->height());
    this->blocker_ = blocker;
    blocker->raise();
    this->raise();
    int width = parent->findChild<Game*>("game")->width()*0.6;
    int height = parent->findChild<Game*>("game")->height()*0.6;
    this->setGeometry((parent->width()-width)/2, (parent->height()-height)/2, width, height);
    connect(newGame, SIGNAL(clicked()), this->parentWidget(), SLOT(newGame()));
    connect(back, SIGNAL(clicked()), this->parentWidget(), SLOT(returnToMenu()));
    connect(newGame, SIGNAL(clicked()), this, SLOT(remove()));
    connect(back, SIGNAL(clicked()), this, SLOT(remove()));
    connect(close, SIGNAL(clicked()), this, SLOT(remove()));
}

QSize EndScreen::sizeHint() const {
    return QWidget::sizeHint();
    int height = this->parentWidget()->height()*0.6;
    int width = this->parentWidget()->width()*0.6;
    qInfo() << "Height: " << height << ", Width:" << width;
    return QSize(width, height);
}

void EndScreen::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(QRectF(2, 2, this->width()-4, this->height()-4), 20, 20);
    painter.setPen(QPen(QColor(255, 156, 8), 4));
    painter.fillPath(path, QColor(211, 211, 211));
    painter.drawPath(path);
    QWidget::paintEvent(event);
}

void EndScreen::moveEvent(QMoveEvent *event) {
    QWidget::moveEvent(event);
}

QWidget* EndScreen::getBlocker() {
    return this->blocker_;
}

void EndScreen::remove() {
    if(this->parentWidget()->findChild<Game*>("game")) {
        this->parentWidget()->findChild<Game*>("game")->setEndscreen();
    }
    delete this->blocker_;
    delete this;
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
    //qInfo() << this->hint_[0] << ", " << this->hint_[1] << ", " << this->hint_[2] << ", " << this->hint_[3];
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
