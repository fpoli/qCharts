#include "indexhash.h"

IndexHash::IndexHash(const QModelIndex& index)
{
    hash = makeHash(index.row(), index.column());
}

IndexHash::IndexHash(int row, int column)
{
    hash = makeHash(row, column);
}

int IndexHash::makeHash(int x, int y)
{
    int diag = x + y;
    return diag*(diag-1) + x;
}

bool IndexHash::operator== (const IndexHash& other)
{
    return hash == other.hash;
}

IndexHash::operator int() const
{
    return hash;
}
