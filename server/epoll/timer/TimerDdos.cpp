#include "TimerDdos.h"
#include "../Epoll.h"

#include "../../base/LocalClientHandlerWithoutSender.h"
#include "../../base/BroadCastWithoutSender.h"
#include "../../base/ClientNetworkReadWithoutSender.h"
#include "../../base/GlobalServerData.h"
#include "../../VariableServer.h"
#include "../../../general/base/GeneralVariable.h"

#include <iostream>

TimerDdos::TimerDdos()
{
}

void TimerDdos::exec()
{
    #ifdef CATCHCHALLENGER_EXTRA_CHECK
    if(CATCHCHALLENGER_SERVER_DDOS_MAX_VALUE<=0)
    {
        std::cerr << "CATCHCHALLENGER_SERVER_DDOS_MAX_VALUE out of range:" << CATCHCHALLENGER_SERVER_DDOS_MAX_VALUE << std::endl;
        return;
    }
    #endif
    CatchChallenger::LocalClientHandlerWithoutSender::localClientHandlerWithoutSender.doDDOSChat();
    CatchChallenger::BroadCastWithoutSender::broadCastWithoutSender.doDDOSChat();
    #ifdef CATCHCHALLENGER_DDOS_FILTER
    CatchChallenger::ClientNetworkReadWithoutSender::clientNetworkReadWithoutSender.doDDOSAction();
    #endif
}

