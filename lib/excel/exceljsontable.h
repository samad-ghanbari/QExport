#ifndef EXCELJSONTABLE_H
#define EXCELJSONTABLE_H

// table : [ [][sheet0] [][sheet1] ... ]
#include "modules/QXlsx/header/xlsxdocument.h"
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>

class ExcelJsonTable : public QObject
{
    Q_OBJECT
public:
    explicit ExcelJsonTable(QObject *parent = nullptr);
    void setTables(QJsonArray _titleArray, QJsonArray _tableArray);
    void exportExcel(QString _outputPath, QList<int> _repeatedRows, bool _skipImages = false);
    void writeSheetTitleArray();
    void writeRepeatedRows();
    QXlsx::Format getFormat(QJsonObject Obj);
    void setCellSize(int row, int column, QJsonObject Obj);
    QJsonObject getSingleObject(QJsonArray Row);
    void writeCell(int row, int column, QJsonObject Obj);
    void writeRow(QJsonArray Row);
    void writeOneItemInRow(QJsonObject obj);
    double getCellWidth(int startColumn, int endColumn);
    void updateWidth(QJsonArray &table, int column, double width);
    void updateHeight(QJsonArray &table, int row,  double height);
    QString getSheetName(int row);
    int getStartRow(int sheetIndex);
    void updateColumnWidthMap(int sheetIndex);
    int getMaxColumnCount(int sheetIndex);
    int getSheetCount();
    QJsonArray excelToJson(QString filePath, int sheetIndex = 0, int headerRow = 1, int startColumn = 1, int endColumn = -1);

private:
    QJsonArray titleArray, tableArray;
    QString outputPath, sheetName;
    QList<int> repeatedRows;
    QMap<int, double> columnWidth;
    QXlsx::Document doc;
    int sheetIndex;
    int currentRow, currentColumn;
    bool skipImages;

};

#endif // EXCELJSONTABLE_H
