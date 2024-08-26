#include "pdfjsontable.h"
#include <QPainter>
#include <QPrinter>
#include <QDebug>

PdfJsonTable::PdfJsonTable(QString outputPath, QString Creator, QString DocName, QString _pageSize, QString orientation, qreal pageMarginLeft, qreal pageMarginTop, qreal pageMarginRight, qreal pageMarginBottom, QObject *parent)
    : QObject{parent}, painter(new QPainter()), printer(new QPrinter(QPrinter::ScreenResolution))
{

    pageNumber = 1;
    error = "";
    // screanResolution dpi 96
    // A3 11.7 in x 16.5 in
    // dpi 96 standard
    // 11.7 * 96 = 1123.2
    // 16.5 * 96 = 1584 >> 1588 1123
    // default padding left right 11
    // default padding top bottom 13.1

    QMarginsF margins(pageMarginLeft/96,pageMarginTop/96,pageMarginRight/96, pageMarginBottom/96);
    printer->setDocName(DocName);
    printer->setCreator(Creator);
    printer->setOutputFileName(outputPath);

#if QT_VERSION > 0x051210
    QMap<QString, QPageSize::PageSizeId> PAGE_SIZE_ID = {
        {"A0", QPageSize::A0}, {"A1", QPageSize::A1}, {"A2", QPageSize::A2}, {"A3", QPageSize::A3}, {"A4", QPageSize::A4}, {"A5", QPageSize::A5}, {"A6", QPageSize::A6}, {"A7", QPageSize::A7}, {"A8", QPageSize::A8}, {"A9", QPageSize::A9}, {"A10", QPageSize::A10},
        {"B0", QPageSize::B0}, {"B1", QPageSize::B1}, {"B2", QPageSize::B2}, {"B3", QPageSize::B3}, {"B4", QPageSize::B4}, {"B5", QPageSize::B5}, {"B6", QPageSize::B6}, {"B7", QPageSize::B7}, {"B8", QPageSize::B8}, {"B9", QPageSize::B9}, {"B10", QPageSize::B10}
    };
    QMap<QString, QPageLayout::Orientation> ORIENTATION ={ {"landscape", QPageLayout::Landscape}, {"portrait", QPageLayout::Portrait} };

    printer->setPageOrientation(ORIENTATION.value(orientation));

    printer->setPageSize(PAGE_SIZE_ID.value(_pageSize));

    printer->setPageMargins(margins, QPageLayout::Inch );
    // A3 : 1580 1119

#else
    QMap<QString, QPrinter::PaperSize> PAGE_SIZE_ID = {
        {"A0", QPrinter::A0}, {"A1", QPrinter::A1}, {"A2", QPrinter::A2}, {"A3", QPrinter::A3}, {"A4", QPrinter::A4}, {"A5", QPrinter::A5}, {"A6", QPrinter::A6}, {"A7", QPrinter::A7}, {"A8", QPrinter::A8}, {"A9", QPrinter::A9}, {"A10", QPrinter::A10},
        {"B0", QPrinter::B0}, {"B1", QPrinter::B1}, {"B2", QPrinter::B2}, {"B3", QPrinter::B3}, {"B4", QPrinter::B4}, {"B5", QPrinter::B5}, {"B6", QPrinter::B6}, {"B7", QPrinter::B7}, {"B8", QPrinter::B8}, {"B9", QPrinter::B9}, {"B10", QPrinter::B10}
    };
    QMap<QString, QPrinter::Orientation> ORIENTATION ={ {"landscape", QPrinter::Landscape}, {"portrait", QPrinter::Portrait} };

    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOrientation(ORIENTATION.value(orientation));
    printer->setPaperSize(PAGE_SIZE_ID.value(_pageSize));
    printer->setPageMargins(pageMarginLeft/96,pageMarginTop/96,pageMarginRight/96, pageMarginBottom/96, QPrinter::Inch );
    // A3 : 1580 1119

#endif

    if(!painter->begin(printer))
    {
        error = "painter->begin error.";
    }
    else
    {
        paperWidth = painter->viewport().width();
        paperHeight = painter->viewport().height();
    }
}

void PdfJsonTable::setPageHeader(QJsonArray *_headerArray)
{
    pageHeaderArray = *_headerArray;
}

void PdfJsonTable::setTable(QJsonArray *_table, QList<int> _repeatedRows)
{
    tableArray = *_table;
    repeatedRows = _repeatedRows; // row indeces
}

int PdfJsonTable::getViewPortWidth()
{
    return painter->viewport().width();
}

int PdfJsonTable::getViewPortHeight()
{
    return painter->viewport().height();
}

