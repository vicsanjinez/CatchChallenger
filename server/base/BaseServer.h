#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QList>
#include <QByteArray>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDir>
#include <QSemaphore>
#include <QString>

#include "../../general/base/DebugClass.h"
#include "../../general/base/Map_loader.h"
#include "../../general/base/ProtocolParsing.h"
#include "../../general/base/QFakeServer.h"
#include "../../general/base/QFakeSocket.h"
#include "ServerStructures.h"
#include "Client.h"
#include "Bot/FakeBot.h"
#include "Map_server.h"

#ifndef POKECRAFT_BASESERVER_H
#define POKECRAFT_BASESERVER_H

namespace Pokecraft {
class BaseServer : public QObject
{
    Q_OBJECT
public:
    explicit BaseServer();
    virtual ~BaseServer();
    //stat function
    virtual bool isListen();
    virtual bool isStopped();
protected slots:
    virtual void start_internal_server() = 0;
    //init, constructor, destructor
    virtual void initAll();//call before all
    //remove all finished client
    virtual void removeOneClient();
    //new connection
    virtual void newConnection();
signals:
    void error(const QString &error);
    void try_initAll();
    void try_stop_server();
    void need_be_started();
    //stat
    void is_started(bool);
protected:
    virtual void parseJustLoadedMap(const Map_to_send &,const QString &);
    virtual void connect_the_last_client();
    //starting function
    virtual void stop_internal_server();
    virtual void check_if_now_stopped();
    virtual QString sqlitePath();
    //player related
    virtual ClientMapManagement * getClientMapManagement();

    //stat
    enum ServerStat
    {
        Down=0,
        InUp=1,
        Up=2,
        InDown=3
    };
    ServerStat stat;

    //to load/unload the content
    struct Map_semi
    {
        //conversion x,y to position: x+y*width
        Map* map;
        Map_semi_border border;
        Map_to_send old_map;
    };
    virtual void preload_the_data();
    virtual void preload_the_map();
    virtual void preload_the_skin();
    virtual void preload_the_visibility_algorithm();
    virtual void unload_the_data();
    virtual void unload_the_map();
    virtual void unload_the_skin();
    virtual void unload_the_visibility_algorithm();

    virtual bool initialize_the_database();
    //FakeServer server;//wrong, create another object, here need use the global static object

    //to keep client list
    QList<Client *> client_list;
};
}

#endif