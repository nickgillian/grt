#pragma once

#include "oscpack/include/osc/OscOutboundPacketStream.h"
#include "oscpack/include/ip/UdpSocket.h"
#include "oscpack/include/osc/OscReceivedElements.h"
#include "oscpack/include/osc/OscPacketListener.h"
#include "oscpack/include/osc/OscHostEndianness.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

typedef unsigned char OSC_BYTE;

class OSCArgument{
public:
    OSCArgument(){
        argumentType = NOT_SET;
        argString = "";
        argInt = 0;
        argFloat = 0;
        argDouble = 0;
        argBlobData = NULL;
        argBlobDataSize = 0;
    }

    OSCArgument(const OSCArgument &rhs):argumentType(NOT_SET),argBlobData(NULL){
        *this = rhs;
    }

    ~OSCArgument(){
        if( argBlobData !=  NULL ){
            this->clearBlobData();
        }
    }

    OSCArgument& operator=(const OSCArgument &rhs){
        if( this != &rhs ){
            this->argumentType = rhs.argumentType;
            this->argString = rhs.argString;
            this->argInt = rhs.argInt;
            this->argFloat = rhs.argFloat;
            this->argDouble = rhs.argDouble;
            this->clearBlobData();
            if( rhs.argBlobData != NULL ){
                this->argBlobDataSize = rhs.argBlobDataSize;
                this->argBlobData = new OSC_BYTE[ this->argBlobDataSize ];
                for(unsigned long i=0; i<this->argBlobDataSize; i++){
                     this->argBlobData[i] = rhs.argBlobData[i];
                }
            }
        }
        return *this;
    }

    bool clearBlobData(){
        if( argBlobData != NULL ){
            delete[] argBlobData;
            argBlobData = NULL;
        }
        argBlobDataSize = 0;
        return true;
    }

    bool getIsString() const{
        return argumentType == ARG_STRING;
    }

    bool getIsBlob() const{
        return argumentType == ARG_BLOB;
    }

    bool getIsInt() const{
        return argumentType == ARG_INT;
    }

    bool getIsFloat() const{
        return argumentType == ARG_FLOAT;
    }

    bool getIsDouble() const{
        return argumentType == ARG_DOUBLE;
    }

    std::string getString() const{
        return argString;
    }

    const unsigned char* getBlobData() const{
        return argBlobData;
    }

    unsigned long getBlobDataSize() const{
        return argBlobDataSize;
    }

    signed int getInt() const{
        switch( argumentType ){
            case ARG_INT:
                return argInt;
            break;
            case ARG_FLOAT:
                return (signed int)argFloat;
            break;
            case ARG_DOUBLE:
                return (signed int)argDouble;
            break;
        }

        return argInt;
    }

    float getFloat() const{
        switch( argumentType ){
            case ARG_INT:
                return (float)argInt;
            break;
            case ARG_FLOAT:
                return argFloat;
            break;
            case ARG_DOUBLE:
                return (float)argDouble;
            break;
        }

        return argInt;
    }

    double getDouble() const{
        switch( argumentType ){
            case ARG_INT:
                return (double)argInt;
            break;
            case ARG_FLOAT:
                return (double)argFloat;
            break;
            case ARG_DOUBLE:
                return argDouble;
            break;
        }

        return argInt;
    }

    bool setString(std::string argString){
        this->clearBlobData();
        this->argString = argString;
        argumentType = ARG_STRING;
        return true;
    }

    bool setBlob(const osc::ReceivedMessage::const_iterator &arg){
        this->clearBlobData();
	    
        //Get a pointer to the raw blob data and the size of the raw data
        const void *rawData = NULL;
        unsigned long rawDataSize = 0;
        arg->AsBlobUnchecked( rawData, rawDataSize );

        if( rawData == NULL ){
            printf("WARNING: Failed to get raw blob data!\n");
            return false;
        }
	
        //Set the blob data size and allocate some memory to hold the blob data
        argBlobDataSize = rawDataSize;
        argBlobData = new OSC_BYTE[ argBlobDataSize ];
	
        if( argBlobData == NULL ){
            printf("WARNING: Failed to allocate data for blob!\n");
            return false;
        }
	
        //Copy the raw data to the argBlobData
        OSC_BYTE *rawPtr = (OSC_BYTE*)rawData;
        for(unsigned long i=0; i<argBlobDataSize; i++){
            argBlobData[ i ] = rawPtr[i];
        }

        //printf("Raw Data Size: %i\n",argBlobDataSize);

        argumentType = ARG_BLOB;
        return true;
    }

