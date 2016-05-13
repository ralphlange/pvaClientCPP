/* pvaClient.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author mrk
 * @date 2015.02
 */
#ifndef PVACLIENT_H
#define PVACLIENT_H

#ifdef epicsExportSharedSymbols
#   define pvaClientEpicsExportSharedSymbols
#   undef epicsExportSharedSymbols
#endif

#include <list>
#include <iostream>
#include <compilerDependencies.h>
#include <pv/requester.h>
#include <pv/status.h>
#include <pv/event.h>
#include <pv/lock.h>
#include <pv/pvData.h>
#include <pv/pvCopy.h>
#include <pv/pvTimeStamp.h>
#include <pv/timeStamp.h>
#include <pv/pvAlarm.h>
#include <pv/alarm.h>
#include <pv/pvAccess.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>
#include <pv/createRequest.h>
#include <pv/nt.h>

#ifdef pvaClientEpicsExportSharedSymbols
#   define epicsExportSharedSymbols
#	undef pvaClientEpicsExportSharedSymbols
#endif

#include <shareLib.h>


namespace epics { namespace pvaClient { 

class PvaClient;
typedef std::tr1::shared_ptr<PvaClient> PvaClientPtr;
class PvaClientGetData;
typedef std::tr1::shared_ptr<PvaClientGetData> PvaClientGetDataPtr;
class PvaClientPutData;
typedef std::tr1::shared_ptr<PvaClientPutData> PvaClientPutDataPtr;
class PvaClientMonitorData;
typedef std::tr1::shared_ptr<PvaClientMonitorData> PvaClientMonitorDataPtr;
class PvaClientChannel;
typedef std::tr1::shared_ptr<PvaClientChannel> PvaClientChannelPtr;
class PvaClientField;
typedef std::tr1::shared_ptr<PvaClientField> PvaClientFieldPtr;
class PvaClientProcess;
typedef std::tr1::shared_ptr<PvaClientProcess> PvaClientProcessPtr;
class PvaClientGet;
typedef std::tr1::shared_ptr<PvaClientGet> PvaClientGetPtr;
class PvaClientPut;
typedef std::tr1::shared_ptr<PvaClientPut> PvaClientPutPtr;
class PvaClientPutGet;
typedef std::tr1::shared_ptr<PvaClientPutGet> PvaClientPutGetPtr;
class PvaClientMonitor;
typedef std::tr1::shared_ptr<PvaClientMonitor> PvaClientMonitorPtr;
class PvaClientMonitorRequester;
typedef std::tr1::shared_ptr<PvaClientMonitorRequester> PvaClientMonitorRequesterPtr;
typedef std::tr1::weak_ptr<PvaClientMonitorRequester> PvaClientMonitorRequesterWPtr;
class PvaClientArray;
typedef std::tr1::shared_ptr<PvaClientArray> PvaClientArrayPtr;


// following are private to pvaClient
class PvaClientChannelCache;
typedef std::tr1::shared_ptr<PvaClientChannelCache> PvaClientChannelCachePtr;

/**
 * @brief A synchronous interface to pvAccess plus convenience methods.
 *
 * @author mrk
 */
class epicsShareClass PvaClient :
     public epics::pvData::Requester,
     public std::tr1::enable_shared_from_this<PvaClient>
{
public:
    POINTER_DEFINITIONS(PvaClient);

    /**
     * Destructor
     */
    ~PvaClient();
    /** Get the single instance of PvaClient.
     * @param providerNames Space separated list of provider names.
     * @return shared pointer to the single instance.
     */
    static PvaClientPtr get(std::string const & providerNames);
    /** Get the single instance of PvaClient.
     * calls get with providerNames "pva ca".
     * @return shared pointer to the single instance.
     */
    static PvaClientPtr get() {return get("pva ca");}
    /** Create an instance of PvaClient with providerName "pva ca".
     * \deprecated This method will go away in future versions. Use get instead.
     * @return shared pointer to the single instance
     */
    static PvaClientPtr create() EPICS_DEPRECATED
    {
        return get("pva ca");
    }
    /** Get the requester name.
     * @return The name.
     */
    std::string getRequesterName();
    /** A new message.
     * If a requester is set then it is called otherwise message is displayed
     * on standard out.
     * @param message The message.
     * @param messageType The type.
     */
    void message(
        std::string const & message,
        epics::pvData::MessageType messageType);
    /** Get a cached channel or create and connect to a new channel.
     *
     * The provider is pva. The timeout is 5 seconds.
     * @param channelName The channelName.
     * @return The interface.
     * @throw runtime_error if connection fails.
     */
    PvaClientChannelPtr channel(std::string const & channelName)
    { return channel(channelName,"pva", 5.0); }
    /** Get a cached channel or create and connect to a new channel.
     *
     * The timeout is 5 seconds.
     * @param channelName The channelName.
     * @param providerName The providerName.
     * @return The interface.
     * @throw runtime_error if connection fails.
     */
    PvaClientChannelPtr channel(
        std::string const & channelName,
        std::string const &providerName)
    { return channel(channelName,providerName, 5.0); }
    /** Get a cached channel or create and connect to a new channel.
     * @param channelName The channelName.
     * @param providerName The providerName.
     * @param timeOut The number of seconds to wait for connection. 0.0 means forever.
     * @return The interface.
     * @throw runtime_error if connection fails.
     */
    PvaClientChannelPtr channel(
        std::string const & channelName,
        std::string const &providerName,
        double timeOut);
    /** Create an PvaClientChannel. The provider is pva.
     * @param channelName The channelName.
     * @return The interface.
     * @throw runtime_error if connection fails.
     */
    PvaClientChannelPtr createChannel(std::string const & channelName)
    { return createChannel(channelName,"pva");}
    /** Create an PvaClientChannel with the specified provider.
     * @param channelName The channelName.
     * @param providerName The provider.
     * @return The interface.
     * @throw runtime_error if connection fails.
     */
    PvaClientChannelPtr createChannel(
       std::string const & channelName,
       std::string const & providerName);
    
    /** Set a requester.
     * The default is for PvaClient to handle messages by printing to System.out.
     * @param requester The requester.
     */
    void setRequester(epics::pvData::RequesterPtr const & requester);
    /** Clear the requester. PvaClient will handle messages.
     */
    void clearRequester();
    /** Show the list of cached channels.
     */
    void showCache();
     /** Get the number of cached channels.
     */
    size_t cacheSize();
    /** Should debug info be shown?
     * @param value true or false
     */
    static void setDebug(bool value) {debug = value;}
    /** Is debug set?
     * @return true or false
     */
    static bool getDebug() {return debug;}
    /** Get shared pointer to this
     */
    PvaClientPtr getPtrSelf()
    {
        return shared_from_this();
    }
    /** Deprecated method
     * \deprecated This method will go away in future versions. 
     */
    void destroy()  EPICS_DEPRECATED {}

private:
    static bool debug;
    PvaClient(std::string const & providerNames);
    PvaClientChannelCachePtr pvaClientChannelCache;
    epics::pvData::Requester::weak_pointer requester;
    bool isDestroyed;
    bool pvaStarted;
    bool caStarted;
    epics::pvData::Mutex mutex;
};

// folowing private to PvaClientChannel
class PvaClientGetCache;
typedef std::tr1::shared_ptr<PvaClientGetCache> PvaClientGetCachePtr;
class PvaClientPutCache;
typedef std::tr1::shared_ptr<PvaClientPutCache> PvaClientPutCachePtr;
class ChannelRequesterImpl;

/** 
 * @brief An easy to use alternative to directly calling the Channel methods of pvAccess.
 *
 * @author mrk
 */
class epicsShareClass PvaClientChannel :
    public epics::pvAccess::ChannelRequester,
    public std::tr1::enable_shared_from_this<PvaClientChannel>
{
public:
    POINTER_DEFINITIONS(PvaClientChannel);
    ~PvaClientChannel();
    /** ChannelRequester method
     * @param status The status
     * @param channel The channel
     */
    void channelCreated(
        const epics::pvData::Status& status,
        epics::pvAccess::Channel::shared_pointer const & channel);
    /** ChannelRequester method
     * @param channel The channel
     * @param connectionState The connection state.
     */
    void channelStateChange(
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvAccess::Channel::ConnectionState connectionState);
    /** ChannelRequester method
     * @return The name
     */
    std::string getRequesterName();
    /** ChannelRequester method
     * @param message The message.
     * @param messageType The message type.
     */
    void message(
        std::string const & message,
        epics::pvData::MessageType messageType);
    /** Get the name of the channel to which PvaClientChannel is connected.
     * @return The channel name.
     */
    std::string getChannelName();
    /** Get the the channel to which PvaClientChannel is connected.
     * @return The channel interface.
     */
    epics::pvAccess::Channel::shared_pointer getChannel();
    /** Connect to the channel.
     * This calls issueConnect and waitConnect.
     * @param timeout The time to wait for connecting to the channel. The defaut is 5 seconds.
     * @throw runtime_error if connection fails.
     */
    void connect(double timeout=5.0);
    /** Issue a connect request and return immediately.
     */
    void issueConnect();
    /** Wait until the connection completes or for timeout.
     * @param timeout The time in seconds to wait. A value of 0 means forever.
     * @return status.
     */
    epics::pvData::Status waitConnect(double timeout);
    /** Calls the next method with subField = "";
     * @return The interface.
     */
    PvaClientFieldPtr createField();
    /** Create an PvaClientField for the specified subField.
     * @param subField The syntax for subField is defined in package org.epics.pvdata.copy
     * @return The interface.
     */
    PvaClientFieldPtr createField(std::string const & subField);
    /** Calls the next method with request = "";
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientProcessPtr createProcess();
    /** First call createRequest as implemented by pvDataJava and then calls the next method.
     * @param request The request as described in package org.epics.pvdata.copy
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientProcessPtr createProcess(std::string const & request);
    /** Creates an PvaClientProcess. 
     * @param pvRequest The syntax of pvRequest is described in package org.epics.pvdata.copy.
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientProcessPtr createProcess(epics::pvData::PVStructurePtr const &  pvRequest);
    /** Call the next method with request =  "field(value,alarm,timeStamp)" 
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientGetPtr get();
    /** Get a cached PvaClientGet or create and connect to a new PvaClientGet.
     * Then call it's get method.
     * If connection can not be made an exception is thrown.
     * @param request The request as described in package org.epics.pvdata.copy
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientGetPtr get(std::string const & request);
    /** Call the next method with request =  "field(value,alarm,timeStamp)" 
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientGetPtr createGet();
    /** First call createRequest as implemented by pvDataJava and then call the next method.
     * @param request The request as described in package org.epics.pvdata.copy
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientGetPtr createGet(std::string const & request);
    /** Creates an PvaClientGet.
     * @param pvRequest The syntax of pvRequest is described in package org.epics.pvdata.copy.
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientGetPtr createGet(epics::pvData::PVStructurePtr const &  pvRequest);
    /** Call the next method with request =  "field(value)" 
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientPutPtr put();
    /** get a cached PvaClientPut or create and connect to a new PvaClientPut.
     *  Then call it's get method.
     *  @param request The request as described in package org.epics.pvdata.copy
     * @return The interface.
     * @throw runtime_error if connection fails
     */
    PvaClientPutPtr put(std::string const & request);
    /** Call the next method with request = "field(value)" 
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientPutPtr createPut();
    /** First call createRequest as implemented by pvDataJava and then calls the next method.
     * @param request The request as described in package org.epics.pvdata.copy
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientPutPtr createPut(std::string const & request);
    /** Create an PvaClientPut.
     * @param pvRequest The syntax of pvRequest is described in package org.epics.pvdata.copy.
     * @return The interface.
     */
    PvaClientPutPtr createPut(epics::pvData::PVStructurePtr const & pvRequest);
    /** Call the next method with request = "record[process=true]putField(argument)getField(result)".
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientPutGetPtr createPutGet();
    /** First call createRequest as implemented by pvDataJava and then calls the next method.
     * @param request The request as described in package org.epics.pvdata.copy
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientPutGetPtr createPutGet(std::string const & request);
    /** Create an PvaClientPutGet.
     * @param pvRequest The syntax of pvRequest is described in package org.epics.pvdata.copy.
     * @return The interface.
     */
    PvaClientPutGetPtr createPutGet(epics::pvData::PVStructurePtr const & pvRequest);
    /** Call the next method with request = "field(value)";
     * @return The interface.
     */
    PvaClientArrayPtr createArray();
    /** First call createRequest as implemented by pvDataJava and then call the next method.
     * @param request The request as described in package org.epics.pvdata.copy
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientArrayPtr createArray(std::string const & request);
    /** Create an PvaClientArray.
     * @param pvRequest The syntax of pvRequest is described in package org.epics.pvdata.copy.
     * @return The interface.
     */
    PvaClientArrayPtr createArray(epics::pvData::PVStructurePtr const &  pvRequest);
    /** Call the next method with request =  "field(value,alarm,timeStamp)" 
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientMonitorPtr monitor();
    /** Create and connect to a new PvaClientMonitor.
     * Then call it's start method.
     * If connection can not be made an exception is thrown.
     * @param request The request as described in package org.epics.pvdata.copy
     * @return The interface.
     */
    PvaClientMonitorPtr monitor(std::string const & request);
    /** Call the next method with request =  "field(value,alarm,timeStamp)" 
      * @param pvaClientMonitorRequester The client callback.
      * @return The interface.
      * @throw runtime_error if failure.
      */
    PvaClientMonitorPtr monitor(PvaClientMonitorRequesterPtr const & pvaClientMonitorRequester);

    /** Create and connect to a new PvaClientMonitor.
     * Then call it's start method.
     * If connection can not be made an exception is thrown.
     * @param request The request as described in package org.epics.pvdata.copy
     * @param pvaClientMonitorRequester The client callback.
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientMonitorPtr monitor(
        std::string const & request,
        PvaClientMonitorRequesterPtr const & pvaClientMonitorRequester);
    /** Call the next method with request = "field(value.alarm,timeStamp)" 
     * @return The interface.
     */
    PvaClientMonitorPtr createMonitor();
    /**
     * @brief First call createRequest as implemented by pvDataJava and then calls the next method.
     * @param request The request as described in package org.epics.pvdata.copy
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientMonitorPtr createMonitor(std::string const & request);
    /** Create an PvaClientMonitor.
     * @param pvRequest  The syntax of pvRequest is described in package org.epics.pvdata.copy.
     * @return The interface.
     * @throw runtime_error if failure.
     */
    PvaClientMonitorPtr createMonitor(epics::pvData::PVStructurePtr const &  pvRequest);
     /** Show the list of cached gets and puts.
     */
    void showCache();
     /** Get the number of cached gets and puts.
     */
    size_t cacheSize();
    PvaClientChannelPtr getPtrSelf()
    {
        return shared_from_this();
    }
    /** Deprecated method
     * \deprecated This method will go away in future versions. 
     */
    void destroy()  EPICS_DEPRECATED {}
private:
    
    static PvaClientChannelPtr create(
         PvaClientPtr const &pvaClient,
         std::string const & channelName,
         std::string const & providerName);
    PvaClientChannel(
        PvaClientPtr const &pvaClient,
        std::string const & channelName,
        std::string const & providerName);

    enum ConnectState {connectIdle,connectActive,notConnected,connected};

    PvaClient::weak_pointer pvaClient;
    std::string channelName;
    std::string providerName;
    ConnectState connectState;
    bool isDestroyed;
    
    epics::pvData::CreateRequest::shared_pointer createRequest;
    PvaClientGetCachePtr pvaClientGetCache;
    PvaClientPutCachePtr pvaClientPutCache;

    epics::pvData::Mutex mutex;
    epics::pvData::Event waitForConnect;
    epics::pvAccess::Channel::shared_pointer channel;
    friend class PvaClient;
};

/** 
 *  @brief A class that holds data returned by PvaClientGet or PvaClientPutGet
 *
 */
class epicsShareClass PvaClientGetData
{
public:
    POINTER_DEFINITIONS(PvaClientGetData);
    ~PvaClientGetData() {}
    /** Set a prefix for throw messages.
     * @param value The prefix.
     */
    void setMessagePrefix(std::string const & value);
   /** Get the structure.
    */
   epics::pvData::StructureConstPtr getStructure();
   /** Get the pvStructure.
    * @return the pvStructure.
    */
   epics::pvData::PVStructurePtr getPVStructure();
   /** Get the BitSet for the pvStructure
    * This shows which fields have changed value.
    * @return The bitSet
    */
   epics::pvData::BitSetPtr getChangedBitSet();
   /** Show the fields that have changed.
    * @param out The stream that shows the changed fields.
    * @return The stream that was input
    */
   std::ostream & showChanged(std::ostream & out);
    /** New data is present.
     * @param pvStructureFrom The new data.
     * @param bitSetFrom the bitSet showing which values have changed.
     */
    void setData(
        epics::pvData::PVStructurePtr const & pvStructureFrom,
        epics::pvData::BitSetPtr const & bitSetFrom);
    /** Is there a top level field named value.
     * @return The answer.
     */
    bool hasValue();
    /** Is the value field a scalar?
     * @return The answer.
     */
    bool isValueScalar();
    /** Is the value field a scalar array?
     * @return The answer.
     */
    bool isValueScalarArray();
    /** Get the interface to the value field.
     * @return The interface. an excetion is thrown if a value field does not exist.
     */
    epics::pvData::PVFieldPtr getValue();
    /**
     * @brief Return the interface to a scalar value field.
     * @return The interface for a scalar value field.
     * @throw runtime_error if failure.
     */
    epics::pvData::PVScalarPtr getScalarValue();
    /** Getthe interface to an array value field.
     * @return The interface.
     * @throw runtime_error if failure.
     */
    std::tr1::shared_ptr<epics::pvData::PVArray> getArrayValue();
    /** Get the interface to a scalar array value field.
     * @return Return the interface.
     * @throw runtime_error if failure.
     */
    std::tr1::shared_ptr<epics::pvData::PVScalarArray> getScalarArrayValue();
    /** Get the value as a double.
     * @throw runtime_error if failure.
     * @return The value.
     */
    double getDouble();
    /** Get the value as a string.
     * @return The value.
     * @throw runtime_error if failure.
     */
    std::string getString();
    /** Get the value as a double array.
     * @return The value.
     * @throw runtime_error if failure.
     */
    epics::pvData::shared_vector<const double>  getDoubleArray();
    /** Get the value as a string array.
     * @return The value.
     * @throw runtime_error if failure.
     */
    epics::pvData::shared_vector<const std::string>  getStringArray();
    /** Get the alarm.
     * If the pvStructure as an alarm field it's values are returned.
     * If no then alarm shows that no alarm defined.
     * @return The alarm.
     */
    epics::pvData::Alarm getAlarm();
    /** Get the timeStamp.
     * If the pvStructure as a timeStamp field, it's values are returned.
     * If no then all fields are 0.
     * @return The timeStamp.
     */
    epics::pvData::TimeStamp getTimeStamp();
    /** Factory method for creating an instance of PvaClientGetData.
     * NOTE: Not normally called by clients
     * @param structure Introspection interface
     */
    static PvaClientGetDataPtr create(epics::pvData::StructureConstPtr const & structure);
private:
    PvaClientGetData(epics::pvData::StructureConstPtr const & structure);
    void checkValue();
    epics::pvData::StructureConstPtr structure;
    epics::pvData::PVStructurePtr pvStructure;
    epics::pvData::BitSetPtr bitSet;

