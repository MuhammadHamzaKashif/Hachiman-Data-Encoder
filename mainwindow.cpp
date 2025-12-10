#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDebug>
#include <QDataStream>
#include <QTextStream>
#include <QResizeEvent>
#include <QDialog>       // Required for Custom Popup
#include <QVBoxLayout>   // Required for Custom Popup Layout
#include <QPushButton>   // Required for Custom Popup Button
#include <QLabel>        // Required for Custom Popup Image

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set Main Window Icon
    this->setWindowIcon(QIcon(":/icon_ore2.png"));

    // =======================================================
    // 1. SETUP WATERMARK OVERLAY (Bottom Right)
    // =======================================================
    watermarkLabel = new QLabel(this);

    QPixmap watermark(":/hachi.png"); // Using your Hachiman image

    if (!watermark.isNull()) {
        watermark = watermark.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    watermarkLabel->setPixmap(watermark);
    watermarkLabel->adjustSize();
    watermarkLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    watermarkLabel->show();

    // --- FORCE POSITION TO BOTTOM RIGHT START ---
    int startPadding = 0;
    // Calculate initial position if window has width, otherwise safe default
    int startX = (this->width() > 0) ? this->width() - watermarkLabel->width() - startPadding : 100;
    int startY = (this->height() > 0) ? this->height() - watermarkLabel->height() - startPadding : 100;
    watermarkLabel->move(startX, startY);


    // =======================================================
    // 2. GLOBAL WINDOW STYLE
    // =======================================================
    this->setStyleSheet(
        "QMainWindow { background-color: #121212; }"
        "QWidget { font-family: 'Segoe UI', sans-serif; color: #e0e0e0; }"
        );

    // --- PLACEHOLDER TEXT SETUP ---
    if(ui->textInput)
        ui->textInput->setPlaceholderText("Input Text");

    if(ui->textInputFreqs)
        ui->textInputFreqs->setPlaceholderText("Input Tree");

    if(ui->textInputBits)
        ui->textInputBits->setPlaceholderText("Input Encoded Bits");

    // =======================================================
    // 3. WIDGET STYLES
    // =======================================================

    ui->labelTitle->setStyleSheet(
        "QLabel {"
        "   color: #ffffff;"
        "   font-size: 36px;"
        "   font-weight: bold;"
        "   padding: 20px;"
        "   qproperty-alignment: AlignCenter;"
        "}"
        );

    QString cardStyle =
        "QFrame#frameText, QFrame#frameImage {"
        "   background-color: #1e1e1e;"
        "   border: 1px solid #333333;"
        "   border-radius: 16px;"
        "   margin: 10px;"
        "}";

    QString headerStyle =
        "QFrame QLabel {"
        "   color: #aaaaaa;"
        "   font-size: 18px;"
        "   font-weight: 600;"
        "   padding-bottom: 10px;"
        "   background: transparent;"
        "}";

    QString btnStyle =
        "QPushButton {"
        "   background-color: #2d2d2d;"
        "   color: #ffffff;"
        "   border: 1px solid #3e3e3e;"
        "   border-radius: 8px;"
        "   padding: 15px;"
        "   font-size: 14px;"
        "   font-weight: 500;"
        "   margin: 6px 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #383838;"
        "   border: 1px solid #555555;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #0078d4;"
        "   border: 1px solid #0078d4;"
        "}";

    ui->frameText->setStyleSheet(cardStyle + headerStyle + btnStyle);
    ui->frameImage->setStyleSheet(cardStyle + headerStyle + btnStyle);

    // =======================================================
    // 4. SCENE & SLIDER SETUP
    // =======================================================

    sceneText = new QGraphicsScene(this);
    ui->graphicsView->setScene(sceneText);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    sceneImage = new QGraphicsScene(this);
    if(ui->graphicsViewImage) {
        ui->graphicsViewImage->setScene(sceneImage);
        ui->graphicsViewImage->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    sceneTextDecomp = new QGraphicsScene(this);
    if(ui->graphicsViewTextDecomp) {
        ui->graphicsViewTextDecomp->setScene(sceneTextDecomp);
        ui->graphicsViewTextDecomp->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    sceneImageDecomp = new QGraphicsScene(this);
    if(ui->graphicsViewImageDecomp) {
        ui->graphicsViewImageDecomp->setScene(sceneImageDecomp);
        ui->graphicsViewImageDecomp->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    ui->zoomSlider->setRange(10, 200); ui->zoomSlider->setValue(100);
    if(ui->zoomSliderImage) { ui->zoomSliderImage->setRange(10, 200); ui->zoomSliderImage->setValue(100); }
    if(ui->zoomSliderTextDecomp) { ui->zoomSliderTextDecomp->setRange(10, 200); ui->zoomSliderTextDecomp->setValue(100); }
    if(ui->zoomSliderImageDecomp) { ui->zoomSliderImageDecomp->setRange(10, 200); ui->zoomSliderImageDecomp->setValue(100); }

    if(ui->stackedWidget->count() > 0) ui->stackedWidget->setCurrentWidget(ui->pageHome);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ===============================================
// CUSTOM SUCCESS POPUP (UPDATED)
// ===============================================
void MainWindow::showCustomSuccessPopup(QString message)
{
    // Create the dialog
    QDialog dialog(this);
    // Use FramelessWindowHint for a clean look without the OS title bar
    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    // Note: Qt::WA_TranslucentBackground is removed to show the background color

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(30, 30, 30, 30); // Padding inside the dialog
    layout->setSpacing(20); // Spacing between image, text, and button

    // --- Add Image ---
    QLabel *imgLabel = new QLabel(&dialog);
    // *** REPLACE WITH YOUR IMAGE PATH ***
    QPixmap icon(":/saika.png");

    if(!icon.isNull()) {
        // Increase size: Scalling to 200x200 pixels (approx 3-4x larger)
        imgLabel->setPixmap(icon.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imgLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(imgLabel);
    }

    // --- Add Message Text ---
    QLabel *textLabel = new QLabel(message, &dialog);
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setWordWrap(true); // Allow text to wrap if it's long
    layout->addWidget(textLabel);

    // --- Add OK Button ---
    QPushButton *btnOk = new QPushButton("OK", &dialog);
    // Connect the button click to close the dialog
    connect(btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
    // Add button to layout and center it
    layout->addWidget(btnOk, 0, Qt::AlignCenter);

    // --- Apply Stylesheet ---
    // This styles the dialog background, border, text, and button to match your theme.
    dialog.setStyleSheet(
        "QDialog {"
        "    background-color: #2b2b2b;" /* Dark grey background */
        "    border: 2px solid #555555;" /* Subtle grey border */
        "    border-radius: 12px;"       /* Rounded corners for the dialog */
        "}"
        "QLabel {"
        "    color: #e0e0e0;"            /* Light grey text */
        "    font-size: 18px;"           /* Larger font size */
        "    font-family: 'Segoe UI', sans-serif;"
        "}"
        "QPushButton {"
        "    background-color: #0078d4;" /* Blue button */
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"        /* Rounded corners for the button */
        "    padding: 10px 30px;"        /* Larger button padding */
        "    font-size: 14px;"
        "    font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0063b1;" /* Darker blue on hover */
        "}"
        "QPushButton:pressed {"
        "    background-color: #004a83;" /* Even darker blue when pressed */
        "}"
        );

    // Show the dialog and wait for user to dismiss it
    dialog.exec();
}

// ===============================================
// RESIZE EVENT (Force Watermark to Front)
// ===============================================
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if(watermarkLabel && watermarkLabel->isVisible()) {
        int padding = 0; // Space from the edge

        int x = this->width() - watermarkLabel->width() - padding;
        int y = this->height() - watermarkLabel->height() - padding;

        watermarkLabel->move(x, y);
        watermarkLabel->raise();
    }
}

// ===============================================
// NAVIGATION
// ===============================================
void MainWindow::on_btnTextMode_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageText); }
void MainWindow::on_btnImageMode_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageImage); }
void MainWindow::on_btnDecompressTextMode_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageTextDecomp); }
void MainWindow::on_btnDecompressImageMode_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageImageDecomp); }

