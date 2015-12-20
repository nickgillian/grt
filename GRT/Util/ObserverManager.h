/*
GRT MIT License
Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GRT_OBSERVER_MANAGER_HEADER
#define GRT_OBSERVER_MANAGER_HEADER

#include "Observer.h"
#include "../DataStructures/Vector.h"
//#include <iostream>
//#include <vector>
//#include <list>
//#include <algorithm>

GRT_BEGIN_NAMESPACE
    
template <class NotifyType>
class ObserverManager {
public:
    ObserverManager(){
    }
    
    ObserverManager(const ObserverManager &rhs){
        *this = rhs;
    }
    
    ~ObserverManager(){}
    
    ObserverManager& operator=(const ObserverManager &rhs){
        if( this != &rhs ){
            removeAllObservers();
            for(size_t i=0; i<rhs.observers.size(); i++){
                observers.push_back( rhs.observers[i] );
            }
        }
        return *this;
    }
    
    bool registerObserver( Observer<NotifyType> &newObserver ){
        
        //Check to make sure we have not registered this observer already
        const size_t numObservers = observers.size();
        for(size_t i=0; i<numObservers; i++){
            Observer<NotifyType> *ptr = observers[i];
            if( ptr == &newObserver ){
                return false;
            }
        }
        
        //If we get this far then we can register the observer
        observers.push_back( &newObserver );
        return true;
    }
    
    bool removeObserver( const Observer<NotifyType> &oldObserver ){
        
        //Find the old observer and remove it from the observers list
        const size_t numObservers = observers.size();
        for(size_t i=0; i<numObservers; i++){
            const Observer<NotifyType> *ptr = observers[i];
            if( ptr == &oldObserver ){
                observers.erase( observers.begin() + i );
                return true;
            }
        }
        return false;
    }
    
    bool removeAllObservers(){
        observers.clear();
        return true;
    }
    
    bool notifyObservers(const NotifyType &data){
        
        //Notify all the observers
        const size_t numObservers = observers.size();
        for(size_t i=0; i<numObservers; i++){
            Observer<NotifyType> *ptr = observers[i];
            if( ptr != NULL )
                ptr->notify( data );
        }
        
        return true;
    }
    
protected:
    
    Vector< Observer<NotifyType>* > observers;
    
};
    
GRT_END_NAMESPACE

#endif //End of 
