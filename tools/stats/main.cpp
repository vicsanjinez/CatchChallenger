#include <iostream>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <ctime>

#include "../../general/base/FacilityLibGeneral.h"
#include "../../general/base/cpp11addition.h"
#include "../../server/base/TinyXMLSettings.h"
#include "../../server/epoll/Epoll.h"
#include "../../server/epoll/EpollSocket.h"
#include "../../server/VariableServer.h"
#include "LinkToLogin.h"

#define MAXEVENTS 512

using namespace CatchChallenger;

std::string applicationDirPath;

void generateTokenStatClient(TinyXMLSettings &settings)
{
    FILE *fpRandomFile = fopen(RANDOMFILEDEVICE,"rb");
    if(fpRandomFile==NULL)
    {
        std::cerr << "Unable to open " << RANDOMFILEDEVICE << " to generate random token" << std::endl;
        abort();
    }
    const int &returnedSize=fread(LinkToLogin::private_token_statclient,1,TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT,fpRandomFile);
    if(returnedSize!=TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT)
    {
        std::cerr << "Unable to read the " << TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT << " needed to do the token from " << RANDOMFILEDEVICE << std::endl;
        abort();
    }
    settings.setValue("token",binarytoHexa(reinterpret_cast<char *>(LinkToLogin::private_token_statclient)
                                           ,TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT).c_str());
    fclose(fpRandomFile);
    settings.sync();
}

