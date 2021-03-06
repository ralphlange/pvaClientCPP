/* pvaClientPutGet.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author mrk
 * @date 2015.02
 */
#include <pv/event.h>

#define epicsExportSharedSymbols

#include <pv/pvaClient.h>

using std::tr1::static_pointer_cast;
using namespace epics::pvData;
using namespace epics::pvAccess;
using namespace std;

namespace epics { namespace pvaClient {

class ChannelPutGetRequesterImpl : public ChannelPutGetRequester
{
    PvaClientPutGet::weak_pointer pvaClientPutGet;
    PvaClient::weak_pointer pvaClient;
public:
    ChannelPutGetRequesterImpl(
        PvaClientPutGetPtr const & pvaClientPutGet,
        PvaClientPtr const &pvaClient)
    : pvaClientPutGet(pvaClientPutGet),
      pvaClient(pvaClient)
    {}
    virtual ~ChannelPutGetRequesterImpl() {
        if(PvaClient::getDebug()) std::cout << "~ChannelPutGetRequesterImpl" << std::endl;
    }

    virtual std::string getRequesterName() {
        PvaClientPutGetPtr clientPutGet(pvaClientPutGet.lock());
        if(!clientPutGet) return string("clientPutGet is null");
        return clientPutGet->getRequesterName();
    }

    virtual void message(std::string const & message, epics::pvData::MessageType messageType) {
        PvaClientPutGetPtr clientPutGet(pvaClientPutGet.lock());
        if(!clientPutGet) return;
        clientPutGet->message(message,messageType);
    }

    virtual void channelPutGetConnect(
        const Status& status,
        ChannelPutGet::shared_pointer const & channelPutGet,
        Structure::const_shared_pointer const & putStructure,
        Structure::const_shared_pointer const & getStructure)
    {
        PvaClientPutGetPtr clientPutGet(pvaClientPutGet.lock());
        if(!clientPutGet) return;
        clientPutGet->channelPutGetConnect(status,channelPutGet,putStructure,getStructure);  
    }

    virtual void putGetDone(
        const Status& status,
        ChannelPutGet::shared_pointer const & channelPutGet,
        PVStructurePtr const & getPVStructure,
        BitSet::shared_pointer const & getBitSet)
    {
        PvaClientPutGetPtr clientPutGet(pvaClientPutGet.lock());
        if(!clientPutGet) return;
        clientPutGet->putGetDone(status,channelPutGet,getPVStructure,getBitSet);
    }

    virtual void getPutDone(
        const Status& status,
        ChannelPutGet::shared_pointer const & channelPutGet,
        PVStructurePtr const & putPVStructure,
        BitSet::shared_pointer const & putBitSet)
    {
        PvaClientPutGetPtr clientPutGet(pvaClientPutGet.lock());
        if(!clientPutGet) return;
        clientPutGet->getPutDone(status,channelPutGet,putPVStructure,putBitSet);
    }