    std::string messagePrefix;
    epics::pvData::PVFieldPtr pvValue;
    epics::pvData::PVAlarm pvAlarm;
    epics::pvData::PVTimeStamp pvTimeStamp;
    friend class PvaClientGet;
    friend class PvaClientPutGet;
};

class PvaClientPostHandlerPvt; // private to PvaClientPutData
/** 
 *  @brief A class that holds data given to  by PvaClientPut or PvaClientPutGet
 *
 */
class epicsShareClass PvaClientPutData 
{
public:
    POINTER_DEFINITIONS(PvaClientPutData);
    
    ~PvaClientPutData() {}
    /** Set a prefix for throw messages.
     * @param value The prefix.
     */
    void setMessagePrefix(std::string const & value);
   /** Get the structure.
    * @return the structure.
    */
   epics::pvData::StructureConstPtr getStructure();
    /** Get the pvStructure.
     * @return the pvStructure.
     */
    epics::pvData::PVStructurePtr getPVStructure();
    /**  Get the BitSet for the pvStructure
     * This shows which fields have changed value.
     * @return The bitSet
     */
    epics::pvData::BitSetPtr getChangedBitSet();
    /**  Show the fields that have changed.
     * @param out The stream that shows the changed fields.
     * @return The stream that was input
     */
    std::ostream & showChanged(std::ostream & out);
    /**
     * Is there a top level field named value.
     * @return The answer.
     */
    bool hasValue();
    /** Is the value field a scalar?
     * @return The answer.
     */
    bool isValueScalar();
    /** Is the value field a scalar array?
     * @return The answer.
     */
    bool isValueScalarArray();
    /** Get the interface to the value field.
     * @return The interface. an excetion is thrown if a value field does not exist.
     */
    epics::pvData::PVFieldPtr getValue();
    /** Get the interface to a scalar value field.
     * @return The interface for a scalar value field.
     * @throw runtime_error if failure.
     */
    epics::pvData::PVScalarPtr getScalarValue();
    /** Get the interface to an array value field.
     * @return The interface.
     * An exception is thown if no array value field.
     */
    std::tr1::shared_ptr<epics::pvData::PVArray> getArrayValue();
    /** Get the interface to a scalar array value field.
     * @return Return the interface.
     * @throw runtime_error if failure.
     */
    std::tr1::shared_ptr<epics::pvData::PVScalarArray> getScalarArrayValue();
    /** Get the value as a double.
     * If value is not a numeric scalar an exception is thrown.
     * @return The value.
     */
    double getDouble();
    /** Get the value as a string.
     * @return The value.
     * @throw runtime_error if failure.
     */
    std::string getString();
    /** Get the value as a double array.
     * If the value is not a numeric array an exception is thrown.
     * @return The value.
     */
    epics::pvData::shared_vector<const double>  getDoubleArray();
    /** Get the value as a string array.
     * @return The value.
     * @throw runtime_error if failure.
     */
    epics::pvData::shared_vector<const std::string>  getStringArray();
    /** Put the value as a double.
     * @param value The new value.
     * An exception is also thrown if the actualy type can cause an overflow.
     * @throw runtime_error if failure.
     */
    void putDouble(double value);
    /** Put the value as a string.
     * If value is not a  scalar an exception is thrown.
     */
    void putString(std::string const & value);
    /** Copy the array to the value field.
     * @param value The place where data is copied.
     * @throw runtime_error if failure.
     */
    void putDoubleArray(epics::pvData::shared_vector<const double> const & value);
    /** Copy array to the value field.
     * @param value data source
     * @throw runtime_error if failure.
     */
    void putStringArray(epics::pvData::shared_vector<const std::string> const & value);
    /** Copy array to the value field.
     * @param value data source
     * @throw runtime_error if failure.
     */
    void putStringArray(std::vector<std::string> const & value);
    /** Factory method for creating an instance of PvaClientGetData.
     * NOTE: Not normally called by clients
     * @param structure Introspection interface
     */
     static PvaClientPutDataPtr create(epics::pvData::StructureConstPtr const & structure);
private:
    PvaClientPutData(epics::pvData::StructureConstPtr const &structure);
    void checkValue();
    void postPut(size_t fieldNumber);

