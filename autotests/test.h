#ifndef TEST_H
#define TEST_H

#include <QPalette>
#include <QStyle>
#include <QtTest/QtTest>

class MyStyle : public QStyle
{
public:
    void drawPrimitive(PrimitiveElement, const QStyleOption*, QPainter*, const QWidget*) const {}
    void drawControl(ControlElement, const QStyleOption*, QPainter*, const QWidget*) const {}
    QRect subElementRect(SubElement, const QStyleOption*, const QWidget*) const { return QRect(); }
    void drawComplexControl(ComplexControl, const QStyleOptionComplex*, QPainter*, const QWidget*) const {}
    SubControl hitTestComplexControl(ComplexControl, const QStyleOptionComplex*, const QPoint&, const QWidget*) const { return SubControl(); }
    QRect subControlRect(ComplexControl, const QStyleOptionComplex*, SubControl, const QWidget*) const { return QRect(); }
    int pixelMetric(PixelMetric, const QStyleOption*, const QWidget*) const { return 0; }
    QSize sizeFromContents(ContentsType, const QStyleOption*, const QSize&, const QWidget*) const { return QSize(); }
    int styleHint(StyleHint, const QStyleOption*, const QWidget*, QStyleHintReturn*) const { return 0; }
    QPixmap standardPixmap(StandardPixmap, const QStyleOption*, const QWidget*) const { return QPixmap(); }
    QPixmap generatedIconPixmap(QIcon::Mode, const QPixmap&, const QStyleOption*) const { return QPixmap(); }
};
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
#define QTEST_GUILESS_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    MyStyle style; \
    QPalette palette; \
    QApplication::setPalette(palette, "nongui"); \
    QApplication::setStyle(&style); \
    QApplication app(argc, argv, QApplication::Tty); \
    TestObject tc; \
    return QTest::qExec(&tc, argc, argv); \
}
#endif

#endif // TEST_H
