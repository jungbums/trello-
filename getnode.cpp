#include "getnode.h"
#include <QFile>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <QDebug>

GetNode::GetNode()
{
    usage[8] = {0,};

    for (int i = 0; i < 8; i++) {
        QString temp;
        temp.sprintf("/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_cur_freq", i);//내용을temp에저장.
        cpu_node_list[i] = temp;    //Qstring
    }
}

QString GetNode::GetGPUCurFreq()//Gpu를읽어서buf에저장반환.
{
    FILE *fp = NULL;//파일포인터.
    char buf[4] = {'\0',};
    fp = fopen(GPUFREQ_NODE, "r");//읽기전용, 성공시주소파일주소가들어감,실패Null반환;

    if (fp == NULL) {
        return 0;
    }

    fread(buf, 1, 3, fp);//buf에저장.fp로읽음.

    fclose(fp);

    return buf;
}

QString GetNode::GetCPUCurFreq(int cpuNum)
{
    FILE *fp = NULL;
    char buf[8] = {'\0',};
    int v;
    fp = fopen(cpu_node_list[cpuNum].toUtf8(), "r");

    if (fp == NULL) {
        return 0;
    }

    fread(buf, 1, 8, fp);
    fclose(fp);

    v = atoi(buf) / 1000;//atoi 정수.
    sprintf(buf, "%d", v);

    return buf;
}

QString GetNode::GetCPUTemp(int cpuNum)
{
    FILE *fp = NULL;

    fp = fopen(TEMP_NODE, "r");

    char buf[16];

    if (fp == NULL) {
        return NULL;
    }

    for (int i = 0; i < cpuNum + 1; i++)//for문을쓰는이유??
        fread(buf, 1, 16, fp);

    fclose(fp);

    buf[12] = '\0';

    return &buf[9];
}


int GetNode::open_sensor(const char *node, sensor_t *sensor)
{
    if ((sensor->fd = open(node, O_RDWR)) < 0)//읽고쓰기모두가능:O_RDWR
        qDebug() << node << "Open Fail"; //open 실패시.-1반환.

    return sensor->fd;
}

int GetNode::OpenINA231()
{
    if (open_sensor(DEV_SENSOR_ARM, &sensor[SENSOR_ARM]) < 0)//0 안열리면, -1반환.
        return -1;
    if (open_sensor(DEV_SENSOR_MEM, &sensor[SENSOR_MEM]) < 0)//1
        return -1;
    if (open_sensor(DEV_SENSOR_KFC, &sensor[SENSOR_KFC]) < 0)//2
        return -1;
    if (open_sensor(DEV_SENSOR_G3D, &sensor[SENSOR_G3D]) < 0)//3
        return -1;

    if (read_sensor_status(&sensor[SENSOR_ARM]))
        return -1;
    if (read_sensor_status(&sensor[SENSOR_MEM]))
        return -1;
    if (read_sensor_status(&sensor[SENSOR_KFC]))
        return -1;
    if (read_sensor_status(&sensor[SENSOR_G3D]))
        return -1;

    if (!sensor[SENSOR_ARM].data.enable)
        enable_sensor(&sensor[SENSOR_ARM], 1);
    if (!sensor[SENSOR_MEM].data.enable)
        enable_sensor(&sensor[SENSOR_MEM], 1);
    if (!sensor[SENSOR_KFC].data.enable)
        enable_sensor(&sensor[SENSOR_KFC], 1);
    if (!sensor[SENSOR_G3D].data.enable)
        enable_sensor(&sensor[SENSOR_G3D], 1);

    return 0;
}

void GetNode::CloseINA231()
{
    if (sensor[SENSOR_ARM].data.enable)
        enable_sensor(&sensor[SENSOR_ARM], 0);
    if (sensor[SENSOR_MEM].data.enable)
        enable_sensor(&sensor[SENSOR_MEM], 0);
    if (sensor[SENSOR_KFC].data.enable)
        enable_sensor(&sensor[SENSOR_KFC], 0);
    if (sensor[SENSOR_G3D].data.enable)
        enable_sensor(&sensor[SENSOR_G3D], 0);

    close_sensor(&sensor[SENSOR_ARM]);
    close_sensor(&sensor[SENSOR_MEM]);
    close_sensor(&sensor[SENSOR_KFC]);
    close_sensor(&sensor[SENSOR_G3D]);
}

