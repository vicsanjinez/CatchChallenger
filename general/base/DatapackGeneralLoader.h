#ifndef DATAPACKGENERALLOADER_H
#define DATAPACKGENERALLOADER_H

#include <QHash>
#include <QString>
#include "GeneralStructures.h"

namespace CatchChallenger {
class DatapackGeneralLoader
{
public:
    static QHash<QString, Reputation> loadReputation(const QString &file);
};
}

#endif // DATAPACKGENERALLOADER_H