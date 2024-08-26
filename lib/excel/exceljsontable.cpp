#include "exceljsontable.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QImage>
#include <QtMath>
#include <QPainter>

ExcelJsonTable::ExcelJsonTable(QObject *parent)
    : QObject{parent}
{
}

void ExcelJsonTable::setTables(QJsonArray _titleArray, QJsonArray _tableArray)
{
    tableArray = _tableArray;
    titleArray = _titleArray;
}

void ExcelJsonTable::exportExcel(QString _outputPath, QList<int> _repeatedRows, bool _skipImages)
{
    outputPath = _outputPath;
    repeatedRows = _repeatedRows;

    QString sheetName = doc.currentSheet()->sheetName();
    QString newName = getSheetName(1);
    doc.renameSheet( sheetName, newName );
    skipImages = _skipImages;
    sheetIndex = 0;
    doc.selectSheet(sheetIndex);
    updateColumnWidthMap(sheetIndex);
    currentRow = 1;
    currentColumn = 1;

    QJsonArray  Row;
    QJsonObject obj;
    // write title
    writeSheetTitleArray();

    //write table
    for(int i=0; i < tableArray.size(); i++)
    {
        Row = tableArray[i].toArray();
        if(Row.size() == 0)
        {
            // new sheet
            // new sheet name is the next record name attribute
            sheetIndex++;
            QString sheetName = "sheet-"+QString::number(sheetIndex);
            if((i+1) < tableArray.size())
                sheetName = getSheetName(i+1);

            currentRow = 1;
            currentColumn = 1;

            doc.addSheet(sheetName);
            doc.selectSheet(sheetIndex);
            writeSheetTitleArray();
            writeRepeatedRows();
        }
        writeRow(Row);
    }

    doc.selectSheet(0);
    doc.saveAs(outputPath);
}

void ExcelJsonTable::writeSheetTitleArray()
{
    QJsonArray  Row;
    QJsonObject obj;
    // write title
    for(int i=0; i < titleArray.size(); i++)
    {
        Row = titleArray[i].toArray();
        writeRow(Row);
    }

    int columnCount = getMaxColumnCount(sheetIndex);
    QXlsx::CellRange range(currentRow, 1, currentRow, columnCount);
    QXlsx::Format format;
    format.setPatternBackgroundColor(QColor(QString("#55A")));
    doc.setRowHeight(currentRow,10);
    doc.mergeCells(range, format);
    currentRow++;
}

void ExcelJsonTable::writeRepeatedRows()
{
    foreach (int i, repeatedRows)
    {
        QJsonArray Row = tableArray[repeatedRows[i]].toArray();
        writeRow(Row);
    }
}

QXlsx::Format ExcelJsonTable::getFormat(QJsonObject Obj)
{
    QXlsx::Format format;

    QJsonObject style = Obj["style"].toObject();
    //name; width; height; color; background-color; font-family;  font-size; bold; align; border; row-span
    //QString name = style["name"].toString();
    QString color = style["color"].toString();
    QString backgroundColor = style["background-color"].toString();
    QString fontFamily = style["font-family"].toString();
    QString align =  style["align"].toString();

    int fontSize = style["font-size"].toInt();
    int border = 1;//style["border"].toInt();
    //int rowSpan = style["row-span"].toInt();
    bool bold = style["bold"].toBool();

    QFont font(fontFamily);
    font.setBold(bold);
    font.setPointSize(fontSize);
    format.setFont(font);
    format.setFontColor(QColor(color));
    format.setTextWrap(true);
    if(align.compare("center", Qt::CaseInsensitive) == 0)
        format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    else if(align.compare("right", Qt::CaseInsensitive) == 0)
        format.setHorizontalAlignment(QXlsx::Format::AlignRight);
    else
        format.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);

    format.setPatternBackgroundColor(QColor(backgroundColor));

    if(border > 2)
        format.setBorderStyle(QXlsx::Format::BorderThick);
    else if(border == 2)
        format.setBorderStyle(QXlsx::Format::BorderDouble);
    else if(border == 1)
        format.setBorderStyle(QXlsx::Format::BorderThin);
    else if(border == 0)
        format.setBorderStyle(QXlsx::Format::BorderNone);



    return format;
}

void ExcelJsonTable::setCellSize(int row, int column, QJsonObject Obj)
{
    QJsonObject style = Obj["style"].toObject();
    // width :  x 7 pt
    // height : pt
    double width = style["width"].toDouble() * 0.75 / 7;
    double height = style["height"].toDouble() * 0.75;
    if(width < 15)
        width = 15; // in excel file: width: 105 >> 105/7 = 15
    if(height < 30)
        height = 30;


    doc.setColumnWidth(column, width);
    doc.setRowHeight(row, height);
}