void PdfJsonTable::preparePage()
{
    painter->resetTransform();


    QJsonArray row;
    QJsonObject obj;
    int headerHeight = 0;
    bool newRow;

    for(int i=0; i < pageHeaderArray.count(); i++)
    {
        row = pageHeaderArray[i].toArray();
        newRow = true;

        for(int j=0; j < row.count(); j++)
        {
            obj = row[j].toObject();
            if(newRow)
            {
                headerHeight += obj.value("style").toObject().value("height").toDouble();
                newRow = false;
            }

            if(!printCell(i, j, obj))
                break;
        }
        painter->resetTransform();
        painter->translate(0, headerHeight);
    }

    painter->resetTransform();
    painter->setFont(QFont("tahoma", 10));
    painter->setPen(QPen(QColor(0, 0, 0), 2));
    QRect rec(0, paperHeight-20, paperWidth, paperHeight);
    painter->drawText(rec,Qt::AlignHCenter | Qt::AlignTop, QString("Page %1").arg(pageNumber));

    painter->translate(0, headerHeight);

    painter->drawLine(0, 10, paperWidth , 10);
    painter->translate(0, 20);

    currentHeight = headerHeight + 30;

    // print repeated table-header
    if(pageNumber > 1)
    {
        double rowHeight;
        for(int i =0; i < repeatedRows.count(); i++)
        {
            painter->save();
            row = tableArray[repeatedRows[i]].toArray();
            rowHeight = getHeight(row);
            for(int j=0; j < row.count(); j++)
            {
                obj = row[j].toObject();
                if(!printCell(i, j, obj))
                    break;
            }
            currentHeight += rowHeight;
            painter->restore();
            painter->translate(0, rowHeight);
        }
    }

    pageNumber += 1;
}

bool PdfJsonTable::print()
{
    preparePage();

    QJsonArray row;
    QJsonObject obj;
    int rowSpan ;
    double rowHeight, height;

    // print content
    for(int i=0; i < tableArray.count(); i++ )
    {
        painter->save();
        row = tableArray[i].toArray();
        //check empty array
        if(row.count() == 0)
        {
            painter->restore();
            printer->newPage();
            preparePage();
            continue;
        }

        rowHeight = getHeight(i);
        if( (currentHeight + rowHeight) > (paperHeight - 30)  )
        {
            painter->restore();
            printer->newPage();
            preparePage();
            painter->save();
        }

        for(int j=0; j < row.count(); j++)
        {
            obj = row[j].toObject();
            // check if object fits in current page or need to create new page
            rowSpan = obj["style"].toObject()["row-span"].toInt();
            if(rowSpan > 1)
            {
                height = getRowSpanHeight(i,j);

                if( (currentHeight + height) > (paperHeight - 30)  )
                {
                    //column should be respanned to fit the page
                    respanRow(i, j);
                }
            }


            if(!printCell(i, j, obj))
                break;
        }
        currentHeight += rowHeight;
        painter->restore();
        painter->translate(0, rowHeight);
    }

    painter->end();

    return true;
}

double PdfJsonTable::getHeight(int startRow, int endRow)
{
    double height=0;
    int lastRow = tableArray.count() -1 ;
    if(endRow > lastRow) endRow = lastRow;

    for(int i=startRow;  i <= endRow; i++)
        height += getHeight(i);

    return height;
}

double PdfJsonTable::getHeight(int row)
{
    //get row height
    // find the minimum height of objects as row-height
    //normally row with row-span = 1
    // spanned row has big height
    QJsonArray Row = tableArray[row].toArray();
    QJsonObject obj;
    double height, temp;
    height = 0;

    if(Row.count() == 0) return 0;


    for(int i=0; i < Row.count(); i++)
    {
        obj = Row[i].toObject();
        if(i == 0)
            height = obj["style"].toObject()["height"].toDouble();

        temp = obj["style"].toObject()["height"].toDouble();

        if(height > temp)
            height = temp;
    }

    return height;
}

double PdfJsonTable::getHeight(QJsonArray Row)
{
    QJsonObject obj;
    double height, temp;
    height = 0;

    if(Row.count() == 0) return 0;


    for(int i=0; i < Row.count(); i++)
    {
        obj = Row[i].toObject();
        if(i == 0)
            height = obj["style"].toObject()["height"].toDouble();

        temp = obj["style"].toObject()["height"].toDouble();

        if(height > temp)
            height = temp;
    }

    return height;
}

QJsonObject PdfJsonTable::updateObjectStyle(QJsonObject _object, QString _key, double _val)
{
    QJsonObject style;
    if(_object.contains("style"))
    {
        style = _object["style"].toObject();
        if( style.contains(_key) )
            style[_key] = _val;

        _object["style"] = style;
    }

    return _object;
}