    bool setInt(signed int argInt){
        this->clearBlobData();
        this->argInt = argInt;
        argumentType = ARG_INT;
        return true;
    }

    bool setFloat(float argFloat){
        this->clearBlobData();
        this->argFloat = argFloat;
        argumentType = ARG_FLOAT;
        return true;
    }

    bool setDouble(double argDouble){
        this->clearBlobData();
        this->argDouble = argDouble;
        argumentType = ARG_DOUBLE;
        return true;
    }

protected:
    int argumentType;
    std::string argString;
    OSC_BYTE* argBlobData;
    unsigned long argBlobDataSize;
    signed int argInt;
    float argFloat;
    double argDouble;

public:

    enum ArgumentTypes{NOT_SET=0,ARG_STRING,ARG_BLOB,ARG_INT,ARG_FLOAT,ARG_DOUBLE};
};

class OSCMessage{
public:
    OSCMessage(){
        this->senderAddress = "";
        this->addressPattern = "";
    }
    OSCMessage(std::string senderAddress,const osc::ReceivedMessage &message){

        try{
            this->senderAddress = senderAddress;
            this->addressPattern = message.AddressPattern();
            this->messageArguments.clear();

            unsigned long numArgs = message.ArgumentCount();

            if( numArgs > 0 ){
                std::string typeTags = message.TypeTags();
                osc::ReceivedMessage::const_iterator arg = message.ArgumentsBegin();
                messageArguments.resize( numArgs );

                for(unsigned long i=0; i<numArgs; i++){
                    switch( typeTags[i] ){
                        case 'b':
                            messageArguments[i].setBlob( arg );
                            arg++;
                        break;
                        case 'i':
                            messageArguments[i].setInt( arg->AsInt32() );
                            arg++;
                            //printf("Int32: %i\n",messageArguments[i].getInt());
                        break;
                        case 's':
                            messageArguments[i].setString( arg->AsString() );
                            arg++;
                            //printf("String: %s\n",messageArguments[i].getString().c_str());
                        break;
                        case 'f':
                            messageArguments[i].setFloat( arg->AsFloat() );
                            arg++;
                            //printf("Float: %f\n",messageArguments[i].getFloat());
                        break;
                        case 'd':
                            messageArguments[i].setDouble( arg->AsDouble() );
                            arg++;
                            //printf("Double: %f\n",messageArguments[i].getDouble());
                        break;
                        default:
                            printf("WARNING: Unknown OSC message type! type: %i\n",typeTags[i]);
                            arg++;
                            break;
                    }
                }

                //printf("Message Address: %s Num Args: %lu TypeTags: %s\n",addressPattern.c_str(),numArgs,typeTags.c_str());
            }

        }catch( const osc::Exception &e ){
            //Any parsing errors such as unexpected argument types, or missing arguments get thrown as exceptions.
            std::cout << "ERROR: Error while parsing message: " << message.AddressPattern() << ": " << e.what() << std::endl;
        }
    }

    OSCMessage(const OSCMessage &rhs){
        *this = rhs;
    }

    ~OSCMessage(){}

    OSCMessage& operator=(const OSCMessage &rhs){
        if( this != &rhs ){
            this->senderAddress = rhs.senderAddress;
            this->addressPattern = rhs.addressPattern;
            this->messageArguments = rhs.messageArguments;
        }
        return *this;
    }

    OSCArgument& operator[](const unsigned int &i){
        return messageArguments[i];
    }

    const OSCArgument& operator[](const unsigned int &i) const{
        return messageArguments[i];
    }

    std::string getSenderAddress() const { return senderAddress; }
    std::string getAddressPattern() const{ return addressPattern; }
    unsigned int getNumArgs() const { return (unsigned int)messageArguments.size(); }

protected:
    std::string senderAddress;
    std::string addressPattern;
    std::vector< OSCArgument > messageArguments;
};