    virtual void getGetDone(
        const Status& status,
        ChannelPutGet::shared_pointer const & channelPutGet,
        PVStructurePtr const & getPVStructure,
        BitSet::shared_pointer const & getBitSet)
    {
        PvaClientPutGetPtr clientPutGet(pvaClientPutGet.lock());
        if(!clientPutGet) return;
        clientPutGet->getGetDone(status,channelPutGet,getPVStructure,getBitSet);
    }
};

PvaClientPutGetPtr PvaClientPutGet::create(
        PvaClientPtr const &pvaClient,
        PvaClientChannelPtr const & pvaClientChannel,
        PVStructurePtr const &pvRequest)
{
    PvaClientPutGetPtr clientPutGet(new PvaClientPutGet(pvaClient,pvaClientChannel,pvRequest));
    clientPutGet->channelPutGetRequester = ChannelPutGetRequesterImplPtr(
        new ChannelPutGetRequesterImpl(clientPutGet,pvaClient));
    return clientPutGet;
}

PvaClientPutGet::PvaClientPutGet(
        PvaClientPtr const &pvaClient,
        PvaClientChannelPtr const & pvaClientChannel,
        PVStructurePtr const &pvRequest)
: pvaClient(pvaClient),
  pvaClientChannel(pvaClientChannel),
  pvRequest(pvRequest),
  connectState(connectIdle),
  putGetState(putGetIdle)
{
    if(PvaClient::getDebug()) {
         cout<< "PvaClientPutGet::PvaClientPutGet"
             << " channelName " << pvaClientChannel->getChannel()->getChannelName()
             << endl;
    }
}

PvaClientPutGet::~PvaClientPutGet()
{
    if(PvaClient::getDebug()) {
        cout<< "PvaClientPutGet::~PvaClientPutGet"
           << " channelName " <<  pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
}

void PvaClientPutGet::channelStateChange(PvaClientChannelPtr const & pvaClientChannel, bool isConnected)
{
    if(PvaClient::getDebug()) {
           cout<< "PvaClientPutGet::channelStateChange"
               << " channelName " << pvaClientChannel->getChannel()->getChannelName()
               << " isConnected " << (isConnected ? "true" : "false")
               << endl;
    }
    if(isConnected&&!channelPutGet)
    {
        connectState = connectActive;
        channelPutGet = pvaClientChannel->getChannel()->createChannelPutGet(channelPutGetRequester,pvRequest);
    }
    PvaClientChannelStateChangeRequesterPtr req(pvaClientChannelStateChangeRequester.lock());
    if(req) {
          req->channelStateChange(pvaClientChannel,isConnected);
    }
}

void PvaClientPutGet::checkPutGetState()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::checkPutGetState"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    if(connectState==connectIdle){
          connect();
    }
    if(connectState==connectActive){
        string message = string("channel ") + pvaClientChannel->getChannel()->getChannelName()
            + " "
            + channelPutGetConnectStatus.getMessage();
        throw std::runtime_error(message);
    }
}

string PvaClientPutGet::getRequesterName()
{
     PvaClientPtr yyy = pvaClient.lock();
     if(!yyy) return string("PvaClientPutGet::getRequesterName() PvaClient isDestroyed");
     return yyy->getRequesterName();
}

void PvaClientPutGet::message(string const & message,MessageType messageType)
{
    PvaClientPtr yyy = pvaClient.lock();
    if(!yyy) return;
    yyy->message(message, messageType);
}

void PvaClientPutGet::channelPutGetConnect(
    const Status& status,
    ChannelPutGet::shared_pointer const & channelPutGet,
    StructureConstPtr const & putStructure,
    StructureConstPtr const & getStructure)
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::channelPutGetConnect"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << " status.isOK " << (status.isOK() ? "true" : "false")
           << endl;
    }
    {
        Lock xx(mutex);
        this->channelPutGet = channelPutGet;
        if(status.isOK()) {
            channelPutGetConnectStatus = status;
            connectState = connected;
            pvaClientPutData = PvaClientPutData::create(putStructure);
            pvaClientPutData->setMessagePrefix(channelPutGet->getChannel()->getChannelName());
            pvaClientGetData = PvaClientGetData::create(getStructure);
            pvaClientGetData->setMessagePrefix(channelPutGet->getChannel()->getChannelName());
           
        } else {
             stringstream ss;
             ss << pvRequest;
             string message = string("\nPvaClientPutGet::channelPutGetConnect)")
               + "\npvRequest\n" + ss.str()
               + "\nerror\n" + status.getMessage();
             channelPutGetConnectStatus = Status(Status::STATUSTYPE_ERROR,message);
        }
    }
    PvaClientPutGetRequesterPtr  req(pvaClientPutGetRequester.lock());
    if(req) {
          req->channelPutGetConnect(status,shared_from_this());
    }
    waitForConnect.signal();
    
}

void PvaClientPutGet::putGetDone(
        const Status& status,
        ChannelPutGet::shared_pointer const & channelPutGet,
        PVStructurePtr const & getPVStructure,
        BitSetPtr const & getChangedBitSet)
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::putGetDone"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << " status.isOK " << (status.isOK() ? "true" : "false")
           << endl;
    }
    channelPutGetStatus = status;
    if(status.isOK()) {
        PVStructurePtr pvs = pvaClientGetData->getPVStructure();
        pvs->copyUnchecked(*getPVStructure,*getChangedBitSet);
        BitSetPtr bs = pvaClientGetData->getChangedBitSet();
        bs->clear();
        *bs |= *getChangedBitSet;
    }
    PvaClientPutGetRequesterPtr  req(pvaClientPutGetRequester.lock());
    if(req) {
          req->putGetDone(status,shared_from_this());
    }
    waitForPutGet.signal();
}

