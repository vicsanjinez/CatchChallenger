#include "../base/BaseServer.h"
#include "../../general/base/GeneralVariable.h"
#include "../../general/base/CommonDatapack.h"
#include "../../general/base/CommonMap.h"
#include "../../general/fight/FightLoader.h"
#ifdef CATCHCHALLENGER_XLMPARSER_TINYXML1
#include "../../general/base/tinyXML/tinyxml.h"
#elif defined(CATCHCHALLENGER_XLMPARSER_TINYXML2)
#include "../../general/base/tinyXML2/tinyxml2.h"
#endif
#include "../base/GlobalServerData.h"
#include "../VariableServer.h"
#include "../base/DatabaseFunction.h"

#include <string>
#include <vector>

using namespace CatchChallenger;

void BaseServer::preload_monsters_drops()
{
    GlobalServerData::serverPrivateVariables.monsterDrops=DatapackGeneralLoader::loadMonsterDrop(GlobalServerData::serverSettings.datapack_basePath+DATAPACK_BASE_PATH_MONSTERS,CommonDatapack::commonDatapack.items.item,CommonDatapack::commonDatapack.monsters);

    std::cout << CommonDatapack::commonDatapack.monsters.size() << " monster drop(s) loaded" << std::endl;
}

#ifndef CATCHCHALLENGER_CLASS_ONLYGAMESERVER
void BaseServer::load_sql_monsters_max_id()
{
    #ifndef EPOLLCATCHCHALLENGERSERVER
    std::cout << GlobalServerData::serverPrivateVariables.cityStatusList.size() << " SQL city loaded" << std::endl;
    #endif

    //start to 0 due to pre incrementation before use
    GlobalServerData::serverPrivateVariables.maxMonsterId=1;
    std::string queryText;
    switch(GlobalServerData::serverPrivateVariables.db_common->databaseType())
    {
        default:
        #if defined(CATCHCHALLENGER_DB_MYSQL) && (not defined(EPOLLCATCHCHALLENGERSERVER))
        case DatabaseBase::DatabaseType::Mysql:
            queryText="SELECT `id` FROM `monster` ORDER BY `id` DESC LIMIT 0,1;";
        break;
        #endif
        #ifndef EPOLLCATCHCHALLENGERSERVER
        case DatabaseBase::DatabaseType::SQLite:
            queryText="SELECT id FROM monster ORDER BY id DESC LIMIT 0,1;";
        break;
        #endif
        #if not defined(EPOLLCATCHCHALLENGERSERVER) || defined(CATCHCHALLENGER_DB_POSTGRESQL)
        case DatabaseBase::DatabaseType::PostgreSQL:
            queryText="SELECT id FROM monster ORDER BY id DESC LIMIT 1;";
        break;
        #endif
    }
    if(GlobalServerData::serverPrivateVariables.db_common->asyncRead(queryText,this,&BaseServer::load_monsters_max_id_static)==NULL)
    {
        std::cerr << "Sql error for: " << queryText << ", error: " << GlobalServerData::serverPrivateVariables.db_common->errorMessage() << std::endl;
        abort();//stop because can't do the first db access
        load_clan_max_id();
    }
    return;
}

void BaseServer::load_monsters_max_id_static(void *object)
{
    static_cast<BaseServer *>(object)->load_monsters_max_id_return();
}

void BaseServer::load_monsters_max_id_return()
{
    while(GlobalServerData::serverPrivateVariables.db_common->next())
    {
        bool ok;
        const uint32_t &maxMonsterId=GlobalServerData::serverPrivateVariables.db_common->stringtouint32(GlobalServerData::serverPrivateVariables.db_common->value(0),&ok);
        if(!ok)
        {
            std::cerr << "Max monster id is failed to convert to number" << std::endl;
            continue;
        }
        else
            if(maxMonsterId>=GlobalServerData::serverPrivateVariables.maxMonsterId)
                GlobalServerData::serverPrivateVariables.maxMonsterId=maxMonsterId+1;
    }
    load_clan_max_id();
}
#endif
