#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDebug>
#include <QDataStream>
#include <QTextStream>
#include <QResizeEvent>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Adding icon to the app
    this->setWindowIcon(QIcon(":/icon_ore2.png"));


    // Adding a watermark at the bottom right

    this->watermarkLabel = new QLabel(this);

    QPixmap watermark(":/hachi.png");

    if (!watermark.isNull())
    {
        watermark = watermark.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->watermarkLabel->setPixmap(watermark);
    this->watermarkLabel->adjustSize();
    this->watermarkLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->watermarkLabel->show();


    int startPadding = 0;

    // Forcing the position of watermark to bottom right
    int startX = (this->width() > 0) ? this->width() - this->watermarkLabel->width() - startPadding : 100;
    int startY = (this->height() > 0) ? this->height() - this->watermarkLabel->height() - startPadding : 100;

    this->watermarkLabel->move(startX, startY);



    this->setStyleSheet(
        "QMainWindow { background-color: #121212; }"
        "QWidget { font-family: 'Segoe UI', sans-serif; color: #e0e0e0; }"
        );

    // Text that shows in the input boxes in text comp

    if (this->ui->textInput)
    {
        this->ui->textInput->setPlaceholderText("Input Text");
    }

    if (this->ui->textInputFreqs)
    {
        this->ui->textInputFreqs->setPlaceholderText("Input Tree");
    }

    if (this->ui->textInputBits)
    {
        this->ui->textInputBits->setPlaceholderText("Input Encoded Bits");
    }


    // CSS for widgets

    this->ui->labelTitle->setStyleSheet(
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

    this->ui->frameText->setStyleSheet(cardStyle + headerStyle + btnStyle);
    this->ui->frameImage->setStyleSheet(cardStyle + headerStyle + btnStyle);


    // Sliders in graphic tree visualization box

    this->sceneText = new QGraphicsScene(this);
    this->ui->graphicsView->setScene(this->sceneText);
    this->ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    this->sceneImage = new QGraphicsScene(this);
    if (this->ui->graphicsViewImage)
    {
        this->ui->graphicsViewImage->setScene(this->sceneImage);
        this->ui->graphicsViewImage->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    this->sceneTextDecomp = new QGraphicsScene(this);
    if (this->ui->graphicsViewTextDecomp)
    {
        this->ui->graphicsViewTextDecomp->setScene(this->sceneTextDecomp);
        this->ui->graphicsViewTextDecomp->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    this->sceneImageDecomp = new QGraphicsScene(this);
    if (this->ui->graphicsViewImageDecomp)
    {
        this->ui->graphicsViewImageDecomp->setScene(this->sceneImageDecomp);
        this->ui->graphicsViewImageDecomp->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    this->ui->zoomSlider->setRange(10, 200);
    this->ui->zoomSlider->setValue(100);

    if (this->ui->zoomSliderImage)
    {
        this->ui->zoomSliderImage->setRange(10, 200);
        this->ui->zoomSliderImage->setValue(100);
    }

    if (this->ui->zoomSliderTextDecomp)
    {
        this->ui->zoomSliderTextDecomp->setRange(10, 200);
        this->ui->zoomSliderTextDecomp->setValue(100);
    }

    if (this->ui->zoomSliderImageDecomp)
    {
        this->ui->zoomSliderImageDecomp->setRange(10, 200);
        this->ui->zoomSliderImageDecomp->setValue(100);
    }

    if (this->ui->stackedWidget->count() > 0)
    {
        this->ui->stackedWidget->setCurrentWidget(this->ui->pageHome);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}



// Custom pop up for when file is saved (image shown)

void MainWindow::showCustomSuccessPopup(QString message)
{
    QDialog dialog(this);

    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    QLabel *imgLabel = new QLabel(&dialog);

    QPixmap icon(":/saika.png");

    if (!icon.isNull())
    {
        imgLabel->setPixmap(icon.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imgLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(imgLabel);
    }

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


    // CSS for buttons text and overall theme
    dialog.setStyleSheet(
        "QDialog {"
        "    background-color: #2b2b2b;"
        "    border: 2px solid #555555;"
        "    border-radius: 12px;"
        "}"
        "QLabel {"
        "    color: #e0e0e0;"
        "    font-size: 18px;"
        "    font-family: 'Segoe UI', sans-serif;"
        "}"
        "QPushButton {"
        "    background-color: #0078d4;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 10px 30px;"
        "    font-size: 14px;"
        "    font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0063b1;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #004a83;"
        "}"
        );

    // Show the dialog and wait for user to dismiss it
    dialog.exec();
}


//Watermark forced to front

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (this->watermarkLabel && this->watermarkLabel->isVisible())
    {
        int padding = 0; // Space from the edge

        int x = this->width() - this->watermarkLabel->width() - padding;
        int y = this->height() - this->watermarkLabel->height() - padding;

        this->watermarkLabel->move(x, y);
        this->watermarkLabel->raise();
    }
}


// Buttons on homepage

void MainWindow::on_btnTextMode_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->pageText);
}

void MainWindow::on_btnImageMode_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->pageImage);
}

void MainWindow::on_btnDecompressTextMode_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->pageTextDecomp);
}

