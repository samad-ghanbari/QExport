#ifndef PDFJSONTABLE_H
#define PDFJSONTABLE_H

#include <QObject>
#include <QList>
#include <QPen>
#include <QFont>
#include <QPageSize>
#include <QPrinter>

class QPrinter;
class QPainter;

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

/*
A0  841 x 1189 mm
A1  594 x 841 mm
A2  420 x 594 mm
A3  297 x 420 mm
A4  210 x 297 mm
A5  148 x 210 mm
A6  105 x 148 mm
A7  74 x 105 mm
A8  52 x 74 mm
A9  37 x 52 mm

B0  1030 x 1456 mm
B1  728 x 1030 mm
B2  515 x 728 mm
B3  364 x 515 mm
B4  257 x 364 mm
B5  182 x 257 mm
B6  128 x 182 mm
B7  91 x 128 mm
B8  64 x 91 mm
B9  45 x 64 mm
B10 32 x 45 mm

Letter 216 x 279 mm

A3: 1550 : top-5 bottom-5 right-18 left-18
*/

class PdfJsonTable : public QObject
{
    Q_OBJECT
public:
    explicit PdfJsonTable(QString outputPath, QString Creator, QString DocName, QString pageSize="A3", QString orientation="landscape", qreal pageMarginLeft=18, qreal pageMarginTop=5, qreal pageMarginRight=18, qreal pageMarginBottom=5, QObject *parent = nullptr);
    void setPageHeader(QJsonArray *_headerArray);
    void setTable(QJsonArray *_table, QList<int> _repeatedRows={}); // empty array means no repeat row
    //#if QT_VERSION > 0x051210
        bool setPage();
//    #else
//        void setPaper(QPrinter::PageSize pageSize, qreal pageMarginLeft=4, qreal pageMarginTop=2, qreal pageMarginRight=4, qreal pageMarginBottom=1.5);
//    #endif
    int getViewPortWidth();
    int getViewPortHeight();
    void preparePage();

    bool print();
    double getHeight(int startRow, int endRow);
    double getHeight(int row);
    double getHeight(QJsonArray Row);
    QJsonObject updateObjectStyle(QJsonObject _object, QString _key, double _val);

    bool printCell(int row, int column, QJsonObject obj);
    void respanRow(int row, int column); // re-rowspan
    void updateTableStyle(int row, int column, QJsonObject style);

    double getRowSpanHeight(int row, int column);
    void updateTableObjectWidth(QJsonArray table);
    void updateRowObjectWidth(QJsonArray row);
    QString lastError();

private:
    QJsonArray pageHeaderArray, tableArray;
    QList<int> repeatedRows;
    QPainter *painter;
    QPrinter *printer;
    QPen pen; // for table borders
    int pageNumber;
    QString error;
    //paper size
    double paperWidth, paperHeight, currentHeight;
};

#endif // PDFJSONTABLE_H