bool PdfJsonTable::printCell(int row, int column, QJsonObject obj)
{
    // write from left
    QString type = obj.value("type").toString();
    QString value = obj.value("value").toString();
    QJsonObject style = obj.value("style").toObject();

    double width = style.value("width").toDouble();
    double height = style.value("height").toDouble();
    QString fontFamily = style.value("font-family").toString();
    int fontSize = style.value("font-size").toInt();
    bool bold = style.value("bold").toBool();
    QString align = style.value("align").toString();
    int border = style.value("border").toInt();
    int hPadding = style.value("h-padding").toInt();
    int rowSpan = style.value("row-span").toInt();
    QString _color = style.value("color").toString();
    QString _backgroundColor = style.value("background-color").toString();

    if(rowSpan == -1){painter->translate(width, 0); return true;}
    if(rowSpan > 1)
          height = getRowSpanHeight(row, column);

    QColor color, backgroundColor;
    if(_color.isEmpty())
        color = QColor(Qt::black);
    else
        color = QColor(_color);

    if(_backgroundColor.isEmpty())
        backgroundColor = QColor("transparent");
    else
        backgroundColor = QColor(_backgroundColor);

    QFont font(fontFamily);
    font.setPixelSize(fontSize);
    font.setBold(bold);

    height = (height == 0)? 50 : height;
    painter->setPen(QPen(QBrush(color), border));
    painter->setFont(font);
    QRect rect(0,0,width, height );
    painter->fillRect(rect, backgroundColor);
    if(border > 0)
        painter->drawRect(rect);

    if(type.compare("text", Qt::CaseInsensitive) == 0)
    {
        //text
        QRect innerRect(hPadding,0,width-2*hPadding, height );
        if(align.compare("center",Qt::CaseInsensitive) == 0)
            painter->drawText(innerRect, Qt::AlignVCenter | Qt::AlignHCenter |  Qt::TextWordWrap , value);
        else if(align.compare("right",Qt::CaseInsensitive) == 0)
            painter->drawText(innerRect, Qt::AlignVCenter | Qt::AlignRight | Qt::TextWordWrap, value);
        else
            painter->drawText(innerRect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWordWrap, value);

        painter->translate(width, 0);
    }
    else
    {
        //img
        QPixmap img(value);
        int imgWidth = img.width();
        int imgHeight = img.height();
        if(imgWidth > width)
        {
            img.scaledToWidth(width - 10);
            imgWidth = img.width();
        }
        if(imgHeight > height)
        {
            img.scaledToHeight(height - 10);
            imgHeight = img.height();
        }
        double y = height/2 - imgHeight/2;
        if(align.compare("center",Qt::CaseInsensitive) == 0)
        {
            double x = width/2 - imgWidth/2;
            painter->drawPixmap(x,y, img ) ;
        }
        else if(align.compare("right",Qt::CaseInsensitive) == 0)
        {
            double x = width - imgWidth;
            painter->drawPixmap(x,y, img ) ;
        }
        else
        {
            painter->drawPixmap(0,y, img ) ;
        }

        painter->translate(width, 0);
    }

    return true;

}

void PdfJsonTable::respanRow(int row, int column)
{
    QJsonArray Row;
    QJsonObject Obj;
    int rowSpan, lastRow, middleRow, newRowSpan;
    double height, newHeight;

    Row = tableArray[row].toArray();
    Obj = Row[column].toObject();
    rowSpan = Obj["style"].toObject()["row-span"].toInt();
    if(rowSpan <= 1) return;
    height = getRowSpanHeight(row, column);
    if( (currentHeight + height) <= (paperHeight - 30) )
        return;
    lastRow = row + rowSpan - 1;
    middleRow = lastRow;
    while(middleRow > row)
    {
        newHeight = getHeight(row, middleRow);
        if( (currentHeight + newHeight) <= (paperHeight - 30) )
            break;
        middleRow--;
    }

    // update object width new row-span
    // row column
    QJsonObject style = Obj["style"].toObject();
    newRowSpan = middleRow - row + 1;
    style["row-span"] = newRowSpan;
    //style["height"] = newHeight; height are the same in a row
    updateTableStyle(row, column, style);

    // update next row object row-span and height
    newRowSpan = lastRow - middleRow;
    //newHeight = getHeight(middleRow+1 , lastRow);
    Row = tableArray[middleRow+1].toArray();
    Obj = Row[column].toObject();
    style = Obj["style"].toObject();
    style["row-span"] = newRowSpan;
    //style["height"] = newHeight;

    updateTableStyle(middleRow+1 , column, style);
}

void PdfJsonTable::updateTableStyle(int row, int column, QJsonObject style)
{
    QJsonArray Row = tableArray[row].toArray();
    QJsonObject Obj = Row[column].toObject();
    Obj["style"] = style;
    Row.removeAt(column);
    Row.insert(column, Obj);
    tableArray.removeAt(row);
    tableArray.insert(row, Row);
}

double PdfJsonTable::getRowSpanHeight(int row, int column)
{
    double height = 0;
    QJsonArray array;
    QJsonObject obj;
    array = tableArray[row].toArray();
    obj = array[column].toObject();
    obj = obj["style"].toObject();
    int rowSpan = obj["row-span"].toInt();
    if(rowSpan == -1) return 0;
    height = obj["height"].toDouble();
    if(rowSpan == 1) return height;
    if(rowSpan > 1)
    {
        for(int i = row+1; i < tableArray.count(); i++)
        {
            array = tableArray[i].toArray();
            obj = array[column].toObject();
            obj = obj["style"].toObject();
            rowSpan = obj["row-span"].toInt();
            if(rowSpan == -1)
                height += obj["height"].toDouble();
            else
                break;
        }

        return height;
    }
    else return 0;


    return height;
}

QString PdfJsonTable::lastError()
{
    return error;
}