int main(int argc, char *argv[])
{
    if(argc<1)
    {
        std::cerr << "argc<1: wrong arg count" << std::endl;
        return EXIT_FAILURE;
    }
    applicationDirPath=argv[0];

    srand(time(NULL));
    ProtocolParsing::initialiseTheVariable();

    LinkToLogin::linkToLogin=NULL;
    std::string outputFile;
    {
        TinyXMLSettings settings(FacilityLibGeneral::getFolderFromFile(applicationDirPath)+"/stats-client.xml");

        if(!settings.contains("host"))
            settings.setValue("host","localhost");
        const std::string host=settings.value("host");
        if(!settings.contains("port"))
            settings.setValue("port",rand()%40000+10000);
        const uint16_t port=stringtouint16(settings.value("port"));

        if(!settings.contains("tryInterval"))
            settings.setValue("tryInterval","30");
        const uint8_t tryInterval=stringtouint8(settings.value("tryInterval"));
        if(!settings.contains("considerDownAfterNumberOfTry"))
            settings.setValue("considerDownAfterNumberOfTry","5");
        const uint8_t considerDownAfterNumberOfTry=stringtouint8(settings.value("considerDownAfterNumberOfTry"));

        if(!settings.contains("outputFile"))
            settings.setValue("outputFile","gameserver.json");

        if(!settings.contains("token"))
            generateTokenStatClient(settings);
        std::string token=settings.value("token");
        if(token.size()!=TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT*2/*String Hexa, not binary*/)
            generateTokenStatClient(settings);
        token=settings.value("token");

        settings.sync();

        outputFile=settings.value("outputFile");
        LinkToLogin::linkToLogin->pFilePath=outputFile;

        const std::vector<char> &tokenBinary=hexatoBinary(token);
        if(tokenBinary.empty())
        {
            std::cerr << "convertion to binary for pass failed for: " << token << std::endl;
            abort();
        }
        memcpy(LinkToLogin::private_token_statclient,tokenBinary.data(),TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT);

        if(!Epoll::epoll.init())
            return EPOLLERR;

        const int &linkfd=LinkToLogin::tryConnect(
                    host.c_str(),
                    port,
                    tryInterval,
                    considerDownAfterNumberOfTry
                    );
        if(linkfd<0)
        {
            std::cerr << "Unable to connect on login" << std::endl;
            abort();
        }

        #ifdef SERVERSSL
        ctx from what?
        LoginLinkToLogin::loginLinkToLogin=new LoginLinkToLogin(linkfd,ctx);
        #else
        LinkToLogin::linkToLogin=new LinkToLogin(linkfd);
        #endif

        LinkToLogin::linkToLogin->stat=LinkToLogin::Stat::Connected;
        LinkToLogin::linkToLogin->readTheFirstSslHeader();
        LinkToLogin::linkToLogin->setConnexionSettings(tryInterval,considerDownAfterNumberOfTry);
    }

    {
        if(LinkToLogin::linkToLogin->pFilePath.empty())
        {
            std::cerr << "The output file can't be emtpy" << std::endl;
            abort();
        }
        LinkToLogin::linkToLogin->pFile = fopen(LinkToLogin::linkToLogin->pFilePath.c_str(),"wb");
        if(LinkToLogin::linkToLogin->pFile==NULL)
        {
            std::cerr << "Unable to open the output file: " << LinkToLogin::linkToLogin->pFilePath << std::endl;
            abort();
        }
        else
            std::cout << "Write the status to: " << LinkToLogin::linkToLogin->pFilePath << std::endl;
    }

    char buf[4096];
    memset(buf,0,4096);
    /* Buffer where events are returned */
    epoll_event events[MAXEVENTS];

    /* The event loop */
    std::vector<std::pair<void *,BaseClassSwitch::EpollObjectType> > elementsToDelete;
    int number_of_events, i;
    while(1)
    {
        number_of_events = Epoll::epoll.wait(events, MAXEVENTS);
        if(!elementsToDelete.empty())
        {
            unsigned int index=0;
            while(index<elementsToDelete.size())
            {
                switch(elementsToDelete.at(index).second)
                {
                    case BaseClassSwitch::EpollObjectType::MasterLink:
                        delete static_cast<LinkToLogin *>(elementsToDelete.at(index).first);
                    break;
                    default:
                    break;
                }
                index++;
            }
            elementsToDelete.clear();
        }
        for(i = 0; i < number_of_events; i++)
        {
            switch(static_cast<BaseClassSwitch *>(events[i].data.ptr)->getType())
            {
/*                case BaseClassSwitch::EpollObjectType::Timer:
                {
                    #ifdef SERVERBENCHMARKFULL
                    start_inter = std::chrono::high_resolution_clock::now();
                    #endif
                    #ifdef SERVERBENCHMARKFULL
                    timerDisplayEventBySeconds.addTimerCount();
                    #endif
                    static_cast<EpollTimer *>(events[i].data.ptr)->exec();
                    static_cast<EpollTimer *>(events[i].data.ptr)->validateTheTimer();
                    #ifdef SERVERBENCHMARKFULL
                    std::chrono::duration<unsigned long long int,std::nano> elapsed_seconds = std::chrono::high_resolution_clock::now()-start_inter;
                    EpollUnixSocketClientFinal::timeUsedForTimer+=elapsed_seconds.count();
                    #endif
                }
                break;*/
                case BaseClassSwitch::EpollObjectType::MasterLink:
                {
                    LinkToLogin * const client=static_cast<LinkToLogin *>(events[i].data.ptr);
                    if((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN) && !(events[i].events & EPOLLOUT)))
                    {
                        /* An error has occured on this fd, or the socket is not
                        ready for reading (why were we notified then?) */
                        if(!(events[i].events & EPOLLHUP))
                            std::cerr << "client epoll error: " << events[i].events << std::endl;
                        client->tryReconnect();
                        continue;
                    }
                    //ready to read
                    client->parseIncommingData();
                    if(events[i].events & EPOLLHUP || events[i].events & EPOLLRDHUP)
                        client->tryReconnect();
                }
                break;
                default:
                break;
            }
        }
    }
    fclose(LinkToLogin::linkToLogin->pFile);
    LinkToLogin::linkToLogin->closeSocket();
    delete LinkToLogin::linkToLogin;
    return EXIT_SUCCESS;
}