void MainWindow::on_btnDecompressImageMode_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->pageImageDecomp);
}

void MainWindow::on_btnBack_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->pageHome);
}

void MainWindow::on_btnBack2_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->pageHome);
}

void MainWindow::on_btnBack3_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->pageHome);
}

void MainWindow::on_btnBack4_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->pageHome);
}


// Helper functions for visualization

// Recursive function to get total leaves
int MainWindow::getLeafCount(HuffNode* node)
{
    if (!node) return 0;

    if (!node->left && !node->right) return 1;

    return getLeafCount(node->left) + getLeafCount(node->right);
}


// recursively draws tree by
// diving into the huffman nodes and
// calculating x, y coords based on leaves
void MainWindow::drawTreeRecursive(QGraphicsScene* sc, HuffNode* node, double leftBound, double rightBound, double y)
{
    if (!node) return;

    double x = (leftBound + rightBound) / 2.0;
    double radius = 40;
    double vGap = 80;

    if (node->left)
    {
        int leftLeaves = getLeafCount(node->left);
        int totalLeaves = getLeafCount(node);

        if (totalLeaves == 0) totalLeaves = 1;

        double splitPoint = leftBound + (rightBound - leftBound) * ((double)leftLeaves / totalLeaves);
        double childX = (leftBound + splitPoint) / 2.0;

        sc->addLine(x, y + radius/2, childX, y + vGap, QPen(Qt::white, 2));
        drawTreeRecursive(sc, node->left, leftBound, splitPoint, y + vGap);
    }

    if (node->right)
    {
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
    if (!node->left && !node->right)
    {
        if (node->symbol == 32) txt = "Sp";
        else if (node->symbol == 10) txt = "\\n";
        else if (node->symbol < 256 && isprint(node->symbol)) txt = QString(QChar(node->symbol));
        else txt = QString::number(node->symbol);
    }
    else
    {
        txt = QString::number(node->f);
    }

    QGraphicsTextItem* textItem = sc->addText(txt);
    textItem->setDefaultTextColor(Qt::black);
    textItem->setPos(x - textItem->boundingRect().width()/2, y - textItem->boundingRect().height()/2);
    textItem->setZValue(3);
}


void MainWindow::drawForest(QGraphicsScene* targetScene)
{
    targetScene->clear();
    double currentX = 0;
    double unitWidth = 50;
    double startY = 60;

    for (HuffNode* tree : forest)
    {
        int leaves = getLeafCount(tree);
        double treeWidth = leaves * unitWidth;
        drawTreeRecursive(targetScene, tree, currentX, currentX + treeWidth, startY);
        currentX += treeWidth + 100;
    }
    targetScene->setSceneRect(0, 0, currentX, 2000);
}


void MainWindow::displayTableInUI(QTableWidget* table, std::map<int, int> freqs, std::string *codes, int range)
{
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Symbol", "Freq", "Code"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    for (int i = 0; i < range; i++)
    {
        bool hasCode = !codes[i].empty();
        int f = freqs.count(i) ? freqs[i] : 0;

        if (f > 0 || hasCode)
        {
            table->insertRow(row);
            QString charDisplay = (range == 256 && i < 256) ? (i==32?"Space":(i==10?"\\n":QString(QChar(i)))) : QString::number(i);
            table->setItem(row, 0, new QTableWidgetItem(charDisplay));
            table->setItem(row, 1, new QTableWidgetItem(QString::number(f)));
            table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(codes[i])));
            row++;
        }
    }
}


