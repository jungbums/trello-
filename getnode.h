#ifndef GETNODE_H
#define GETNODE_H

#include <QWidget>
#include <QFile>        // provides an interface for reading from and writing to files
#include <sys/ioctl.h>

#define GPUFREQ_NODE    "/sys/devices/11800000.mali/clock"
#define TEMP_NODE       "/sys/devices/10060000.tmu/temp"

typedef struct ina231_iocreg__t {
    char name[20];
    unsigned int enable;
    unsigned int cur_uV;
    unsigned int cur_uA;
    unsigned int cur_uW;
} ina231_iocreg_t;

typedef struct sensor__t {
    int  fd;
    ina231_iocreg_t data;
} sensor_t;

#define INA231_IOCGREG      _IOR('i', 1, ina231_iocreg_t *)
#define INA231_IOCSSTATUS   _IOW('i', 2, ina231_iocreg_t *)
#define INA231_IOCGSTATUS   _IOR('i', 3, ina231_iocreg_t *)

#define DEV_SENSOR_ARM  "/dev/sensor_arm"
#define DEV_SENSOR_MEM  "/dev/sensor_mem"
#define DEV_SENSOR_KFC  "/dev/sensor_kfc"
#define DEV_SENSOR_G3D  "/dev/sensor_g3d"

enum    {                  //enum (0,1,2,3...)
    SENSOR_ARM = 0,
    SENSOR_MEM,
    SENSOR_KFC,
    SENSOR_G3D,
    SENSOR_MAX
};


class GetNode
{
public:
    GetNode();//생성자.
    QString cpu_node_list[8];
    float armuV,armuA, armuW;
    float g3duV, g3duA, g3duW;
    float kfcuV, kfcuA, kfcuW;
    float memuV, memuA, memuW;
    int usage[8];

    QString GetGPUCurFreq(void);
    QString GetCPUCurFreq(int cpuNum);
    QString GetCPUTemp(int cpuNum);
    int GetCPUUsage(void);
    int calUsage(int cpu_idx, int user, int nice, int system, int idle);

    int open_sensor(const char *node, sensor_t *sensor);
    void close_sensor(sensor_t *sensor);
    void enable_sensor(sensor_t *sensor, unsigned char enable);
    int read_sensor_status(sensor_t *sensor);
    void read_sensor(sensor_t *sensor);

    int OpenINA231(void);
    void CloseINA231(void);
    void GetINA231(void);

private:
    sensor_t sensor[SENSOR_MAX];//4
    int mOldUserCPU[8];
    int mOldSystemCPU[8];
    int mOldIdleCPU[8];

};

#endif // GETNODE_H
