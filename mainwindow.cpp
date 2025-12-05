#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Setup Text Scene
    sceneText = new QGraphicsScene(this);
    ui->graphicsView->setScene(sceneText);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    // 2. Setup Image Scene
    sceneImage = new QGraphicsScene(this);
    if(ui->graphicsViewImage) {
        ui->graphicsViewImage->setScene(sceneImage);
        ui->graphicsViewImage->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    // 3. Setup Sliders
    ui->zoomSlider->setRange(10, 200);
    ui->zoomSlider->setValue(100);

    if(ui->zoomSliderImage) {
        ui->zoomSliderImage->setRange(10, 200);
        ui->zoomSliderImage->setValue(100);
    }

    // 4. Force Start Home
    if(ui->stackedWidget->count() > 0) {
        ui->stackedWidget->setCurrentWidget(ui->pageHome);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ===============================================
// NAVIGATION
// ===============================================
void MainWindow::on_btnTextMode_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageText); }
void MainWindow::on_btnImageMode_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageImage); }
void MainWindow::on_btnBack_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageHome); }
void MainWindow::on_btnBack2_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageHome); }

// ===============================================
// VISUALIZATION HELPERS
// ===============================================

int MainWindow::getLeafCount(HuffNode* node) {
    if (!node) return 0;
    if (!node->left && !node->right) return 1;
    return getLeafCount(node->left) + getLeafCount(node->right);
}

void MainWindow::drawTreeRecursive(QGraphicsScene* sc, HuffNode* node, double leftBound, double rightBound, double y) {
    if (!node) return;

    double x = (leftBound + rightBound) / 2.0;
    double radius = 40;
    double vGap = 80;

    if (node->left) {
        int leftLeaves = getLeafCount(node->left);
        int totalLeaves = getLeafCount(node);
        // Protect against division by zero
        if (totalLeaves == 0) totalLeaves = 1;

        double splitPoint = leftBound + (rightBound - leftBound) * ((double)leftLeaves / totalLeaves);
        double childX = (leftBound + splitPoint) / 2.0;

        sc->addLine(x, y + radius/2, childX, y + vGap, QPen(Qt::white, 2));
        drawTreeRecursive(sc, node->left, leftBound, splitPoint, y + vGap);
    }

    if (node->right) {
        int leftLeaves = getLeafCount(node->left);
        int totalLeaves = getLeafCount(node);
        if (totalLeaves == 0) totalLeaves = 1;

        double splitPoint = leftBound + (rightBound - leftBound) * ((double)leftLeaves / totalLeaves);
        double childX = (splitPoint + rightBound) / 2.0;

        sc->addLine(x, y + radius/2, childX, y + vGap, QPen(Qt::white, 2));
        drawTreeRecursive(sc, node->right, splitPoint, rightBound, y + vGap);
    }

    QBrush brush = (!node->left && !node->right) ? QBrush(QColor(100, 255, 100)) : QBrush(Qt::lightGray);
    QGraphicsEllipseItem* circle = sc->addEllipse(x - radius/2, y - radius/2, radius, radius, QPen(Qt::black), brush);
    circle->setZValue(2);

    QString txt;
    if (!node->left && !node->right) {
        if(node->symbol == 32) txt = "Sp";
        else if(node->symbol == 10) txt = "\\n";
        else if(node->symbol < 256 && isprint(node->symbol)) txt = QString(QChar(node->symbol));
        else txt = QString::number(node->symbol);
    } else {
        txt = QString::number(node->f);
    }

    QGraphicsTextItem* textItem = sc->addText(txt);
    textItem->setPos(x - textItem->boundingRect().width()/2, y - textItem->boundingRect().height()/2);
    textItem->setZValue(3);
}

void MainWindow::drawForest(QGraphicsScene* targetScene) {
    targetScene->clear();

    double currentX = 0;
    double unitWidth = 50;
    double gapBetweenTrees = 100;
    double startY = 60;

    for (HuffNode* tree : forest) {
        int leaves = getLeafCount(tree);
        double treeWidth = leaves * unitWidth;
        drawTreeRecursive(targetScene, tree, currentX, currentX + treeWidth, startY);
        currentX += treeWidth + gapBetweenTrees;
    }
    targetScene->setSceneRect(0, 0, currentX, 2000);
}

void MainWindow::displayTableInUI(QTableWidget* table, int* freqs, std::string *codes, int range) {
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Symbol", "Freq", "Code"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    for (int i = 0; i < range; i++) {
        if (freqs[i] > 0) {
            table->insertRow(row);
            QString charDisplay = (range == 256 && i < 256) ? (i==32?"Space":(i==10?"\\n":QString(QChar(i)))) : QString::number(i);

            table->setItem(row, 0, new QTableWidgetItem(charDisplay));
            table->setItem(row, 1, new QTableWidgetItem(QString::number(freqs[i])));
            table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(codes[i])));
            row++;
        }
    }
}