QJsonObject ExcelJsonTable::getSingleObject(QJsonArray Row)
{
    // text
    // img
    // in skipImage
    //      img text img
    //      img text
    //      text img

    if(Row.size() == 1)
    {
        if(skipImages)
        {
            if(Row[0].toObject().value("type").toString().compare("text", Qt::CaseInsensitive) == 0)
                return Row[0].toObject();
            else
                return {};
        }
        else
        {
            return Row[0].toObject();
        }
    }
    else if(Row.size() == 2)
    {
        if(skipImages)
        {
            bool item0Text = (Row[0].toObject().value("type").toString().compare("text", Qt::CaseInsensitive) == 0)? true :false;
            bool item1Text = (Row[1].toObject().value("type").toString().compare("text", Qt::CaseInsensitive) == 0)? true :false;

            // skipImage is true by default
            if( item0Text && !item1Text )
                return Row[0].toObject();
            else if( !item0Text && item1Text )
                return Row[1].toObject();
            else return {};
        }
        else return {};
    }
    else if(Row.size() == 3)
    {
        if(skipImages)
        {
            bool item0Text = (Row[0].toObject().value("type").toString().compare("text", Qt::CaseInsensitive) == 0)? true :false;
            bool item1Text = (Row[1].toObject().value("type").toString().compare("text", Qt::CaseInsensitive) == 0)? true :false;
            bool item2Text = (Row[2].toObject().value("type").toString().compare("text", Qt::CaseInsensitive) == 0)? true :false;


            // skipImage is true by default
            if(item0Text && !item1Text && !item2Text)
                return Row[0].toObject();
            else if(!item0Text && item1Text && !item2Text)
                return Row[1].toObject();
            else if(!item0Text && !item1Text && item2Text)
                return Row[2].toObject();
            else return {};
        }
        else
            return {};
    }

    return {};
}