void MainWindow::on_btnBack_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageHome); }
void MainWindow::on_btnBack2_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageHome); }
void MainWindow::on_btnBack3_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageHome); }
void MainWindow::on_btnBack4_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageHome); }

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
    textItem->setDefaultTextColor(Qt::black);
    textItem->setPos(x - textItem->boundingRect().width()/2, y - textItem->boundingRect().height()/2);
    textItem->setZValue(3);
}

void MainWindow::drawForest(QGraphicsScene* targetScene) {
    targetScene->clear();
    double currentX = 0;
    double unitWidth = 50;
    double startY = 60;

    for (HuffNode* tree : forest) {
        int leaves = getLeafCount(tree);
        double treeWidth = leaves * unitWidth;
        drawTreeRecursive(targetScene, tree, currentX, currentX + treeWidth, startY);
        currentX += treeWidth + 100;
    }
    targetScene->setSceneRect(0, 0, currentX, 2000);
}

void MainWindow::displayTableInUI(QTableWidget* table, std::map<int, int> freqs, std::string *codes, int range) {
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Symbol", "Freq", "Code"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    for (int i = 0; i < range; i++) {
        bool hasCode = !codes[i].empty();
        int f = freqs.count(i) ? freqs[i] : 0;

        if (f > 0 || hasCode) {
            table->insertRow(row);
            QString charDisplay = (range == 256 && i < 256) ? (i==32?"Space":(i==10?"\\n":QString(QChar(i)))) : QString::number(i);
            table->setItem(row, 0, new QTableWidgetItem(charDisplay));
            table->setItem(row, 1, new QTableWidgetItem(QString::number(f)));
            table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(codes[i])));
            row++;
        }
    }
}

