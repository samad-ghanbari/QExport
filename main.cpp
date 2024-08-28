#include <QApplication>
#include "lib/templates/tableTemplate.h"
#include "lib/exporter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    TableTemplate tableTemplate(4);
    // page title
    tableTemplate.appendTitle({"img"}, {":/assets/images/tct.png"}, "Header-1", {"#005"}, {"center"});
    tableTemplate.appendTitle({"text"}, {"DaNet DSLAM PLAN"}, "Header-2", {"#005"}, {"center"});
    tableTemplate.appendTitle({"img", "text","img"}, {":/assets/images/danet.png", "DaNet Report", ":/assets/images/danet.png"}, "Header-3",  {"", "#005", ""}, {"center", "center", "center"});
    tableTemplate.appendTitle({"text", "text"}, {"2-BA Saloon Data", "1403/05/29"}, "Header-4", {"#050", "#500"}, {"left", "right"});

    //table
    tableTemplate.setTableHeader({"Menu-1", "Menu-2", "Menu-3", "Menu-4"});

    tableTemplate.appentRow("Interfaces", {"Lorem-1 ipsum odor amet, consectetuer adipiscing elit. Blandit eget vulputate cubilia convallis penatibus vivamus ante ante? Odio felis libero auctor elit, parturient donec porta tristique nullam. Scelerisque penatibus maximus erat aptent egestas mus. Eu sed euismod, hac semper arcu tortor ullamcorper vestibulum.", "Lorem-1", "Ipsum-1", "L-I-1"});
    tableTemplate.appentRow("Interfaces", {"Lorem-2", "Data", "CX600X16", "1G 1/2/3"});
    tableTemplate.appentRow();
    tableTemplate.appentRow("Interfaces", {"Lorem-2-2", "Data", "CX600X16", "1G 1/2/3"});


    tableTemplate.appentRow();

    tableTemplate.appentRow("Interfaces", {"Lorem-3 ipsum", "PCM", "CX600X16", "Lorem ipsum odor amet, consectetuer adipiscing elit. Non arcu scelerisque nascetur elementum ante iaculis sapien. Facilisi faucibus dolor arcu ante consequat accumsan facilisi."});

    tableTemplate.appentRow("Interfaces", {"Lorem-5 ", "Data", "CX600X16", "1G 1/1/1"});

//    tableTemplate.appentRow();

    tableTemplate.appendRow({"text", "img", "text", "text"}, {"samad",":/assets/images/tct.png", "test2", "test3"}, "650K NET", {0,0,0,0}, {0,0,0,0},{"#F00", "#F55", "#F33", "#F80"},{"center", "center", "center", "right"});

    tableTemplate.appentRow( "Interfaces", {"Lorem-6", "PCM", "CX600X16", "10G 3/0/0"});
    tableTemplate.appentRow("Interfaces", {"Lorem-7", "Switch", "Lorem ipsum odor amet, consectetuer adipiscing elit.Lorem ipsum odor amet, consectetuer adipiscing elit.Lorem ipsum odor amet, consectetuer adipiscing elit.", "1G 10/0/0"});
    tableTemplate.appentRow("Interfaces", {"Lorem-8", "Switch", "CX600X16", "10G 11/0/0"});
    tableTemplate.appentRow("Interfaces", {"Lorem-9", "PCM", "CX600X16", "10G 3/0/0"});
    tableTemplate.appentRow("Interfaces", {"Lorem-10", "Switch", "CX600X16", "1G 10/0/0"});

   tableTemplate.appentRow();

    tableTemplate.appentRow("Interfaces", {"Lorem-11", "PCM", "CX600X16", "10G 3/0/0"});
    tableTemplate.appentRow("Interfaces", {"Lorem-12", "Switch", "CX600X16", "1G 10/0/0"});
    tableTemplate.appentRow("Interfaces", {"Lorem-13", "Switch", "CX600X16", "10G 11/0/0"});

    tableTemplate.appentRow("Metro Plan", {"Loreeeeeem-14", "PCM", "CX600X16", "10G 3/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-15", "Switch", "CX600X16", "1G 10/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-16", "Switch", "CX600X16 NetEngine Huawei", "10G 11/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-17", "Switch", "CX600X16", "10G 11/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-18", "Switch", "CX600X16", "10G 11/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-19 ipsum odor amet, consectetuer adipiscing elit. Blandit eget vulputate cubilia convallis penatibus vivamus ante ante? Odio felis libero auctor elit, parturient donec porta tristique nullam. Scelerisque penatibus maximus erat aptent egestas mus. Eu sed euismod, hac semper arcu tortor ullamcorper vestibulum.", "Lorem-1", "Ipsum-1", "L-I-1"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-20 ipsum", "PCM", "CX600X16", "Lorem ipsum odor amet, consectetuer adipiscing elit. Non arcu scelerisque nascetur elementum ante iaculis sapien. Facilisi faucibus dolor arcu ante consequat accumsan facilisi."});
    tableTemplate.appentRow("Metro Plan", {"lorem-21", "Switch", "CX600X16", "1G 10/0/0"});
    tableTemplate.appentRow("Metro Plan",  {"Lorem-22", "PCM", "CX600X16", "10G 3/0/0"});
//    tableTemplate.appentRow();
    tableTemplate.appentRow("Interconnection", {"Lorem-23", "Switch", "Lorem ipsum odor amet, consectetuer adipiscing elit.Lorem ipsum odor amet, consectetuer adipiscing elit.Lorem ipsum odor amet, consectetuer adipiscing elit.", "1G 10/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-24", "Switch", "CX600X16", "10G 11/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-25", "PCM", "CX600X16", "10G 3/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-26", "Switch", "CX600X16", "1G 10/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-27", "PCM", "CX600X16", "10G 3/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-28", "Switch", "CX600X16", "1G 10/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-29", "Switch", "CX600X16", "10G 11/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-30", "PCM", "CX600X16", "10G 3/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-31", "Switch", "CX600X16", "1G 10/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-32", "Switch", "CX600X16 NetEngine Huawei", "10G 11/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-33", "Switch", "CX600X16", "10G 11/0/0"});
    tableTemplate.appentRow("Metro Plan", {"Lorem-34", "Switch", "CX600X16", "10G 11/0/0"});
    tableTemplate.appendRow({"text", "img", "text", "text"}, {"samad",":/assets/images/kuber.png", "Kuber", "test3"}, "Metro Plan-2",{0,0,0,0}, {0,0,0,0},{"#F00", "#F55", "#F33", "#F80"},{"center", "center", "center", "right"});

    tableTemplate.highlight(5);

    Exporter fileExporter(&tableTemplate);
    fileExporter.pdf("my-pdf.pdf",2, true,"Ghanbari","DaNet Report","A3");
    fileExporter.excel("my-excel.xlsx",true);


    return 1;
}