void ExcelJsonTable::writeCell(int row, int column, QJsonObject Obj)
{
    // cell index starts from 1
    // sheet index starts from 0

    QString type = Obj["type"].toString();
    if(type.compare("img", Qt::CaseInsensitive) == 0)
        if(skipImages) return;

    QVariant value = Obj.value("value").toVariant();

    QXlsx::Format format = getFormat(Obj);
    setCellSize(row, column, Obj);


    if(type.compare("text", Qt::CaseInsensitive) == 0)
        doc.write(row, column, value, format);
    else
    {
        double w = Obj.value("style").toObject()["width"].toDouble();
        if(w < 140) w = 140; // excel width min: 15
        double h = Obj.value("style").toObject()["height"].toDouble();

        doc.write(currentRow, currentColumn, "", format); // set background and other attributes

        QImage baseImage(value.toString());
        QImage img(w, h, QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter painter(&img);
        double x = w/2 - baseImage.width()/2;
        double y = h/2 - baseImage.height()/2;
        painter.drawImage(x,y,baseImage);

        doc.insertImage(row-1, column-1, img);

        // insertImage index starts from 0
        // but write index starts from 1
    }
}

void ExcelJsonTable::writeRow(QJsonArray Row)
{
    QJsonObject obj;
    obj = getSingleObject(Row);
    bool goAhead = false;
    if(!obj.isEmpty())
    {
        writeOneItemInRow(obj);
        goAhead = true;
    }
    else
    {
        for(int i=0; i < Row.size(); i++)
        {
            obj = Row[i].toObject();
            writeCell(currentRow, currentColumn, obj);
            currentColumn++;

            if(!goAhead)
            {
                if(skipImages)
                {
                    //check text exisxts or not
                    QString t = obj.value("type").toString();
                    if(t.compare("text", Qt::CaseInsensitive) == 0)
                        goAhead = true;
                }
                else
                    goAhead = true;
            }
        }
    }

    if(goAhead)
    {
        currentRow++;
        currentColumn = 1;
    }
}

void ExcelJsonTable::writeOneItemInRow(QJsonObject obj)
{
    // text
    // img
    // in skipImage
    //      img text img
    //      img text
    //      text img

    int columnCount = getMaxColumnCount(sheetIndex);
    QXlsx::CellRange range(currentRow, 1, currentRow, columnCount);
    doc.mergeCells(range);

    QXlsx::Format format = getFormat(obj);
    QString type = obj.value("type").toString();
    doc.write(currentRow, currentColumn, "", format); // set background and other attributes

    double height = obj.value("style").toObject()["height"].toDouble() * 0.75;
    if(height < 30 ) height = 30;
    doc.setRowHeight(currentRow, height);

    if(type.compare("text", Qt::CaseInsensitive) == 0)
        doc.write(currentRow,1, obj.value("value").toVariant(),format);
    else
    {
        //img
        double w = getCellWidth(0, columnCount);
        if(w < 140) w = 140; // excel width min: 15
        double h = obj.value("style").toObject()["height"].toDouble();

        QImage baseImage(obj.value("value").toString());
        QImage img(w, h, QImage::Format_ARGB32);
        img.fill(Qt::transparent); // format.patternBackgroundColor()
        QPainter painter(&img);
        double x = w/2 - baseImage.width()/2;
        double y = h/2 - baseImage.height()/2;
        painter.drawImage(x,y,baseImage);

        doc.insertImage(currentRow-1, currentColumn-1, img);
    }
}

double ExcelJsonTable::getCellWidth(int startColumn, int endColumn)
{
    double width = 0;
    updateColumnWidthMap(sheetIndex);
    foreach (int column, columnWidth.keys())
        if(column >= startColumn && column <=endColumn)
            width += columnWidth.value(column);

    return width;
}

void ExcelJsonTable::updateWidth(QJsonArray &table, int column, double width)
{
    QJsonObject obj, style;
    QJsonArray Row;
    for(int r=0; r < table.size(); r++)
    {
        Row = table[r].toArray();
        obj = Row[column].toObject();
        style = obj["style"].toObject();
        style["width"] = width;
        Row.removeAt(column);
        Row.insert(column, obj);
        table.removeAt(r);
        table.insert(r, Row);
    }
}

void ExcelJsonTable::updateHeight(QJsonArray &table, int row, double height)
{
    QJsonObject obj, style;
    QJsonArray Row = table[row].toArray();

    for(int c=0; c < Row.size(); c++)
    {
        obj = Row[c].toObject();
        style = obj["style"].toObject();
        style["height"] = height;
        Row.removeAt(c);
        Row.insert(c, obj);
    }

    table.removeAt(row);
    table.insert(row, Row);
}

QString ExcelJsonTable::getSheetName(int row)
{
    if( tableArray[row].toArray().size() == 0 )
    {
        return QString("Sheet-").QString::number(row);
    }

    QJsonArray Row = tableArray[row].toArray();
    QJsonObject style = Row[0].toObject().value("style").toObject();
    return style["name"].toString();
}

int ExcelJsonTable::getStartRow(int sheetIndex)
{
    if(sheetIndex < 1) sheetIndex = 1;
    // find sheetIndex empty row
    if(sheetIndex == 1) return 1;

    int index = 1, temp, row = 0;
    QJsonArray Row;
    for(int i=0; i < tableArray.count(); i++)
    {
        Row = tableArray[i].toArray();
        temp = Row.count();
        if(temp == 0) index++;
        if(index == sheetIndex)
        {
            row = i+1;
            break;
        }
    }
    return row;
}

void ExcelJsonTable::updateColumnWidthMap(int sheetIndex)
{
    int startRow = 0;
    if(sheetIndex != 0)
        startRow = getStartRow(sheetIndex);
    columnWidth.clear();
    double val;
    QJsonArray Row = tableArray[startRow].toArray();
    for(int i=0;i < Row.size(); i++)
    {
        val = Row[i].toObject().value("style").toObject().value("width").toDouble();
        columnWidth[i] = val;
    }
}

int ExcelJsonTable::getMaxColumnCount(int sheetIndex)
{
    int temp, count = 0;
    QJsonArray Row;
    int row = getStartRow(sheetIndex);
    for(int i=row; i < tableArray.count(); i++)
    {
        Row = tableArray[i].toArray();
        temp = Row.count();
        if(temp > count)
            count = temp;
        if(temp == 0) break;
    }

    return count;
}

int ExcelJsonTable::getSheetCount()
{
    // an empty row means new sheet
    int temp, count = 1;
    QJsonArray Row;
    for(int i=0; i < tableArray.count(); i++)
    {
        Row = tableArray[i].toArray();
        temp = Row.count();
        if(temp == 0)
            count++;
    }

    return count;
}

QJsonArray ExcelJsonTable::excelToJson(QString filePath, int sheetIndex, int headerRow, int startColumn, int endColumn)
{
    /*
      [
           [{menu1: val1}, {menu2: val2}],
           [].
           ...
      ]
     */
    QXlsx::Document file(filePath);
    QJsonArray jTable;
    QMap<int, QString> headers;
    QString val;
    if(endColumn == -1)
        //to the end
        endColumn = file.dimension().lastColumn();

    int lastRow = file.dimension().lastRow();

    if(file.load())
    {
        file.selectSheet(sheetIndex);
        //get headers
        for(int i=startColumn; i <= endColumn; i++)
            headers[i] = file.cellAt(headerRow,i)->value().toString();

        for(int r=headerRow+1; r <= lastRow; r++)
        {
            QJsonArray array;
            for(int c=startColumn; c <= endColumn; c++)
            {
                val = file.cellAt(r,c)->value().toString();
                QJsonObject obj;
                obj[headers[c]] = val;

                array.append(obj);
            }

            jTable.append(array);
        }
    }
    return jTable;
}

