#include "plyexporter.h"

PlyExporter::PlyExporter(QString rootPath) :
    m_rootPath(rootPath)
{
    //m_bigEndian = isBigEndian();

    // make target directory if not existing
    if (!QDir(m_rootPath).exists()){
        QDir().mkpath(m_rootPath);
    }

}


bool PlyExporter::writePoints(int step, QList<Particle *> particles, int diggerId, Body *digger){
    m_filename = QDir(m_rootPath).filePath(QString("sand-%08d.ply").arg(step, 5, 10, QChar('0')));
    QFile m_file(m_filename);
    if (!m_file.open(QIODevice::ReadWrite)){
        std::cout << "Error opening file: " << m_filename.toStdString() << std::endl;
        return false;
    }
    QTextStream stream(&m_file);

    // writer header
    stream << "ply" << Qt::endl;

    // TODO: Make writing to binary possibly!
    /*
    if (m_bigEndian){
        stream << "format binary_big_endian 1.0" << Qt::endl;
    } else {
        stream << "format binary_little_endian 1.0" << Qt::endl;
    }
    */

    stream << "format ascii 1.0" << Qt::endl;

    stream << "comment Time " << step << Qt::endl;

    stream << "element digger " << 1 << Qt::endl;
    stream << "property float x" << Qt::endl;
    stream << "property float y" << Qt::endl;
    stream << "property float angle" << Qt::endl;

    stream << "element vertex " << particles.size()  << Qt::endl;
    stream << "property float x" << Qt::endl;
    stream << "property float y" << Qt::endl;
    stream << "property float xv" << Qt::endl;
    stream << "property float yv" << Qt::endl;

    stream << "end_header" << Qt::endl;

    stream << digger->ccenter.x << " " << digger->ccenter.y << " " << digger->angle << Qt::endl;

    Particle *part;
    for (int i = 0; i < particles.size(); i++){
        // case particle belongs to digger
        part = particles.at(i);
        if (part->bod == diggerId){
            continue;
        }
        stream << part->p.x << " " << part->p.y << " " << part->v.x << " " << part->v.y << Qt::endl;
    }
    m_file.close();

    return true;
}


QFile m_file;
QTextStream m_stream;
bool m_bigEndian;
