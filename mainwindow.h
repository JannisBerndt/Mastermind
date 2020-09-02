#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addWidgetToPage(QString pageName, QWidget* widget);

private slots:
    void playGame();

private:
    Ui::MainWindow *ui;
};

#include <QPushButton>
#include <QColor>

class GuessButton : public QPushButton {
    Q_OBJECT

public:
    GuessButton(QWidget *parent = nullptr);
    QColor getColor();
    void setColor(QColor newColor);
    int heightForWidth(int w) const override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QColor color_;
};

#include <QRadioButton>

class ColorSelectButton : public QRadioButton {
    Q_OBJECT

public:
    ColorSelectButton(QColor color, QWidget* parent = nullptr);
    QColor getColor();
    void paintEvent(QPaintEvent *event) override;

private:
    QColor color_;
};

class HintViewer : public QWidget {
    Q_OBJECT

public:
    HintViewer(QVector<int> hint, QWidget* parent = nullptr);
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<int> hint_;
};

class StyleSheetFactory {
public:
    StyleSheetFactory();

private:
    QString staticSheet_;
    QMap<QString, QString> arguments_();
};

#endif // MAINWINDOW_H
