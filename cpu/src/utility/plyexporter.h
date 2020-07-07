#ifndef PLYEXPORTER_H
#define PLYEXPORTER_H

#include <QDir>
#include <QTextStream>
#include <iostream>
#include <particle.h>

/*
bool isBigEndian(void)
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1;
}
*/


class PlyExporter
{
public:
    PlyExporter(QString rootPath);
    ~PlyExporter(){};


    bool writePoints(int step, QList<Particle *> &particles, int diggerId, Body *digger, float borderRadius = 25);



private:
    QString m_filename;
    QString m_rootPath;
    QFile m_file;
    bool m_bigEndian;
};

#endif // PLYEXPORTER_H