void MainWindow::extractFreqsFromTree(HuffNode* root, std::map<int, int>& map) {
    if(!root) return;
    if(!root->left && !root->right) {
        map[root->symbol] = root->f;
    }
    extractFreqsFromTree(root->left, map);
    extractFreqsFromTree(root->right, map);
}

// ===============================================
// TEXT COMPRESSION
// ===============================================

void MainWindow::on_btnReset_clicked() {
    QString inputText = ui->textInput->toPlainText();
    if(inputText.isEmpty()) return;
    forest.clear(); sceneText->clear();

    std::string s = inputText.toStdString();
    int charFreqs[256] = {0};
    for (char c : s) charFreqs[(unsigned char)c]++;

    for (int i = 0; i < 256; i++) {
        if (charFreqs[i] > 0) forest.push_back(new HuffNode(i, charFreqs[i]));
    }
    std::sort(forest.begin(), forest.end(), compareNodes);
    drawForest(sceneText);

    ui->lblTextStats->clear(); ui->textOutput->clear(); ui->tableWidget->setRowCount(0);
}

void MainWindow::on_btnStep_clicked() {
    if (forest.size() <= 1) return;
    std::sort(forest.begin(), forest.end(), compareNodes);
    HuffNode* left = forest[0]; HuffNode* right = forest[1];
    forest.erase(forest.begin()); forest.erase(forest.begin());
    forest.push_back(new HuffNode(left->f + right->f, left, right));
    drawForest(sceneText);
}

void MainWindow::on_btnCompressText_clicked() {
    on_btnReset_clicked();
    if(forest.empty()) return;

    while(forest.size() > 1) {
        std::sort(forest.begin(), forest.end(), compareNodes);
        HuffNode* left = forest[0]; HuffNode* right = forest[1];
        forest.erase(forest.begin()); forest.erase(forest.begin());
        forest.push_back(new HuffNode(left->f + right->f, left, right));
    }
    drawForest(sceneText);

    std::string s = ui->textInput->toPlainText().toStdString();
    HuffNode* root = forest[0];
    string *codes = getHuffmanCodes(root);
    string encoded = encode(s, root);

    ui->textOutput->setText(QString::fromStdString(encoded));

    long long origBits = s.length() * 8;
    long long compBits = encoded.length();
    double ratio = 0.0;
    if(origBits > 0) ratio = (1.0 - ((double)compBits / (double)origBits)) * 100.0;

    ui->lblTextStats->setText("Original: " + QString::number(origBits) + " bits\nNew: " + QString::number(compBits) + " bits\nRatio: " + QString::number(ratio, 'f', 2) + "%");

    std::map<int, int> fMap;
    for(char c : s) fMap[(unsigned char)c]++;
    displayTableInUI(ui->tableWidget, fMap, codes, 256);
}