void GetNode::GetINA231()
{
    read_sensor(&sensor[SENSOR_ARM]);
    read_sensor(&sensor[SENSOR_MEM]);
    read_sensor(&sensor[SENSOR_KFC]);
    read_sensor(&sensor[SENSOR_G3D]);//디버깅?

    armuV = (float)(sensor[SENSOR_ARM].data.cur_uV / 100000) / 10;
    armuA = (float)(sensor[SENSOR_ARM].data.cur_uA / 1000) / 1000;
    armuW = (float)(sensor[SENSOR_ARM].data.cur_uW / 1000) / 1000;

    memuV = (float)(sensor[SENSOR_MEM].data.cur_uV / 100000) / 10;
    memuA = (float)(sensor[SENSOR_MEM].data.cur_uA / 1000) / 1000;
    memuW = (float)(sensor[SENSOR_MEM].data.cur_uW / 1000) / 1000;

    kfcuV = (float)(sensor[SENSOR_KFC].data.cur_uV / 100000) / 10;
    kfcuA = (float)(sensor[SENSOR_KFC].data.cur_uA / 1000) / 1000;
    kfcuW = (float)(sensor[SENSOR_KFC].data.cur_uW / 1000) / 1000;

    g3duV = (float)(sensor[SENSOR_G3D].data.cur_uV / 100000) / 10;
    g3duA = (float)(sensor[SENSOR_G3D].data.cur_uA / 1000) / 1000;
    g3duW = (float)(sensor[SENSOR_G3D].data.cur_uW / 1000) / 1000;

}

void GetNode::enable_sensor(sensor_t *sensor, unsigned char enable)
{
    if (sensor->fd > 0)
    {
        sensor->data.enable = enable ? 1 : 0;
        if (ioctl(sensor->fd, INA231_IOCSSTATUS, &sensor->data) < 0)//ioctl(a,b,c);a=입출력지정자;b=request
            qDebug() << "IOCTL Error";
    }
}

int GetNode::read_sensor_status(sensor_t *sensor)
{
    if (sensor->fd > 0) {
        if (ioctl(sensor->fd, INA231_IOCGSTATUS, &sensor->data) < 0)
            qDebug() << sensor->data.name << "IOCTL Error";
    }
    return 0;
}

void GetNode::read_sensor(sensor_t *sensor)
{
    if (sensor->fd > 0) {
        if (ioctl(sensor->fd, INA231_IOCGREG, &sensor->data) < 0)
            qDebug() << sensor->data.name << "IOCTL Error!";
    }
}

void GetNode::close_sensor(sensor_t *sensor)
{
    if (sensor->fd > 0)
        close(sensor->fd);
}

int GetNode::calUsage(int cpu_idx, int user, int nice, int system, int idle)
{
    long total = 0;
    long usage = 0;
    int diff_user, diff_system, diff_idle;

    diff_user = mOldUserCPU[cpu_idx] - user;
    diff_system = mOldSystemCPU[cpu_idx] - system;
    diff_idle = mOldIdleCPU[cpu_idx] - idle;

    total = diff_user + diff_system + diff_idle;
    if (total != 0)
        usage = diff_user * 100 / total;

    mOldUserCPU[cpu_idx] =user;
    mOldSystemCPU[cpu_idx] = system;
    mOldIdleCPU[cpu_idx] = idle;

    return usage;
}

int GetNode::GetCPUUsage(void)
{
    char buf[80] = {0,};
    char cpuid[8] = "cpu";
    int findCPU = 0;
    int user, system, nice, idle;
    FILE *fp;
    int cpu_index = 0;

    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
        return 0;

    while (fgets(buf, 80, fp)) {
        char temp[4] = "cpu";
        temp[3] = '0' + cpu_index;
        if (!strncmp(buf, temp, 4)) {
            findCPU = 1;
            sscanf(buf, "%s %d %d %d %d",
                   cpuid, &user, &nice, &system, &idle);
            usage[cpu_index] = calUsage(cpu_index, user, nice, system, idle);
            cpu_index++;
        }
        if (!strncmp(buf, "intr", 4))//strncmo0 = 결과 값이면 s1 = s2 ,0 < 결과 값이면 s1 > s2 ,0 > 결과 값이면 s1 < s2
            break;//buf와 intr을 크기4만큼 비교
        if (findCPU == 0)
            mOldUserCPU[cpu_index] = mOldSystemCPU[cpu_index] = mOldIdleCPU[cpu_index] = 0;
        else
            findCPU = 0;
    }

    fclose(fp);

    return 0;
}