// ===============================================
// TEXT COMPRESSION
// ===============================================
void MainWindow::on_btnReset_clicked() {
    QString inputText = ui->textInput->toPlainText();
    if(inputText.isEmpty()) {
        QMessageBox::warning(this, "Wait", "Please enter text first!");
        return;
    }

    // Clear old memory
    // Note: In a real app, we should delete the pointers in forest, but for this viz project,
    // we assume forest nodes are managed or we leak slightly for simplicity.
    forest.clear();
    sceneText->clear();

    std::string s = inputText.toStdString();
    int charFreqs[256] = {0};
    for (char c : s) charFreqs[int(c)]++; // Logic: int cast ok here for freq counting

    for (int i = 0; i < 256; i++) {
        if (charFreqs[i] > 0) forest.push_back(new HuffNode(i, charFreqs[i]));
    }

    // Initial Sort
    std::sort(forest.begin(), forest.end(), compareNodes);
    drawForest(sceneText);

    // Clear Outputs
    if(ui->lblTextStats) ui->lblTextStats->clear();
    ui->textOutput->clear();
    ui->tableWidget->setRowCount(0);
}

void MainWindow::on_btnStep_clicked() {
    if (forest.size() <= 1) {
        QMessageBox::information(this, "Info", "Tree Complete!");
        return;
    }

    // Sort
    std::sort(forest.begin(), forest.end(), compareNodes);

    HuffNode* left = forest[0];
    HuffNode* right = forest[1];

    forest.erase(forest.begin());
    forest.erase(forest.begin()); // The vector shifts, so we erase begin() again

    forest.push_back(new HuffNode(left->f + right->f, left, right));
    drawForest(sceneText);
}

void MainWindow::on_btnCompressText_clicked() {
    // 1. Reset logic to start clean
    on_btnReset_clicked();

    if(forest.empty()) return;

    // 2. Build Tree in Memory (NO DRAWING HERE to prevent crash)
    while(forest.size() > 1) {
        std::sort(forest.begin(), forest.end(), compareNodes);
        HuffNode* left = forest[0];
        HuffNode* right = forest[1];
        forest.erase(forest.begin());
        forest.erase(forest.begin());
        forest.push_back(new HuffNode(left->f + right->f, left, right));
    }

    // 3. Draw the final result ONCE
    drawForest(sceneText);

    // 4. Encode & Calculate Stats
    std::string s = ui->textInput->toPlainText().toStdString();

    // Convert to ASCII if needed (or just use s)
    //string asciiS = utf8ToAsciiIconv(s);

    HuffNode* root = forest[0];
    string *codes = getHuffmanCodes(root);
    string encoded = encode(s, root);

    ui->textOutput->setText(QString::fromStdString(encoded));

    // --- UPDATED STATISTICS SECTION ---
    long long origBits = s.length() * 8;      // 8 bits per character
    long long compBits = encoded.length();    // 1 bit per character of '0' or '1' string

    double ratio = 0.0;
    if(origBits > 0) {
        ratio = (1.0 - ((double)compBits / (double)origBits)) * 100.0;
    }

    if(ui->lblTextStats) {
        ui->lblTextStats->setText(
            "Original Size: " + QString::number(origBits) + " bits\n" +
            "New Size: " + QString::number(compBits) + " bits\n" +
            "Compression Ratio: " + QString::number(ratio, 'f', 2) + "%"
            );
    }
    // ----------------------------------

    // Recalculate freqs for table display
    int freqs[256] = {0};
    for(char c : s) freqs[(unsigned char)c]++;

    displayTableInUI(ui->tableWidget, freqs, codes, 256);
}

void MainWindow::on_zoomSlider_valueChanged(int value) {
    qreal scale = value / 100.0;
    ui->graphicsView->resetTransform();
    ui->graphicsView->scale(scale, scale);
}

// ===============================================
// IMAGE COMPRESSION
// ===============================================

// 1. SELECT IMAGE
void MainWindow::on_btnSelectImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if(fileName.isEmpty()) return;

    currentImagePath = fileName;

    QPixmap p(fileName);
    ui->labelOriginal->setPixmap(p.scaled(ui->labelOriginal->size(), Qt::KeepAspectRatio));

    // Clear old data when selecting new image
    if(ui->labelCompressed) ui->labelCompressed->clear();
    if(ui->tableImage) ui->tableImage->setRowCount(0);
    ui->labelStats->setText("Image Loaded. Ready to compress.");

    initializeImageForest();
}