void MainWindow::extractFreqsFromTree(HuffNode* root, std::map<int, int>& map)
{
    if (!root) return;

    if (!root->left && !root->right)
    {
        map[root->symbol] = root->f;
    }

    extractFreqsFromTree(root->left, map);
    extractFreqsFromTree(root->right, map);
}


//Text Compression

void MainWindow::on_btnReset_clicked()
{
    QString inputText = this->ui->textInput->toPlainText();

    if (inputText.isEmpty()) return;

    this->forest.clear();
    this->sceneText->clear();

    std::string s = inputText.toStdString();

    // First we initialize an array of 256 chars (for ASCII vals)
    int charFreqs[256] = {0};

    for (char c : s)
        charFreqs[(unsigned char)c]++;

    // Then we insert the chars having more than 1 freq to the forest
    for (int i = 0; i < 256; i++)
    {
        if (charFreqs[i] > 0)
            this->forest.push_back(new HuffNode(i, charFreqs[i]));
    }

    std::sort(this->forest.begin(), this->forest.end(), compareNodes);
    this->drawForest(this->sceneText);

    this->ui->lblTextStats->clear();
    this->ui->textOutput->clear();
    this->ui->tableWidget->setRowCount(0);
}


void MainWindow::on_btnStep_clicked()
{
    if (this->forest.size() <= 1) return;

    // Sort forest, pop first two elements and join them
    std::sort(this->forest.begin(), this->forest.end(), compareNodes);

    HuffNode* left = this->forest[0];
    HuffNode* right = this->forest[1];

    this->forest.erase(this->forest.begin());
    this->forest.erase(this->forest.begin());

    this->forest.push_back(new HuffNode(left->f + right->f, left, right));

    this->drawForest(this->sceneText);
}


void MainWindow::on_btnCompressText_clicked()
{
    this->on_btnReset_clicked();

    if (this->forest.empty()) return;

    // Continue this step till only one node remains
    while (this->forest.size() > 1)
    {
        std::sort(this->forest.begin(), this->forest.end(), compareNodes);

        HuffNode* left = this->forest[0];
        HuffNode* right = this->forest[1];

        this->forest.erase(this->forest.begin());
        this->forest.erase(this->forest.begin());

        this->forest.push_back(new HuffNode(left->f + right->f, left, right));
    }

    this->drawForest(this->sceneText);

    std::string s = this->ui->textInput->toPlainText().toStdString();
    HuffNode* root = this->forest[0];
    string *codes = getHuffmanCodes(root);
    string encoded = encode(s, root);

    this->ui->textOutput->setText(QString::fromStdString(encoded));

    long long origBits = s.length() * 8;
    long long compBits = encoded.length();

    double ratio = 0.0;
    if (origBits > 0)
        ratio = (1.0 - ((double)compBits / (double)origBits)) * 100.0;

    this->ui->lblTextStats->setText("Original: " + QString::number(origBits) + " bits\nNew: " + QString::number(compBits) + " bits\nRatio: " + QString::number(ratio, 'f', 2) + "%");

    std::map<int, int> fMap;
    for (char c : s)
        fMap[(unsigned char)c]++;

    displayTableInUI(this->ui->tableWidget, fMap, codes, 256);
}


void MainWindow::on_btnSaveTextHuff_clicked()
{
    if (this->forest.empty()) return;

    QString encodedText = this->ui->textOutput->toPlainText();
    string treeString = serializeTree(this->forest[0]);

    QString savePath = QFileDialog::getSaveFileName(this, "Save", "", "Text Files (*.txt)");
    if (savePath.isEmpty()) return;

    QFile file(savePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "--- TREE KEY ---\n" << QString::fromStdString(treeString) << "\n";
        out << "--- BINARY CODE ---\n" << encodedText << "\n";
        file.close();

        showCustomSuccessPopup("File successfully saved!");
    }
}


// Text decompression

