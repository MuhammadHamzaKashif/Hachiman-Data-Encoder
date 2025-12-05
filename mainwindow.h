#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <vector>
#include <algorithm>
#include <QTableWidget>
#include "HuffmanLogic.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Navigation
    void on_btnTextMode_clicked();
    void on_btnImageMode_clicked();
    void on_btnBack_clicked();
    void on_btnBack2_clicked();

    // TEXT Page Slots
    void on_btnCompressText_clicked();
    void on_btnReset_clicked();
    void on_btnStep_clicked();
    void on_zoomSlider_valueChanged(int value);

    // IMAGE Page Slots
    void on_btnSelectImage_clicked();
    void on_btnCompressImage_clicked();
    void on_btnStepImage_clicked();
    void on_btnResetImage_clicked();     // <--- NEW: Reset Button Slot
    void on_zoomSliderImage_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    // Scenes
    QGraphicsScene *sceneText;
    QGraphicsScene *sceneImage;

    // The visualizer "Forest"
    std::vector<HuffNode*> forest;

    // Store current image path
    QString currentImagePath;

    // Helpers
    void drawForest(QGraphicsScene* targetScene);
    void drawTreeRecursive(QGraphicsScene* sc, HuffNode* node, double xLeft, double xRight, double y);
    int getLeafCount(HuffNode* node);
    void displayTableInUI(QTableWidget* table, int* freqs, std::string *codes, int range);
    void initializeImageForest();

    string utf8ToAsciiIconv(const string& input);
};

#endif // MAINWINDOW_H
