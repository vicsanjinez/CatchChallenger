#include "ClientNetworkReadWithoutSender.h"
#include "GlobalServerData.h"
#include "Client.h"

using namespace CatchChallenger;

ClientNetworkReadWithoutSender ClientNetworkReadWithoutSender::clientNetworkReadWithoutSender;

#ifdef CATCHCHALLENGER_DDOS_FILTER
void ClientNetworkReadWithoutSender::doDDOSAction()
{
    if(CATCHCHALLENGER_SERVER_DDOS_MAX_VALUE>0)
    {
        const int &size=Client::clientBroadCastList.size();
        int index=0;
        while(index<size)
        {
            Client::clientBroadCastList.at(index)->doDDOSCompute();
            index++;
        }
    }
}
#endif
