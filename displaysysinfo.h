#ifndef DISPLAYSYSINFO_H// _A(define)_의 조건이 안맞으면 구문안으로 안들어가고 endif로 갑니다.
#define DISPLAYSYSINFO_H

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class GetNode;

namespace Ui {
class DisplaySysInfo;
}

typedef struct plotdata {
    double xData[100];
    double yData[100];
    int index;//축?
    float Watt;
} plotdata;//struct 와 typestruct 의차이..

class DisplaySysInfo : public QWidget
{
    Q_OBJECT

public:
    explicit DisplaySysInfo(QWidget *parent = 0);
    ~DisplaySysInfo();

private:
    Ui::DisplaySysInfo *ui;

    GetNode *getNode;
    QwtPlotCurve *ARMSensorCurve;
    QwtPlotCurve *MEMSensorCurve;
    QwtPlotCurve *KFCSensorCurve;
    QwtPlotCurve *G3DSensorCurve;
    QwtPlot *qwtPlotSensor;

    plotdata armPlotData;//plotdata 구조체.
    plotdata memPlotData;
    plotdata kfcPlotData;
    plotdata g3dPlotData;

    QString a15Volt, a15Ampere, a15Watt;
    QString a7Volt, a7Ampere, a7Watt;
    QString gpuVolt, gpuAmpere, gpuWatt;
    QString memVolt, memAmpere, memWatt;

    void DisplaySensor(void);
    void float2string(void);
    void displaySensorPlot(void);
    void drawARMSensorCurve(void);
    void drawMEMSensorCurve(void);
    void drawKFCSensorCurve(void);
    void drawG3DSensorCurve(void);
    void displayCpuFrequency(void);

private slots:
    void update();
};

#endif // DISPLAYSYSINFO_H
