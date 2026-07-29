#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractListModel>
#include <QDir>
#include <QFileInfoList>

class FileModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum RoleNames {
        FilePathRole = Qt::UserRole + 1,
        FileNameRole,
        FileTypeRole
    };

    explicit FileModel(QObject *parent = nullptr);

    void loadFiles(const QString &directory);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    struct FileData {
        QString filePath;
        QString fileName;
        QString fileType;
    };
    QVector<FileData> files;
};

#endif // FILEMODEL_H
