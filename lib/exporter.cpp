#include "exporter.h"
#include "lib/templates/tableTemplate.h"
#include "lib/pdf/pdfjsontable.h"
#include "lib/excel/exceljsontable.h"

Exporter::Exporter(TableTemplate *tables, QObject *parent)
    : QObject{parent}, title({}), table({}),tableTemplate(tables)
{
    width = 0;
}

void Exporter::pdf(QString outputPath, int rowSpanColIndex, bool justifyWidth, QString Creator, QString DocName, QString pageSize, QString orientation, qreal pageMarginLeft, qreal pageMarginTop, qreal pageMarginRight, qreal pageMarginBottom)
{
    PdfJsonTable pdf(outputPath, Creator,DocName, pageSize, orientation, pageMarginLeft, pageMarginTop, pageMarginRight, pageMarginBottom);
    width = pdf.getViewPortWidth();

    prepareTables(rowSpanColIndex, justifyWidth);

    pdf.setPageHeader(&title);
    pdf.setTable(&table, {0});
    pdf.print();
}

void Exporter::excel(QString outputPath, bool justifyWidth, QList<int> repeatedRows, bool skipImages)
{
    prepareTables(0, justifyWidth);

    ExcelJsonTable ejs;
    ejs.setTables(title,table);
    ejs.exportExcel(outputPath,repeatedRows,skipImages);
}

void Exporter::prepareTables(int rowSpanColIndex, bool justifyWidth)
{
    if(width == 0)
        width = 1500;

    if(title.isEmpty())
        title = tableTemplate->getTitle(width,rowSpanColIndex, justifyWidth);

    if(table.isEmpty())
        table = tableTemplate->getTable(width,rowSpanColIndex, justifyWidth);
}