    std::vector<epics::pvData::PostHandlerPtr> postHandler;
    epics::pvData::StructureConstPtr structure;
    epics::pvData::PVStructurePtr pvStructure;
    epics::pvData::BitSetPtr bitSet;
    friend class PvaClientPostHandlerPvt;

    std::string messagePrefix;
    epics::pvData::PVFieldPtr pvValue;
    friend class PvaClientPut;
    friend class PvaClientPutGet;
};

/**
 * @brief A class that holds data returned by PvaClientMonitor
 *
 */
class epicsShareClass PvaClientMonitorData
{
public:
    POINTER_DEFINITIONS(PvaClientMonitorData);
    
    ~PvaClientMonitorData() {}
    /** Set a prefix for throw messages.
     * @param value The prefix.
     */
    void setMessagePrefix(std::string const & value);
   /**  Get the structure.
    * @return the structure.
    */
   epics::pvData::StructureConstPtr getStructure();
    /**  Get the pvStructure.
     * @return the pvStructure.
     */
    epics::pvData::PVStructurePtr getPVStructure();
    /**  Get the BitSet for the pvStructure
     * This shows which fields have changed value.
     * @return The bitSet
     */
    epics::pvData::BitSetPtr getChangedBitSet();
    /** Get the overrun BitSet for the pvStructure
     * This shows which fields have had more than one change.
     * @return The bitSet
     */
    epics::pvData::BitSetPtr getOverrunBitSet();
    /** Show the fields that have changed.
     * @param out The stream that shows the changed fields.
     * @return The stream that was input
     */
    std::ostream & showChanged(std::ostream & out);
    /** Show the fields that have overrun.
     * @param out The stream that shows the overrun fields.
     * @return The stream that was input
     */
    std::ostream & showOverrun(std::ostream & out);
    /** Is there a top level field named value.
     * @return The answer.
     */
    bool hasValue();
    /** Is the value field a scalar?
     * @return The answer.
     */
    bool isValueScalar();
    /** Is the value field a scalar array?
     * @return The answer.
     */
    bool isValueScalarArray();
    /** Get the interface to the value field.
     * @return The interface. an excetion is thrown if a value field does not exist.
     */
    epics::pvData::PVFieldPtr getValue();
    /** Get the interface to a scalar value field.
     * @return The interface for a scalar value field.
     * @throw runtime_error if failure.
     * An exception is thown if no scalar value field.
     */
    epics::pvData::PVScalarPtr getScalarValue();
    /** Get the interface to an array value field.
     * @return The interface.
     * @throw runtime_error if failure.
     * An exception is thown if no array value field.
     */
    std::tr1::shared_ptr<epics::pvData::PVArray> getArrayValue();
    /** Get the interface to a scalar array value field.
     * @return Return the interface.
     * @throw runtime_error if failure.
     * An exception is thown if no scalar array value field.
     */
    std::tr1::shared_ptr<epics::pvData::PVScalarArray> getScalarArrayValue();
    /** Get the value as a double.
     * If value is not a numeric scalar an exception is thrown.
     * @return The value.
     */
    double getDouble();
    /** Get the value as a string.
     * If value is not a scalar an exception is thrown
     * @return The value.
     * @throw runtime_error if failure.
     */
    std::string getString();
    /** Get the value as a double array.
     * If the value is not a numeric array an exception is thrown.
     * @return The value.
     * @throw runtime_error if failure.
     */
    epics::pvData::shared_vector<const double>  getDoubleArray();
    /** Get the value as a string array.
     * If the value is not a string array an exception is thrown.
     * @return The value.
     * @throw runtime_error if failure.
     */
    epics::pvData::shared_vector<const std::string>  getStringArray();
    /** Get the alarm.
     * If the pvStructure as an alarm field it's values are returned.
     * If no then alarm shows that not alarm defined.
     * @return The alarm.
     * @throw runtime_error if failure.
     */
    epics::pvData::Alarm getAlarm();
    /** Get the timeStamp.
     * If the pvStructure has a timeStamp field, it's values are returned.
     * If no then all fields are 0.
     * @return The timeStamp.
     */
    epics::pvData::TimeStamp getTimeStamp();
    /** Factory method for creating an instance of PvaClientGetData.
     * NOTE: Not normally called by clients
     * @param structure Introspection interface
     */
     static PvaClientMonitorDataPtr create(epics::pvData::StructureConstPtr const & structure);
     /** Put data into PVStructure from monitorElement
      *  NOTE: Not normally called by clients
      * @param monitorElement the monitorElement that has new data.
      */
     void setData(epics::pvData::MonitorElementPtr const & monitorElement);
private:
    PvaClientMonitorData(epics::pvData::StructureConstPtr const & structure);
    void checkValue();
    
