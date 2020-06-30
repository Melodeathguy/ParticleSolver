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


bool PlyExporter::writePoints(int step, QList<Particle *> &particles, int diggerId, Body *digger, float borderRadius){
    m_filename = QDir(m_rootPath).filePath(QString("sand-%08d.ply").arg(step, 5, 10, QChar('0')));
    QFile m_file(m_filename);
    if (!m_file.open(QIODevice::ReadWrite)){
        std::cout << "Error opening file: " << m_filename.toStdString() << std::endl;
        return false;
    }
    QTextStream stream(&m_file);

    // writer header
    stream << "ply" << endl;

    // TODO: Make writing to binary possibly!
    /*
    if (m_bigEndian){
        stream << "format binary_big_endian 1.0" << Qt::endl;
    } else {
        stream << "format binary_little_endian 1.0" << Qt::endl;
    }
    */

    stream << "format ascii 1.0" << endl;

    stream << "comment Time " << step << endl;

    stream << "element digger " << 1 << endl;
    stream << "property float x" << endl;
    stream << "property float y" << endl;
    stream << "property float xv" << endl;
    stream << "property float yv" << endl;
    stream << "property float a" << endl;
    stream << "property float am" << endl;

    stream << "element vertex " << particles.size() - digger->particles.size() << endl;
    stream << "property float x" << endl;
    stream << "property float y" << endl;
    stream << "property float z" << endl; // currently 3rd dimension not in use, set to constant
    stream << "property float xv" << endl;
    stream << "property float yv" << endl;
    stream << "property float zv" << endl; // currently 3rd dimension not in use, set to constant

    stream << "end_header" << endl;

    float dx, dy, dvx, dvy, da, dam;
    dx = digger->ccenter.x / borderRadius;
    dy = digger->ccenter.y / borderRadius;
    dvx = digger->getVelocityHint().x / borderRadius;
    dvy = digger->getVelocityHint().y / borderRadius;
    da = digger->getAngle(&particles);
    dam = digger->getAngleImpulseHint();

    stream << dx << " " << dy << " " << dvx << " " << dvy << " " << da << " " << dam <<endl;
    //std::cout << dx << " " << dy << " " << dvx << " " << dvy << " " << da << " " << dam <<endl;

    Particle *part;
    float x, y, vx, vy;
    for (int i = 0; i < particles.size(); i++){
        // case particle belongs to digger
        part = particles.at(i);
        if (part->bod == diggerId){
            continue;
        }
        x = part->p.x / borderRadius;
        y = part->p.y / borderRadius;
        vx = part->v.x / borderRadius;
        vy = part->v.y / borderRadius;

        stream << x << " " << y << " " << 0 << " " << vx << " " << vy << " " << 0 << " " << endl;
    }
    m_file.close();

    return true;
}