void MainWindow::on_btnDecompressTextAction_clicked()
{
    QString treeKey = this->ui->textInputFreqs->toPlainText();
    QString bits = this->ui->textInputBits->toPlainText();

    if (treeKey.isEmpty() || bits.isEmpty()) return;

    HuffNode* root = deserializeTree(treeKey.toStdString());
    if (!root)
    {
        QMessageBox::warning(this, "Error", "Invalid Tree Key.");
        return;
    }

    // Visualizing
    this->forest.clear();
    this->forest.push_back(root);
    this->drawForest(this->sceneTextDecomp);

    // Decoding
    string decoded = decode(bits.toStdString(), root);
    this->ui->textOutputDecomp->setText(QString::fromStdString(decoded));

    // Filling Table with Restored Frequencies
    if (this->ui->tableTextDecomp)
    {
        string *codes = getHuffmanCodes(root);
        std::map<int, int> restoredFreqs;
        extractFreqsFromTree(root, restoredFreqs);
        displayTableInUI(this->ui->tableTextDecomp, restoredFreqs, codes, 256);
    }
}


// Image compression

void MainWindow::on_btnSelectImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (fileName.isEmpty()) return;

    this->currentImagePath = fileName;
    QPixmap p(fileName);
    this->ui->labelOriginal->setPixmap(p.scaled(this->ui->labelOriginal->size(), Qt::KeepAspectRatio));
    initializeImageForest();
}


void MainWindow::initializeImageForest()
{
    if (this->currentImagePath.isEmpty()) return;

    this->forest.clear();
    this->sceneImage->clear();

    int w, h, c;
    unsigned char* img_data = stbi_load(this->currentImagePath.toStdString().c_str(), &w, &h, &c, 0);
    if (!img_data) return;

    int freqs[511] = {0};
    long long size = w * h * c;

    for (long long i = 0; i < size; i++)
    {
        int pixel = (int)img_data[i];
        int val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
        freqs[val]++;
    }
    stbi_image_free(img_data);

    for (int i = 0; i < 511; i++)
    {
        if (freqs[i] > 0)
            this->forest.push_back(new HuffNode(i, freqs[i]));
    }

    std::sort(this->forest.begin(), this->forest.end(), compareNodes);
    this->drawForest(this->sceneImage);
}


void MainWindow::on_btnResetImage_clicked()
{
    if (this->currentImagePath.isEmpty()) return;

    initializeImageForest();

    if (this->ui->labelCompressed)
        this->ui->labelCompressed->clear();

    if (this->ui->tableImage)
        this->ui->tableImage->setRowCount(0);
}


void MainWindow::on_btnStepImage_clicked()
{
    if (this->forest.size() <= 1) return;

    std::sort(this->forest.begin(), this->forest.end(), compareNodes);

    HuffNode* left = this->forest[0];
    HuffNode* right = this->forest[1];

    this->forest.erase(this->forest.begin());
    this->forest.erase(this->forest.begin());

    this->forest.push_back(new HuffNode(left->f + right->f, left, right));

    this->drawForest(this->sceneImage);
}


void MainWindow::on_btnCompressImage_clicked()
{
    if (this->currentImagePath.isEmpty()) return;

    while (this->forest.size() > 1)
    {
        std::sort(this->forest.begin(), this->forest.end(), compareNodes);

        HuffNode* left = this->forest[0];
        HuffNode* right = this->forest[1];

        this->forest.erase(this->forest.begin());
        this->forest.erase(this->forest.begin());

        this->forest.push_back(new HuffNode(left->f + right->f, left, right));
    }

    this->drawForest(this->sceneImage);

    HuffNode* tree = nullptr;
    long long rawDataSize = 0;
    string encoded = encodeImage(this->currentImagePath.toStdString(), tree, rawDataSize);

    if (encoded.empty()) return;

    unsigned char* decodedBytes = decodeImage(encoded, tree, rawDataSize);
    int w, h, c;
    unsigned char* temp = stbi_load(this->currentImagePath.toStdString().c_str(), &w, &h, &c, 0);
    stbi_image_free(temp);

    stbi_write_png("verified_output.png", w, h, c, decodedBytes, w*c);
    delete[] decodedBytes;

    QPixmap pComp("verified_output.png");
    if (this->ui->labelCompressed)
    {
        this->ui->labelCompressed->setVisible(true);
        this->ui->labelCompressed->setPixmap(pComp.scaled(this->ui->labelCompressed->size(), Qt::KeepAspectRatio));
    }

    long long rawSizeBytes = w * h * c;
    long long compressedSizeBytes = encoded.length() / 8;

    double ratio = 0.0;
    if (rawSizeBytes > 0)
        ratio = (1.0 - ((double)compressedSizeBytes / (double)rawSizeBytes)) * 100.0;

    this->ui->labelStats->setText("Original: " + QString::number(rawSizeBytes) + " bytes\nComp: " + QString::number(compressedSizeBytes) + " bytes\nRatio: " + QString::number(ratio, 'f', 2) + "%");

    if (this->ui->tableImage)
    {
        string *codes = getHuffmanCodesForImage(tree);
        std::map<int, int> fMap;
        unsigned char* img_data = stbi_load(this->currentImagePath.toStdString().c_str(), &w, &h, &c, 0);

        for (long long i = 0; i < rawDataSize; i++)
        {
            int pixel = (int)img_data[i];
            int val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
            fMap[val]++;
        }
        stbi_image_free(img_data);
        displayTableInUI(this->ui->tableImage, fMap, codes, 511);
    }
}


