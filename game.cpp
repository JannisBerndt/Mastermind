#include "game.h"
#include "mainwindow.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QDebug>

Game::Game(QWidget *parent) {
    QWidget* content = new QWidget;
    QVBoxLayout* contentLayout = new QVBoxLayout;
    QScrollArea* scrollArea = new QScrollArea;
    QGridLayout* gridLayout = new QGridLayout;
    scrollArea->setLayout(gridLayout);
    contentLayout->addWidget(scrollArea);
    content->setLayout(contentLayout);

    for(int i = 0; i < 12; i++) {
        QLabel* label = new QLabel();
        label->setText(QString::number(12-i));
        label->setFixedWidth(15);
        gridLayout->addWidget(label, i, 0);

        for(int j = 1; j < 5; j++) {
            GuessButton* button = new GuessButton();
            button->sizePolicy().setWidthForHeight(true);
            if(i != 11) button->setEnabled(false);
            connect(button, SIGNAL(clicked()),this, SLOT(putColorGuess()));
            gridLayout->addWidget(button, i, j, Qt::AlignCenter);
        }
        QWidget* placeholder = new QWidget();
        placeholder->setFixedWidth(50);
        gridLayout->addWidget(placeholder, i, 5);
    }

    this->uiContent_ = content;
    this->uiGridLayout_ = gridLayout;
    QHBoxLayout* colorSelectButtonsLayout = new QHBoxLayout();
    this->colors_ = {QColor(222, 222, 222), QColor(168, 17, 17), QColor(245, 241, 10), QColor(11, 128, 37), QColor(11, 32, 219), QColor(219, 11, 181)};
    for(int i = 0; i < 6; i++) {
        ColorSelectButton* button = new ColorSelectButton(colors_[i], parent);
        if(i == 0) {
            button->setChecked(true);
            this->selectedColor_ = colors_[i];
        }
        connect(button, SIGNAL(clicked()),this, SLOT(selectColor()));
        colorSelectButtonsLayout->addWidget(button);
    }
    contentLayout->addLayout(colorSelectButtonsLayout);
    this->numCurrentGuess_ = 1;
    this->generateCode();
}

QWidget* Game::getContent() {
    return this->uiContent_;
}

void Game::putColorGuess() {
    GuessButton* button = qobject_cast<GuessButton*>(sender());
    button->setColor(this->selectedColor_);
    // check if all four are filled
    QVector<QColor> colors;
    for(int i = 1; i < 5; i++) {
        colors.push_back(qobject_cast<GuessButton*>(this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_, i)->widget())->getColor());
    }
    bool valid = std::accumulate(colors.begin(), colors.end(), true, [](bool rest, QColor color) { return rest && color != QColor(0,0,0); } );
    if(valid) {
        // create "Submit" Button
        QPushButton* button = new QPushButton("Submit");
        button->setFixedWidth(50);
        connect(button, SIGNAL(clicked()),this, SLOT(submitGuess()));
        this->uiGridLayout_->addWidget(button, 12 - this->numCurrentGuess_, 5);
    }
}

void Game::selectColor() {
    ColorSelectButton* button = qobject_cast<ColorSelectButton*>(sender());
    this->selectedColor_ = button->getColor();
}

void Game::submitGuess() {
    if(this->numCurrentGuess_ == 12) {
        // Submitted the last Try

    } else {
        QVector<QColor> guess;
        for(int i = 1; i < 5; i++) {
            guess.push_back(qobject_cast<GuessButton*>(this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_, i)->widget())->getColor());
        }
        for(int i = 1; i < 5; i++) {
            this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_, i)->widget()->setEnabled(false);
            this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_ - 1, i)->widget()->setEnabled(true);
        }
        auto hint = this->checkGuess(guess);
        this->uiGridLayout_->removeWidget(qobject_cast<QWidget*>(sender()));
        delete sender();
        // print hint
        HintViewer* hints = new HintViewer(hint);
        this->uiGridLayout_->addWidget(hints, 12 - this->numCurrentGuess_, 5);
        this->numCurrentGuess_++;
        if(hint == QVector<int> {1, 1, 1, 1}) {
            this->wonGame();
        }
    }
}

void Game::wonGame() {
    for(int i = 1; i < 5; i++) {
        this->uiGridLayout_->itemAtPosition(12 - this->numCurrentGuess_, i)->widget()->setEnabled(false);
    }
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