void MainWindow::on_btnSaveTextHuff_clicked() {
    if(forest.empty()) return;
    QString encodedText = ui->textOutput->toPlainText();
    string treeString = serializeTree(forest[0]);

    QString savePath = QFileDialog::getSaveFileName(this, "Save", "", "Text Files (*.txt)");
    if(savePath.isEmpty()) return;

    QFile file(savePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "--- TREE KEY ---\n" << QString::fromStdString(treeString) << "\n";
        out << "--- BINARY CODE ---\n" << encodedText << "\n";
        file.close();

        // --- CALL CUSTOM POPUP ---
        showCustomSuccessPopup("File successfully saved!");
    }
}

// ===============================================
// TEXT DECOMPRESSION
// ===============================================

void MainWindow::on_btnDecompressTextAction_clicked() {
    QString treeKey = ui->textInputFreqs->toPlainText();
    QString bits = ui->textInputBits->toPlainText();
    if(treeKey.isEmpty() || bits.isEmpty()) return;

    HuffNode* root = deserializeTree(treeKey.toStdString());
    if(!root) { QMessageBox::warning(this, "Error", "Invalid Tree Key."); return; }

    // Visualize
    forest.clear();
    forest.push_back(root);
    drawForest(sceneTextDecomp);

    // Decode
    string decoded = decode(bits.toStdString(), root);
    ui->textOutputDecomp->setText(QString::fromStdString(decoded));

    // Fill Table with Restored Frequencies
    if(ui->tableTextDecomp) {
        string *codes = getHuffmanCodes(root);
        std::map<int, int> restoredFreqs;
        extractFreqsFromTree(root, restoredFreqs); // Helper fills the map
        displayTableInUI(ui->tableTextDecomp, restoredFreqs, codes, 256);
    }
}

// ===============================================
// IMAGE COMPRESSION
// ===============================================

void MainWindow::on_btnSelectImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if(fileName.isEmpty()) return;
    currentImagePath = fileName;
    QPixmap p(fileName);
    ui->labelOriginal->setPixmap(p.scaled(ui->labelOriginal->size(), Qt::KeepAspectRatio));
    initializeImageForest();
}

