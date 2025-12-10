#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <vector>
#include <algorithm>
#include <QTableWidget>
#include <map>
#include <QLabel>        // For watermark
#include <QResizeEvent>  // For resizing
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

protected:
    // Override resizeEvent to keep watermark in corner & on top
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Navigation
    void on_btnTextMode_clicked();
    void on_btnImageMode_clicked();
    void on_btnDecompressTextMode_clicked();
    void on_btnDecompressImageMode_clicked();

    // Back Buttons
    void on_btnBack_clicked();
    void on_btnBack2_clicked();
    void on_btnBack3_clicked();
    void on_btnBack4_clicked();

    // TEXT COMPRESSION
    void on_btnCompressText_clicked();
    void on_btnReset_clicked();
    void on_btnStep_clicked();
    void on_zoomSlider_valueChanged(int value);
    void on_btnSaveTextHuff_clicked();

    // IMAGE COMPRESSION
    void on_btnSelectImage_clicked();
    void on_btnCompressImage_clicked();
    void on_btnStepImage_clicked();
    void on_btnResetImage_clicked();
    void on_zoomSliderImage_valueChanged(int value);
    void on_btnSaveHuff_clicked();

    // DECOMPRESSION PAGES
    void on_btnDecompressTextAction_clicked();
    void on_zoomSliderTextDecomp_valueChanged(int value);
    void on_btnOpenHuff_clicked();
    void on_zoomSliderImageDecomp_valueChanged(int value);


private:
    Ui::MainWindow *ui;

    // Scenes
    QGraphicsScene *sceneText;
    QGraphicsScene *sceneImage;
    QGraphicsScene *sceneTextDecomp;
    QGraphicsScene *sceneImageDecomp;

    // Data
    std::vector<HuffNode*> forest;
    QString currentImagePath;

    // Watermark Label
    QLabel *watermarkLabel;

    // Visual Helpers
    void drawForest(QGraphicsScene* targetScene);
    void drawTreeRecursive(QGraphicsScene* sc, HuffNode* node, double xLeft, double xRight, double y);
    int getLeafCount(HuffNode* node);

    // Logic Helpers
    void displayTableInUI(QTableWidget* table, std::map<int, int> freqs, std::string *codes, int range);
    void initializeImageForest();
    void extractFreqsFromTree(HuffNode* root, std::map<int, int>& map);

    // NEW: Custom Success Popup Helper
    void showCustomSuccessPopup(QString message);
};

#endif // MAINWINDOW_H