void MainWindow::initializeImageForest() {
    if(currentImagePath.isEmpty()) return;

    forest.clear();
    sceneImage->clear();

    int w, h, c;
    unsigned char* img_data = stbi_load(currentImagePath.toStdString().c_str(), &w, &h, &c, 0);
    if(!img_data) return;

    int freqs[511] = {0};
    long long size = w * h * c;
    for(long long i=0; i<size; i++) {
        int pixel = (int)img_data[i];
        int val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
        freqs[val]++;
    }
    stbi_image_free(img_data);

    for (int i = 0; i < 511; i++) {
        if (freqs[i] > 0) forest.push_back(new HuffNode(i, freqs[i]));
    }
    std::sort(forest.begin(), forest.end(), compareNodes);
    drawForest(sceneImage);
}

// 2. RESET BUTTON
void MainWindow::on_btnResetImage_clicked()
{
    if(currentImagePath.isEmpty()) return;

    // Restart the visualization from scratch
    initializeImageForest();

    // Clear the "Done" state info
    if(ui->labelCompressed) ui->labelCompressed->clear();
    if(ui->tableImage) ui->tableImage->setRowCount(0);
    ui->labelStats->setText("Reset Complete. Ready to step or compress.");
}

// 3. STEP BUTTON
void MainWindow::on_btnStepImage_clicked() {
    if (forest.size() <= 1) {
        QMessageBox::information(this, "Info", "Tree Complete!");
        return;
    }
    std::sort(forest.begin(), forest.end(), compareNodes);
    HuffNode* left = forest[0];
    HuffNode* right = forest[1];
    forest.erase(forest.begin());
    forest.erase(forest.begin());
    forest.push_back(new HuffNode(left->f + right->f, left, right));

    drawForest(sceneImage);
}

// 4. COMPRESS BUTTON
void MainWindow::on_btnCompressImage_clicked() {
    if(currentImagePath.isEmpty()) return;

    // Fast-forward animation
    while(forest.size() > 1) {
        std::sort(forest.begin(), forest.end(), compareNodes);
        HuffNode* left = forest[0];
        HuffNode* right = forest[1];
        forest.erase(forest.begin());
        forest.erase(forest.begin());
        forest.push_back(new HuffNode(left->f + right->f, left, right));
    }
    drawForest(sceneImage);

    // Actual Logic
    HuffNode* tree = nullptr;
    long long rawDataSize = 0;
    string encoded = encodeImage(currentImagePath.toStdString(), tree, rawDataSize);

    if(encoded.empty()) return;

    // Decode & Show
    unsigned char* decodedBytes = decodeImage(encoded, tree, rawDataSize);
    int w, h, c;
    unsigned char* temp = stbi_load(currentImagePath.toStdString().c_str(), &w, &h, &c, 0);
    stbi_image_free(temp);
    stbi_write_png("verified_output.png", w, h, c, decodedBytes, w*c);
    delete[] decodedBytes;

    QPixmap pComp("verified_output.png");
    if(ui->labelCompressed) ui->labelCompressed->setPixmap(pComp.scaled(ui->labelCompressed->size(), Qt::KeepAspectRatio));

    long long rawSizeBytes = w * h * c;
    long long compressedSizeBytes = encoded.length() / 8;

    double ratio = 0.0;
    if(rawSizeBytes > 0) {
        ratio = (1.0 - ((double)compressedSizeBytes / (double)rawSizeBytes)) * 100.0;
    }

    ui->labelStats->setText(
        "Raw Data: " + QString::number(rawSizeBytes/1024) + " KB\n" +
        "Compressed: " + QString::number(compressedSizeBytes/1024) + " KB\n" +
        "Reduction: " + QString::number(ratio, 'f', 2) + "%"
        );

    if(ui->tableImage) {
        string *codes = getHuffmanCodesForImage(tree);
        // We need freqs again for display
        int freqs[511] = {0};
        unsigned char* img_data = stbi_load(currentImagePath.toStdString().c_str(), &w, &h, &c, 0);
        for(long long i=0; i<rawDataSize; i++) {
            int pixel = (int)img_data[i];
            int val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
            freqs[val]++;
        }
        stbi_image_free(img_data);
        displayTableInUI(ui->tableImage, freqs, codes, 511);
    }
}

// 5. ZOOM
void MainWindow::on_zoomSliderImage_valueChanged(int value) {
    if(ui->graphicsViewImage) {
        qreal scale = value / 100.0;
        ui->graphicsViewImage->resetTransform();
        ui->graphicsViewImage->scale(scale, scale);
    }
}
