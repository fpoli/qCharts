#ifndef INDEXHASH_H
#define INDEXHASH_H

#include <QtCore>

class IndexHash {
public:
    IndexHash(const QModelIndex& index);
    IndexHash(int row, int column);
    bool operator== (const IndexHash& other);
    operator int() const;
private:
    int makeHash(int x, int y);
    int hash;
};

#endif // INDEXHASH_H