void PvaClientPutGet::getPutDone(
    const Status& status,
    ChannelPutGet::shared_pointer const & channelPutGet,
    PVStructurePtr const & putPVStructure,
    BitSetPtr const & putBitSet)
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::getPutDone"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << " status.isOK " << (status.isOK() ? "true" : "false")
           << endl;
    }
    channelPutGetStatus = status;
    if(status.isOK()) {
        PVStructurePtr pvs = pvaClientPutData->getPVStructure();
        pvs->copyUnchecked(*putPVStructure,*putBitSet);
        BitSetPtr bs = pvaClientPutData->getChangedBitSet();
        bs->clear();
        *bs |= *putBitSet;
    }
    PvaClientPutGetRequesterPtr  req(pvaClientPutGetRequester.lock());
    if(req) {
          req->getPutDone(status,shared_from_this());
    }
    waitForPutGet.signal();
}

void PvaClientPutGet::getGetDone(
        const Status& status,
        ChannelPutGet::shared_pointer const & channelPutGet,
        PVStructurePtr const & getPVStructure,
        BitSetPtr const & getChangedBitSet)
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::getGetDone"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << " status.isOK " << (status.isOK() ? "true" : "false")
           << endl;
    }
    channelPutGetStatus = status;
    if(status.isOK()) {
        PVStructurePtr pvs = pvaClientGetData->getPVStructure();
        pvs->copyUnchecked(*getPVStructure,*getChangedBitSet);
        BitSetPtr bs = pvaClientGetData->getChangedBitSet();
        bs->clear();
        *bs |= *getChangedBitSet;
    }
    PvaClientPutGetRequesterPtr  req(pvaClientPutGetRequester.lock());
    if(req) {
          req->getGetDone(status,shared_from_this());
    }
    waitForPutGet.signal();
}

void PvaClientPutGet::connect()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::connect"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    issueConnect();
    Status status = waitConnect();
    if(status.isOK()) return;
    string message = string("channel ") 
        + pvaClientChannel->getChannel()->getChannelName()
        + " PvaClientPutGet::connect "
        + status.getMessage();
    throw std::runtime_error(message);
}

void PvaClientPutGet::issueConnect()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::issueConnect"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    if(connectState!=connectIdle) {
        string message = string("channel ")
            + pvaClientChannel->getChannel()->getChannelName()
            + " pvaClientPutGet already connected ";
        throw std::runtime_error(message);
    }
    connectState = connectActive;
    channelPutGetConnectStatus = Status(Status::STATUSTYPE_ERROR, "connect active");
    channelPutGet = pvaClientChannel->getChannel()->createChannelPutGet(channelPutGetRequester,pvRequest);
}

Status PvaClientPutGet::waitConnect()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::waitConnect"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    {
         Lock xx(mutex);
         if(connectState==connected) {
             if(!channelPutGetConnectStatus.isOK()) connectState = connectIdle;
             return channelPutGetConnectStatus;
        }
        if(connectState!=connectActive)  {
            string message = string("channel ")
                + pvaClientChannel->getChannel()->getChannelName()
                + " PvaClientPutGet::waitConnect illegal connect state ";
            throw std::runtime_error(message);
        }
    }
    waitForConnect.wait();
    if(!channelPutGetConnectStatus.isOK()) connectState = connectIdle;
    return channelPutGetConnectStatus;
}


void PvaClientPutGet::putGet()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::putGet"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    issuePutGet();
    Status status = waitPutGet();
    if(status.isOK()) return;
    string message = string("channel ") 
        + pvaClientChannel->getChannel()->getChannelName()
        + " PvaClientPut::putGet "
        + status.getMessage();
    throw std::runtime_error(message);
}

void PvaClientPutGet::issuePutGet()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::issuePutGet"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    if(connectState==connectIdle) connect();
    if(putGetState==putGetActive) {
        string message = string("channel ")
            + pvaClientChannel->getChannel()->getChannelName()
            + " PvaClientPutGet::issuePutGet get or put aleady active ";
        throw std::runtime_error(message);
    }
    putGetState = putGetActive;
    channelPutGet->putGet(pvaClientPutData->getPVStructure(),pvaClientPutData->getChangedBitSet());
}