void MainWindow::on_btnSaveHuff_clicked()
{
    if (this->currentImagePath.isEmpty()) return;

    HuffNode* tree = nullptr;
    long long dataSize = 0;
    string encoded = encodeImage(this->currentImagePath.toStdString(), tree, dataSize);
    string treeString = serializeTree(tree);

    int w, h, c;
    unsigned char* temp = stbi_load(this->currentImagePath.toStdString().c_str(), &w, &h, &c, 0);
    stbi_image_free(temp);

    QString savePath = QFileDialog::getSaveFileName(this, "Save", "", "Huffman File (*.huff)");
    if (savePath.isEmpty()) return;

    QFile file(savePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out << QString("HUFF_IMG_V2") << w << h << c << QString::fromStdString(treeString) << QString::fromStdString(encoded);
        file.close();

        showCustomSuccessPopup("File successfully saved!");
    }
}


// Image decompression

void MainWindow::on_btnOpenHuff_clicked()
{
    QString openPath = QFileDialog::getOpenFileName(this, "Open", "", "Huffman File (*.huff)");
    if (openPath.isEmpty()) return;

    QFile file(openPath);
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);
        QString header, treeString, encoded;
        int w, h, c;

        in >> header;

        if (header == "HUFF_IMG_V2")
        {
            in >> w >> h >> c >> treeString >> encoded;
        }
        else
        {
            QMessageBox::warning(this, "Error", "Invalid Format");
            return;
        }

        HuffNode* root = deserializeTree(treeString.toStdString());
        this->forest.clear();
        this->forest.push_back(root);
        this->drawForest(this->sceneImageDecomp);

        long long dataSize = w * h * c;
        unsigned char* decodedBytes = decodeImage(encoded.toStdString(), root, dataSize);

        stbi_write_png("temp_decomp.png", w, h, c, decodedBytes, w*c);
        QPixmap p("temp_decomp.png");
        this->ui->labelDecompImage->setPixmap(p.scaled(this->ui->labelDecompImage->size(), Qt::KeepAspectRatio));

        if (this->ui->tableImageDecomp)
        {
            string *codes = getHuffmanCodesForImage(root);
            std::map<int, int> restoredFreqs;
            extractFreqsFromTree(root, restoredFreqs);
            displayTableInUI(this->ui->tableImageDecomp, restoredFreqs, codes, 511);
        }

        delete[] decodedBytes;
        file.close();
    }
}


// Zoom Sliders for zooming in and out of the graphic tree visualization
void MainWindow::on_zoomSlider_valueChanged(int value)
{
    qreal s = value / 100.0;
    this->ui->graphicsView->resetTransform();
    this->ui->graphicsView->scale(s, s);
}

void MainWindow::on_zoomSliderImage_valueChanged(int value)
{
    qreal s = value / 100.0;
    if (this->ui->graphicsViewImage)
    {
        this->ui->graphicsViewImage->resetTransform();
        this->ui->graphicsViewImage->scale(s, s);
    }
}

void MainWindow::on_zoomSliderTextDecomp_valueChanged(int value)
{
    qreal s = value / 100.0;
    if (this->ui->graphicsViewTextDecomp)
    {
        this->ui->graphicsViewTextDecomp->resetTransform();
        this->ui->graphicsViewTextDecomp->scale(s, s);
    }
}

void MainWindow::on_zoomSliderImageDecomp_valueChanged(int value)
{
    qreal s = value / 100.0;
    if (this->ui->graphicsViewImageDecomp)
    {
        this->ui->graphicsViewImageDecomp->resetTransform();
        this->ui->graphicsViewImageDecomp->scale(s, s);
    }
}
