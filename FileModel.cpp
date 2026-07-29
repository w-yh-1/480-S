#include "FileModel.h"

FileModel::FileModel(QObject *parent)
    : QAbstractListModel(parent) {}

void FileModel::loadFiles(const QString &directory) {
    beginResetModel();
    QDir dir(directory);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    files.clear();
    for (const QFileInfo &fileInfo : fileInfoList) {
        QString type = fileInfo.suffix().toLower();
        if (type == "jpg" || type == "png") {
            files.append({fileInfo.filePath(), fileInfo.fileName(), "image"});
        } else if (type == "mp4" || type == "avi") {
            files.append({fileInfo.filePath(), fileInfo.fileName(), "video"});
        }
    }
    endResetModel();
}

int FileModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return files.size();
}

QVariant FileModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= files.size())
        return QVariant();

    const auto &file = files[index.row()];
    if (role == FilePathRole) return file.filePath;
    if (role == FileNameRole) return file.fileName;
    if (role == FileTypeRole) return file.fileType;
    return QVariant();
}

QHash<int, QByteArray> FileModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[FilePathRole] = "filePath";
    roles[FileNameRole] = "fileName";
    roles[FileTypeRole] = "fileType";
    return roles;
}