    epics::pvData::StructureConstPtr structure;
    epics::pvData::PVStructurePtr pvStructure;
    epics::pvData::BitSetPtr changedBitSet;
    epics::pvData::BitSetPtr overrunBitSet;

    std::string messagePrefix;
    epics::pvData::PVFieldPtr pvValue;
    epics::pvData::PVAlarm pvAlarm;
    epics::pvData::PVTimeStamp pvTimeStamp;
    friend class PvaClientMonitor;
};

/**
 * @brief An easy to use alternative to ChannelProcess.
 *
 * @author mrk
 */
class epicsShareClass PvaClientProcess :
    public epics::pvAccess::ChannelProcessRequester
{
public:
    POINTER_DEFINITIONS(PvaClientProcess);
    /** Create a PvaClientProcess.
     * @param &pvaClient Interface to PvaClient
     * @param channel Interface to Channel
     * @param pvRequest The request structure.
     * @return The interface to the PvaClientProcess.
     */
    static PvaClientProcessPtr create(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest
    );
    /** Destructor
     */
    ~PvaClientProcess();
    std::string getRequesterName();
    void message(std::string const & message,epics::pvData::MessageType messageType);
    void channelProcessConnect(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelProcess::shared_pointer const & channelProcess);
    void processDone(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelProcess::shared_pointer const & channelProcess);
    /** Call issueConnect and then waitConnect.
     * An exception is thrown if connect fails.
     * @throw runtime_error if failure.
     */
    void connect();
    /** Issue the channelProcess connection to the channel.
     * This can only be called once.
     */
    void issueConnect();
    /** Wait until the channelProcess connection to the channel is complete.
     * @return status;
     */
    epics::pvData::Status waitConnect();
    /** Call issueProcess and then waitProcess.
     * An exception is thrown if get fails.
     */
    void process();
    /** Issue a process and return immediately.
     */
    void issueProcess();
    /** Wait until process completes.
     * @return status.
     */
    epics::pvData::Status waitProcess();
     /** Deprecated method
     * \deprecated This method will go away in future versions. 
     */
    void destroy()  EPICS_DEPRECATED {}
private:
    PvaClientProcess(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest);
    
    enum ProcessConnectState {connectIdle,connectActive,connected};

    PvaClient::weak_pointer pvaClient;
    epics::pvAccess::Channel::shared_pointer channel;
    epics::pvData::PVStructurePtr pvRequest;
    epics::pvData::Mutex mutex;
    epics::pvData::Event waitForConnect;
    epics::pvData::Event waitForProcess;

    bool isDestroyed;
    epics::pvData::Status channelProcessConnectStatus;
    epics::pvData::Status channelProcessStatus;
    epics::pvAccess::ChannelProcess::shared_pointer channelProcess;

    ProcessConnectState connectState;

    enum ProcessState {processIdle,processActive,processComplete};
    ProcessState processState;
};

/**
 * @brief An easy to use alternative to ChannelGet.
 *
 * @author mrk
 */
class epicsShareClass PvaClientGet :
    public epics::pvAccess::ChannelGetRequester
{
public:
    POINTER_DEFINITIONS(PvaClientGet);
    /** Create a PvaClientGet.
     * @param &pvaClient Interface to PvaClient
     * @param channel Interface to Channel
     * @param pvRequest The request structure.
     * @return The interface to the PvaClientGet.
     */
    static PvaClientGetPtr create(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest
    );
    /** Destructor
     */
    ~PvaClientGet();
    std::string getRequesterName();
    void message(std::string const & message,epics::pvData::MessageType messageType);
    void channelGetConnect(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelGet::shared_pointer const & channelGet,
        epics::pvData::StructureConstPtr const & structure);
    void getDone(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelGet::shared_pointer const & channelGet,
        epics::pvData::PVStructurePtr const & pvStructure,
        epics::pvData::BitSetPtr const & bitSet);
    /** Call issueConnect and then waitConnect.
     * An exception is thrown if connect fails.
     * @throw runtime_error if failure.
     */
    void connect();
    /**
     * @brief create the channelGet connection to the channel.
     * This can only be called once.
     */
    void issueConnect();
    /** Wait until the channelGet connection to the channel is complete.
     * @return status;
     */
    epics::pvData::Status waitConnect();
    /** Call issueGet and then waitGet.
     * An exception is thrown if get fails.
     */
    void get();
    /** Issue a get and return immediately.
     */
    void issueGet();
    /** Wait until get completes.
     * @return status;
     */
    epics::pvData::Status waitGet();
    /**
     * @brief Get the data/
     * @return The interface.
     */
    PvaClientGetDataPtr getData();
    /** Deprecated method
     * \deprecated This method will go away in future versions. 
     */
    void destroy()  EPICS_DEPRECATED {}   
private:
    PvaClientGet(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest);

    void checkGetState();
    enum GetConnectState {connectIdle,connectActive,connected};

    PvaClient::weak_pointer pvaClient;
    epics::pvAccess::Channel::shared_pointer channel;
    epics::pvData::PVStructurePtr pvRequest;
    epics::pvData::Mutex mutex;
    epics::pvData::Event waitForConnect;
    epics::pvData::Event waitForGet;
    PvaClientGetDataPtr pvaClientData;
    std::string messagePrefix;

    bool isDestroyed;
    epics::pvData::Status channelGetConnectStatus;
    epics::pvData::Status channelGetStatus;
    epics::pvAccess::ChannelGet::shared_pointer channelGet;

    GetConnectState connectState;

    enum GetState {getIdle,getActive,getComplete};
    GetState getState;
    friend class ChannelGetRequesterImpl;
};

/**
 * @brief An easy to use alternative to ChannelPut.
 *
 * @author mrk
 */
class epicsShareClass PvaClientPut :
    public epics::pvAccess::ChannelPutRequester
{
public:
    POINTER_DEFINITIONS(PvaClientPut);
    /** Create a PvaClientPut.
     * @param &pvaClient Interface to PvaClient
     * @param channel Interface to Channel
     * @param pvRequest The request structure.
     * @return The interface to the PvaClientPut.
     */
    static PvaClientPutPtr create(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest
    );
    /** Destructor
     */
    ~PvaClientPut();
    std::string getRequesterName();
    void message(std::string const & message,epics::pvData::MessageType messageType);
    void channelPutConnect(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelPut::shared_pointer const & channelPut,
        epics::pvData::StructureConstPtr const & structure);
    void getDone(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelPut::shared_pointer const & channelPut,
        epics::pvData::PVStructurePtr const & pvStructure,
        epics::pvData::BitSetPtr const & bitSet);
    void putDone(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelPut::shared_pointer const & channelPut);
    /** Call issueConnect and then waitConnect.
     * An exception is thrown if connect fails.
     * @throw runtime_error if failure.
     */
    void connect();
    /** Issue the channelPut connection to the channel.
     * This can only be called once.
     */
    void issueConnect();
    /** Wait until the channelPut connection to the channel is complete.
     * @return status;
     */
    epics::pvData::Status waitConnect();
    /** Call issueGet and then waitGet.
     * An exception is thrown if get fails.
     * @throw runtime_error if failure.
     */
    void get();
    /** Issue a get and return immediately.
     */
    void issueGet();
    /** Wait until get completes.
     * @return status
     */
    epics::pvData::Status waitGet();
    /** Call issuePut and then waitPut.
     * An exception is thrown if get fails.
     */
    void put();
    /** Issue a put and return immediately.
     */
    void issuePut();
    /** Wait until put completes.
     * @return status
     */
    epics::pvData::Status waitPut();
    /**
     * @brief Get the data/
     * @return The interface.
     */
    PvaClientPutDataPtr getData();
     /** Deprecated method
     * \deprecated This method will go away in future versions. 
     */
    void destroy()  EPICS_DEPRECATED {}   
private :
    PvaClientPut(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest);
    
    void checkPutState();
    enum PutConnectState {connectIdle,connectActive,connected};

    PvaClient::weak_pointer pvaClient;
    epics::pvAccess::Channel::shared_pointer channel;
    epics::pvData::PVStructurePtr pvRequest;
    epics::pvData::Mutex mutex;
    epics::pvData::Event waitForConnect;
    epics::pvData::Event waitForGetPut;
    PvaClientPutDataPtr pvaClientData;

    bool isDestroyed;
    epics::pvData::Status channelPutConnectStatus;
    epics::pvData::Status channelGetPutStatus;
    epics::pvAccess::ChannelPut::shared_pointer channelPut;
    PutConnectState connectState;

    enum PutState {putIdle,getActive,putActive};
    PutState putState;
    friend class ChannelPutRequesterImpl;
};

/** 
 * @brief An easy to use alternative to ChannelPutGet.
 *
 * @author mrk
 */
class epicsShareClass PvaClientPutGet :
    public epics::pvAccess::ChannelPutGetRequester
{
public:
    POINTER_DEFINITIONS(PvaClientPutGet);
    /** Create a PvaClientPutGet.
     * @param &pvaClient Interface to PvaClient
     * @param channel Interface to Channel
     * @param pvRequest The request structure.
     * @return The interface to the PvaClientPutGet.
     */
    static PvaClientPutGetPtr create(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest
    );
    /** Destructor
     */
    ~PvaClientPutGet();
    std::string getRequesterName();
    void message(std::string const & message,epics::pvData::MessageType messageType);
    void channelPutGetConnect(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelPutGet::shared_pointer const & channelPutGet,
        epics::pvData::StructureConstPtr const & putStructure,
        epics::pvData::StructureConstPtr const & getStructure);
    void putGetDone(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelPutGet::shared_pointer const & channelPutGet,
        epics::pvData::PVStructurePtr const & getPVStructure,
        epics::pvData::BitSetPtr const & getBitSet);
    void getPutDone(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelPutGet::shared_pointer const & channelPutGet,
        epics::pvData::PVStructurePtr const & putPVStructure,
        epics::pvData::BitSet::shared_pointer const & putBitSet);
    void getGetDone(
        const epics::pvData::Status& status,
        epics::pvAccess::ChannelPutGet::shared_pointer const & channelPutGet,
        epics::pvData::PVStructurePtr const & getPVStructure,
        epics::pvData::BitSet::shared_pointer const & getBitSet);
    /** Call issueConnect and then waitConnect.
     * An exception is thrown if connect fails.
     * @throw runtime_error if failure.
     */
    void connect();
    /** Issue the channelPutGet connection to the channel.
     * This can only be called once.
     * An exception is thrown if connect fails.
     */
    void issueConnect();
    /** Wait until the channelPutGet connection to the channel is complete.
     * @return status;
     */
    epics::pvData::Status waitConnect();
    /** Call issuePutGet and then waitPutGet.
     * An exception is thrown if putGet fails.
     */
    void putGet();
    /** Issue a putGet and return immediately.
     */
    void issuePutGet();
    /** Wait until putGet completes.
     * If failure getStatus can be called to get reason.
     * @return status
     */
    epics::pvData::Status waitPutGet();
    /** Call issueGet and then waitGetGet.
     * An exception is thrown if get fails.
     */
    void getGet();
    /** Issue a getGet and return immediately.
     */
    void issueGetGet();
    /** Wait until getGet completes.
     * If failure getStatus can be called to get reason.
     * @return status
     */
    epics::pvData::Status waitGetGet();
    /** Call issuePut and then waitGetPut.
     * An exception is thrown if getPut fails.
     */
    void getPut();
    /** Issue a getPut and return immediately.
     */
    void issueGetPut();
    /** Wait until getPut completes.
     * @return status
     */
    epics::pvData::Status waitGetPut();
    /** Get the put data.
     * @return The interface.
     */
    PvaClientPutDataPtr getPutData();   
    /** Get the get data.
     * @return The interface.
     */
    PvaClientGetDataPtr getGetData(); 
    /** Deprecated method
     * \deprecated This method will go away in future versions. 
     */
    void destroy()  EPICS_DEPRECATED {}  
private :
    PvaClientPutGet(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest);
    void checkPutGetState();
    enum PutGetConnectState {connectIdle,connectActive,connected};

    PvaClient::weak_pointer pvaClient;
    epics::pvAccess::Channel::shared_pointer channel;
    epics::pvData::PVStructurePtr pvRequest;
    epics::pvData::Mutex mutex;
    epics::pvData::Event waitForConnect;
    epics::pvData::Event waitForPutGet;
    PvaClientGetDataPtr pvaClientGetData;
    PvaClientPutDataPtr pvaClientPutData;

    bool isDestroyed;
    epics::pvData::Status channelPutGetConnectStatus;
    epics::pvData::Status channelPutGetStatus;
    epics::pvAccess::ChannelPutGet::shared_pointer channelPutGet;
    PutGetConnectState connectState;

    enum PutGetState {putGetIdle,putGetActive,putGetComplete};
    PutGetState putGetState;
    friend class ChannelPutGetRequesterImpl;
};

//class ChannelMonitorRequester; // private to PvaClientMonitor
/**
 * @brief Optional client callback.
 *
 */
class epicsShareClass PvaClientMonitorRequester
{
public:
    POINTER_DEFINITIONS(PvaClientMonitorRequester);
    virtual ~PvaClientMonitorRequester() {}
    /** A monitor event has occurred.
     * @param monitor The PvaClientMonitor that received the event.
     */
    virtual void event(PvaClientMonitorPtr const & monitor) = 0;
};

/**
 * @brief An easy to use alternative to Monitor.
 *
 */
class epicsShareClass PvaClientMonitor :
    public epics::pvData::MonitorRequester,
    public std::tr1::enable_shared_from_this<PvaClientMonitor>
{
public:
    POINTER_DEFINITIONS(PvaClientMonitor);
    /** Create a PvaClientMonitor.
     * @param &pvaClient Interface to PvaClient
     * @param channel Interface to Channel
     * @param pvRequest The request structure.
     * @return The interface to the PvaClientMonitor.
     */
    static PvaClientMonitorPtr create(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest
    );
    /** Destructor
     */
    ~PvaClientMonitor();
    /** epics::pvData::MonitorRequester method
     * The requester must have a name.
     * @return The requester's name.
     */
    virtual std::string getRequesterName();
    /**  epics::pvData::MonitorRequester method
     * A message for the requester.
     * @param message The message.
     * @param messageType The type of message:
     */
    virtual void message(std::string const & message,epics::pvData::MessageType messageType);
    /**  epics::pvData::MonitorRequester method
     * @param status Completion status.
     * @param monitor The monitor
     * @param structure The structure defining the data.
     */
    virtual void monitorConnect(
        const epics::pvData::Status& status,
        epics::pvData::MonitorPtr const & monitor,
        epics::pvData::StructureConstPtr const & structure);
    /**  epics::pvData::MonitorRequester method
     * The client and server have both completed the createMonitor request.
     * The data source is no longer available.
     * @param monitor The monitor.
     */
    virtual void unlisten(epics::pvData::MonitorPtr const & monitor);
    /** epics::pvData::MonitorRequester method
     * A monitor event has occurred.
     * The requester must call Monitor.poll to get data.
     * @param monitor The monitor.
     */
    virtual void monitorEvent(epics::pvData::MonitorPtr const & monitor);
    /** Call issueConnect and then waitConnect.
     * An exception is thrown if connect fails.
     */
    void connect();
    /** Issue the channelMonitor connection to the channel.
     * This can only be called once.
     * An exception is thrown if connect fails.
     * @throw runtime_error if failure.
     */
    void issueConnect();
    /** Wait until the channelMonitor connection to the channel is complete.
     * @return status;
     */
    epics::pvData::Status waitConnect();
    /** Set a user callback.
     * @param pvaClientMonitorrRequester The requester which must be implemented by the caller.
     */
    void setRequester(PvaClientMonitorRequesterPtr const & pvaClientMonitorrRequester);
    /** Start monitoring.
     */
    void start();
    /** Stop monitoring.
     */
    void stop();
    /** Poll for a monitor event.
     * The data will be in PvaClientData.
     * @return (false,true) means event (did not, did) occur.
     */
    bool poll();
    /** Wait for a monitor event.
     * The data will be in PvaClientData.
     * @param secondsToWait Time to wait for event.
     * @return (false,true) means event (did not, did) occur.
     */
    bool waitEvent(double secondsToWait = 0.0);
    /** Release the monitorElement returned by poll
     */
    void releaseEvent();
    /** The data in which monitor events are placed.
     * @return The interface.
     */
    PvaClientMonitorDataPtr getData();   
    /** Get shared pointer to this
     */
    PvaClientMonitorPtr getPtrSelf()
    {
        return shared_from_this();
    }
    /** Deprecated method
     * \deprecated This method will go away in future versions. 
     */
    void destroy()  EPICS_DEPRECATED {}
private:
    PvaClientMonitor(
        PvaClientPtr const &pvaClient,
        epics::pvAccess::Channel::shared_pointer const & channel,
        epics::pvData::PVStructurePtr const &pvRequest);

    void checkMonitorState();
    enum MonitorConnectState {connectIdle,connectActive,connected,monitorStarted};

    PvaClient::weak_pointer pvaClient;
    epics::pvAccess::Channel::shared_pointer channel;
    epics::pvData::PVStructurePtr pvRequest;
    epics::pvData::Mutex mutex;
    epics::pvData::Event waitForConnect;
    epics::pvData::Event waitForEvent;
    PvaClientMonitorDataPtr pvaClientData;

    bool isDestroyed;
    epics::pvData::Status connectStatus;
    epics::pvData::MonitorPtr monitor;
    epics::pvData::MonitorElementPtr monitorElement;
    PvaClientMonitorRequesterWPtr pvaClientMonitorRequester;

    MonitorConnectState connectState;
    bool userPoll;
    bool userWait;
    friend class ChannelMonitorRequester;
};

}}

#endif  /* PVACLIENT_H */

/** @page Overview Documentation
 *
 * <a href = "../pvaClientCPP.html">pvaClientCPP.html</a>
 *
 */

