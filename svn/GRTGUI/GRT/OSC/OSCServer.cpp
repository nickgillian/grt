#include "OSCServer.h"

OSCServer::OSCServer(unsigned int listenerPort,bool verbose){
    this->listenerPort = listenerPort;
    this->verbose = verbose;
    serverRunning = false;
    oscSocket = NULL;
}

OSCServer::~OSCServer(){
    if( getServerRunning() ){
        stop();
    }
}

bool OSCServer::start(){

    if( getServerRunning() ){
        if( verbose )
            qDebug() << STRING_TO_QSTRING( "WARNING: OSCServer::start() - The OSC server is already running!" );
        return false;
    }

    if( verbose )
        qDebug() << STRING_TO_QSTRING("OSCServer::start() - Starting OSC server...");

    try{
        serverThread = new boost::thread( boost::bind( &OSCServer::mainThread, this) );
    }catch( std::exception const &error ){
        QString qstr = "ERROR: OSCServer::start() - Failed to start server thread! Exception: ";
        qstr += error.what();
        qstr += "\n";
        qDebug() << qstr;
        return false;
    }

    return true;
}

bool OSCServer::stop(){

    if( !getServerRunning() ){
        if( verbose )
            qDebug() << "WARNING: OSCServer::stop() - The OSC server is not running!";
        return false;
    }

    //Stop the OSC server
    oscSocket->AsynchronousBreak();

    //Wait for the server to stop
    serverThread->join();

    //Clean up the OSC socket
    delete oscSocket;
    oscSocket = NULL;

    //Clear up any unused messages
    clearMessages();

    return true;
}

bool OSCServer::getServerRunning(){
    boost::mutex::scoped_lock lock( mutex );
    return serverRunning;
}

bool OSCServer::getNewMessagesReceived(){
    boost::mutex::scoped_lock lock( mutex );
    return (messages.size()>0);
}

unsigned int OSCServer::getNumMessages(){
    boost::mutex::scoped_lock lock( mutex );
    return (unsigned int)messages.size();
}

OSCMessage OSCServer::getNextMessage(){
    boost::mutex::scoped_lock lock( mutex );
    OSCMessage m;
    if( messages.size() > 0 ){
        m = messages.front();
        messages.pop();
    }
    return m;
}

bool OSCServer::setVerbose(bool verbose){
    if( getServerRunning() ) return false;
    this->verbose = verbose;
    return true;
}

bool OSCServer::setIncomingDataPort(unsigned int listenerPort){
    if( getServerRunning() ) return false;
    this->listenerPort = listenerPort;
    return true;
}

void OSCServer::mainThread(){

    //Flag that the server is running
    {
        boost::mutex::scoped_lock lock( mutex );
        serverRunning = true;
    }

    //Start the OSC server
    try{
        oscSocket = new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS, listenerPort), this );

        //This will run forever, until the AsynchronousBreak() function is called by the stop() method
        oscSocket->Run();

    }catch( std::exception const &error ){
        QString qstr = "ERROR: OSCManager::mainThread() - Exception: ";
        qstr += error.what();
        qDebug() << qstr;
        boost::mutex::scoped_lock lock( mutex );
        serverRunning = false;
        return;
    }

    //Flag that the server has stopped
    {
        boost::mutex::scoped_lock lock( mutex );
        serverRunning = false;
    }

}

void OSCServer::ProcessMessage(const osc::ReceivedMessage &m, const IpEndpointName &remoteEndpoint ){

    //Get the address of the host that sent the message
    char addressString[ IpEndpointName::ADDRESS_STRING_LENGTH ];
    remoteEndpoint.AddressAndPortAsString( addressString );
    std::string hostAddress = addressString;

    if( verbose ){
        QString info = "Got OSC message ";
        info += m.AddressPattern();
        info += " from ";
        info += hostAddress.c_str();
        qDebug() << info;
    }

    OSCMessage newMessage( hostAddress, m );

    boost::mutex::scoped_lock lock( mutex );

    messages.push( newMessage );
}

bool OSCServer::clearMessages(){
    while( !messages.empty() ){
        messages.pop();
    }
    return true;
}