void MainWindow::initializeImageForest() {
    if(currentImagePath.isEmpty()) return;
    forest.clear(); sceneImage->clear();

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

void MainWindow::on_btnResetImage_clicked() {
    if(currentImagePath.isEmpty()) return;
    initializeImageForest();
    if(ui->labelCompressed) ui->labelCompressed->clear();
    if(ui->tableImage) ui->tableImage->setRowCount(0);
}

void MainWindow::on_btnStepImage_clicked() {
    if (forest.size() <= 1) return;
    std::sort(forest.begin(), forest.end(), compareNodes);
    HuffNode* left = forest[0]; HuffNode* right = forest[1];
    forest.erase(forest.begin()); forest.erase(forest.begin());
    forest.push_back(new HuffNode(left->f + right->f, left, right));
    drawForest(sceneImage);
}

void MainWindow::on_btnCompressImage_clicked() {
    if(currentImagePath.isEmpty()) return;
    while(forest.size() > 1) {
        std::sort(forest.begin(), forest.end(), compareNodes);
        HuffNode* left = forest[0]; HuffNode* right = forest[1];
        forest.erase(forest.begin()); forest.erase(forest.begin());
        forest.push_back(new HuffNode(left->f + right->f, left, right));
    }
    drawForest(sceneImage);

    HuffNode* tree = nullptr;
    long long rawDataSize = 0;
    string encoded = encodeImage(currentImagePath.toStdString(), tree, rawDataSize);
    if(encoded.empty()) return;

    unsigned char* decodedBytes = decodeImage(encoded, tree, rawDataSize);
    int w, h, c;
    unsigned char* temp = stbi_load(currentImagePath.toStdString().c_str(), &w, &h, &c, 0);
    stbi_image_free(temp);

    stbi_write_png("verified_output.png", w, h, c, decodedBytes, w*c);
    delete[] decodedBytes;

    QPixmap pComp("verified_output.png");
    if(ui->labelCompressed) {
        ui->labelCompressed->setVisible(true);
        ui->labelCompressed->setPixmap(pComp.scaled(ui->labelCompressed->size(), Qt::KeepAspectRatio));
    }

    long long rawSizeBytes = w * h * c;
    long long compressedSizeBytes = encoded.length() / 8;
    double ratio = 0.0;
    if(rawSizeBytes > 0) ratio = (1.0 - ((double)compressedSizeBytes / (double)rawSizeBytes)) * 100.0;

    ui->labelStats->setText("Original: " + QString::number(rawSizeBytes) + " bytes\nComp: " + QString::number(compressedSizeBytes) + " bytes\nRatio: " + QString::number(ratio, 'f', 2) + "%");

    if(ui->tableImage) {
        string *codes = getHuffmanCodesForImage(tree);
        std::map<int, int> fMap;
        unsigned char* img_data = stbi_load(currentImagePath.toStdString().c_str(), &w, &h, &c, 0);
        for(long long i=0; i<rawDataSize; i++) {
            int pixel = (int)img_data[i];
            int val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
            fMap[val]++;
        }
        stbi_image_free(img_data);
        displayTableInUI(ui->tableImage, fMap, codes, 511);
    }
}

void MainWindow::on_btnSaveHuff_clicked() {
    if(currentImagePath.isEmpty()) return;
    HuffNode* tree = nullptr;
    long long dataSize = 0;
    string encoded = encodeImage(currentImagePath.toStdString(), tree, dataSize);
    string treeString = serializeTree(tree);

    int w, h, c;
    unsigned char* temp = stbi_load(currentImagePath.toStdString().c_str(), &w, &h, &c, 0);
    stbi_image_free(temp);

    QString savePath = QFileDialog::getSaveFileName(this, "Save", "", "Huffman File (*.huff)");
    if(savePath.isEmpty()) return;

    QFile file(savePath);
    if(file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << QString("HUFF_IMG_V2") << w << h << c << QString::fromStdString(treeString) << QString::fromStdString(encoded);
        file.close();

        // --- CALL CUSTOM POPUP ---
        showCustomSuccessPopup("File successfully saved!");
    }
}

// ===============================================
// IMAGE DECOMPRESSION
// ===============================================

void MainWindow::on_btnOpenHuff_clicked() {
    QString openPath = QFileDialog::getOpenFileName(this, "Open", "", "Huffman File (*.huff)");
    if(openPath.isEmpty()) return;

    QFile file(openPath);
    if(file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        QString header, treeString, encoded;
        int w, h, c;

        in >> header;
        if(header == "HUFF_IMG_V2") {
            in >> w >> h >> c >> treeString >> encoded;
        } else {
            QMessageBox::warning(this, "Error", "Invalid Format");
            return;
        }

        HuffNode* root = deserializeTree(treeString.toStdString());
        forest.clear(); forest.push_back(root);
        drawForest(sceneImageDecomp);

        long long dataSize = w * h * c;
        unsigned char* decodedBytes = decodeImage(encoded.toStdString(), root, dataSize);


        stbi_write_png("temp_decomp.png", w, h, c, decodedBytes, w*c);
        QPixmap p("temp_decomp.png");
        ui->labelDecompImage->setPixmap(p.scaled(ui->labelDecompImage->size(), Qt::KeepAspectRatio));

        if(ui->tableImageDecomp) {
            string *codes = getHuffmanCodesForImage(root);
            std::map<int, int> restoredFreqs;
            extractFreqsFromTree(root, restoredFreqs);
            displayTableInUI(ui->tableImageDecomp, restoredFreqs, codes, 511);
        }

        delete[] decodedBytes;
        file.close();
    }
}

// ZOOM SLIDERS
void MainWindow::on_zoomSlider_valueChanged(int value) {
    qreal s = value / 100.0; ui->graphicsView->resetTransform(); ui->graphicsView->scale(s, s);
}
void MainWindow::on_zoomSliderImage_valueChanged(int value) {
    qreal s = value / 100.0; if(ui->graphicsViewImage) { ui->graphicsViewImage->resetTransform(); ui->graphicsViewImage->scale(s, s); }
}
void MainWindow::on_zoomSliderTextDecomp_valueChanged(int value) {
    qreal s = value / 100.0; if(ui->graphicsViewTextDecomp) { ui->graphicsViewTextDecomp->resetTransform(); ui->graphicsViewTextDecomp->scale(s, s); }
}
void MainWindow::on_zoomSliderImageDecomp_valueChanged(int value) {
    qreal s = value / 100.0; if(ui->graphicsViewImageDecomp) { ui->graphicsViewImageDecomp->resetTransform(); ui->graphicsViewImageDecomp->scale(s, s); }
}