Status PvaClientPutGet::waitPutGet()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::waitPutGet"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    if(putGetState!=putGetActive){
        string message = string("channel ")
            + pvaClientChannel->getChannel()->getChannelName()
            + " PvaClientPutGet::waitPutGet get or put aleady active ";
        throw std::runtime_error(message);
    }
    waitForPutGet.wait();
    putGetState = putGetComplete;
    if(channelPutGetStatus.isOK()) pvaClientPutData->getChangedBitSet()->clear();
    return channelPutGetStatus;
}

void PvaClientPutGet::getGet()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::getGet"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    issueGetGet();
    Status status = waitGetGet();
    if(status.isOK()) return;
    string message = string("channel ") 
        + pvaClientChannel->getChannel()->getChannelName()
        + " PvaClientPut::getGet "
        + status.getMessage();
    throw std::runtime_error(message);
}

void PvaClientPutGet::issueGetGet()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::issueGetGet"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    if(connectState==connectIdle) connect();
    if(putGetState==putGetActive) {
        string message = string("channel ")
            + pvaClientChannel->getChannel()->getChannelName()
            + " PvaClientPutGet::issueGetGet get or put aleady active ";
        throw std::runtime_error(message);
    }
    putGetState = putGetActive;
    channelPutGet->getGet();
}

Status PvaClientPutGet::waitGetGet()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::waitGetGet"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    if(putGetState!=putGetActive){
        string message = string("channel ")
            + pvaClientChannel->getChannel()->getChannelName()
            + " PvaClientPutGet::waitGetGet get or put aleady active ";
        throw std::runtime_error(message);
    }
    waitForPutGet.wait();
    putGetState = putGetComplete;
    return channelPutGetStatus;
}

void PvaClientPutGet::getPut()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::getGetPut"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    issueGetPut();
    Status status = waitGetPut();
    if(status.isOK()) return;
    string message = string("channel ") 
        + pvaClientChannel->getChannel()->getChannelName()
        + " PvaClientPut::getPut "
        + status.getMessage();
    throw std::runtime_error(message);
}

void PvaClientPutGet::issueGetPut()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::issueGetPut"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    if(connectState==connectIdle) connect();
    if(putGetState==putGetActive) {
        string message = string("channel ")
            + pvaClientChannel->getChannel()->getChannelName()
            + " PvaClientPutGet::issueGetPut get or put aleady active ";
        throw std::runtime_error(message);
    }
    putGetState = putGetActive;
    channelPutGet->getPut();
}

Status PvaClientPutGet::waitGetPut()
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::waitGetPut"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    if(putGetState!=putGetActive){
        string message = string("channel ")
            + pvaClientChannel->getChannel()->getChannelName()
            + " PvaClientPutGet::waitGetPut get or put aleady active ";
        throw std::runtime_error(message);
    }
    waitForPutGet.wait();
    putGetState = putGetComplete;
    return channelPutGetStatus;
}

PvaClientGetDataPtr PvaClientPutGet::getGetData()
{
    if(PvaClient::getDebug()) {
           cout<< "PvaClientPutGet::getGetData"
               << " channelName " << pvaClientChannel->getChannel()->getChannelName()
               << endl;
    }
    checkPutGetState();
    if(putGetState==putGetIdle) getGet();
    return pvaClientGetData;
}

PvaClientPutDataPtr PvaClientPutGet::getPutData()
{
    if(PvaClient::getDebug()) {
           cout<< "PvaClientPutGet::getPutData"
               << " channelName " << pvaClientChannel->getChannel()->getChannelName()
               << endl;
    }
    checkPutGetState();
   if(putGetState==putGetIdle) getPut();
    return pvaClientPutData;
}

void PvaClientPutGet::setRequester(PvaClientPutGetRequesterPtr const & pvaClientPutGetRequester)
{
    if(PvaClient::getDebug()) {
        cout << "PvaClientPutGet::setRequester"
           << " channelName " << pvaClientChannel->getChannel()->getChannelName()
           << endl;
    }
    this->pvaClientPutGetRequester = pvaClientPutGetRequester;
}


PvaClientChannelPtr PvaClientPutGet::getPvaClientChannel()
{
    return pvaClientChannel;
}



}}
