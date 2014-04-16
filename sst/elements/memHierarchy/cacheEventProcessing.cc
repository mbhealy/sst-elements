// Copyright 2009-2014 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2014, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

/*
 * File:   cache.cc
 * Author: Caesar De la Paz III
 * Email:  caesar.sst@gmail.com
 */

#include <sst_config.h>
#include <sst/core/serialization.h>
#include "cacheController.h"
#include "coherenceControllers.h"
#include "hash.h"
#include <boost/lexical_cast.hpp>
#include <csignal>

#include "memEvent.h"
#include <sst/core/interfaces/stringEvent.h>


using namespace SST;
using namespace SST::MemHierarchy;

bool Cache::clockTick(Cycle_t time) {
    timestamp_++; topCC_->timestamp_++; bottomCC_->timestamp_++;
    
    if(dirControllerExists_) memNICIdle_ = directoryLink_->clock();
    /*if(memNICIdle_) memNICIdleCount_++;
    else memNICIdleCount_ = 0;*/
    
    topCC_->sendOutgoingCommands();
    bottomCC_->sendOutgoingCommands();
    
    if(UNLIKELY(!retryQueueNext_.empty() && timestamp_ % 20 == 0)){
        retryQueue_ = retryQueueNext_;
        retryQueueNext_.clear();
        for(vector<MemEvent*>::iterator it = retryQueue_.begin(); it != retryQueue_.end();){
            d_->debug(_L1_,"RETRYING EVENT\n");
            processEvent(*it, true);
            retryQueue_.erase(it);
        }
    }
    else if(!incomingEventQueue_.empty()){
        processEvent(incomingEventQueue_.front().first, false);
        incomingEventQueue_.pop();
        idleCount_ = 0;
    }
    else if(clockOn_ && dirControllerExists_ == false){
        ///std::cout << "idleCount " << idleCount_ << std::endl;
        idleCount_++;
    }
    
    
    if(idleCount_ > idleMax_ && dirControllerExists_ == false){
        //std::cout << "idleMax " << idleMax_ << " turning off clock" << std::endl;
        clockOn_ = false;
        idleCount_ = 0;
        return true;
    }
    
    return false;
}

void Cache::init(unsigned int phase){
    
    SST::Event *ev;
    if(directoryLink_) directoryLink_->init(phase);
    
    if(!phase){
        if(L1_) for(uint idc = 0; idc < highNetPorts_->size(); idc++) highNetPorts_->at(idc)->sendInitData(new Interfaces::StringEvent("SST::MemHierarchy::MemEvent"));
        else{
            for(uint i = 0; i < highNetPorts_->size(); i++) {
                highNetPorts_->at(i)->sendInitData(new MemEvent(this, 0, NULLCMD));
            }
        }
        if(!dirControllerExists_){
            for(uint i = 0; i < lowNetPorts_->size(); i++){
                lowNetPorts_->at(i)->sendInitData(new MemEvent(this, 10, NULLCMD));
            }
        }
        
    }

    //leave as is!!!!!!!!!!!!!
    for(uint idc = 0; idc < highNetPorts_->size(); idc++) {
        while ((ev = (highNetPorts_->at(idc))->recvInitData())){
            MemEvent* memEvent = dynamic_cast<MemEvent*>(ev);
            if(!memEvent) delete memEvent;
            else{
                if(dirControllerExists_) directoryLink_->sendInitData(new MemEvent(memEvent));
                else{
                    for(uint idp = 0; idp < lowNetPorts_->size(); idp++){
                        lowNetPorts_->at(idp)->sendInitData(new MemEvent(memEvent));
                    }
                }
            }
            delete memEvent;
        }
    }
    
    if(!dirControllerExists_){
        for(uint i = 0; i < lowNetPorts_->size(); i++) {
            while ((ev = lowNetPorts_->at(i)->recvInitData())){
                MemEvent* memEvent = dynamic_cast<MemEvent*>(ev);
                if(!memEvent) delete memEvent;
                else if(memEvent->getCmd() == NULLCMD){
                    nextLevelCacheName_ = memEvent->getSrc();
                }
                delete memEvent;
            }
        }
    }
}

void Cache::setup(){
    bottomCC_->setNextLevelCache(nextLevelCacheName_);
}

void Cache::processIncomingEvent(SST::Event *ev){
    incomingEventQueue_.push(make_pair(ev, timestamp_));
    //std::cout << "Received event" << std::endl;
    if(!clockOn_){
        //std::cout << "Turning on clock" << std::endl;
        timestamp_ = reregisterClock(defaultTimeBase_, clockHandler_);
        clockOn_ = true;
        memNICIdleCount_ = 0;
        idleCount_ = 0;
    }
}
  
void Cache::processEvent(SST::Event* ev, bool reActivation) {
    MemEvent *event = static_cast<MemEvent*>(ev); assert_msg(event, "Event is Null!!");
    assert(event);
    
    Command cmd     = event->getCmd(); string prefetch =  event->isPrefetch() ? "true" : "false";
    Addr addr       = event->getAddr(), baseAddr = toBaseAddr(addr);
    bool uncached   = event->queryFlag(MemEvent::F_UNCACHED);
        
    if(!reActivation){
        STAT_TotalInstructionsRecieved_++;
        d2_->debug(_L0_,"\n\n----------------------------------------------------------------------------------------\n");    //raise(SIGINT);
    }

    d_->debug(_L0_,"Incoming Event. Name: %s, Cmd: %s, Addr: %"PRIx64", BsAddr: %"PRIx64", Src: %s, Dst: %s, PreF:%s, time: %llu... %s \n", this->getName().c_str(), CommandString[event->getCmd()], addr, baseAddr, event->getSrc().c_str(), event->getDst().c_str(), prefetch.c_str(), timestamp_, uncached ? "un$" : "");
    if(uncached){
        processUncached(event, cmd, baseAddr);
        return;
    }
    
   // try{
    if(cmd <= 2) {                                                          /* GetS, GetX, GetSEx */
        if(cmd == GetSEx && !reActivation) STAT_GetSExReceived_++;
        if(!reActivation) STAT_NonCoherenceReqsReceived_++;
        if(mshr_->isHitAndStallNeeded(baseAddr, cmd)){
            mshr_->insert(baseAddr, event);
            d_->debug(_L1_,"Adding event to MSHR queue.  Wait till blocking event completes to proceed with this event.\n");
            return;
        }
        processAccess(event, cmd, baseAddr, reActivation);
    }
    else if(cmd <= 4)  processAccessAcknowledge(event, baseAddr);           /* GetSResp, GetXResp */
    else if(cmd <= 8)  processAccess(event, cmd, baseAddr, reActivation);   /* PutM, PutS, Put, PutXE */
    else if(cmd <= 10){                                                     /* Inv, InvX */
        mshr_->insert(baseAddr, event);
        processInvalidate(event, cmd, baseAddr, reActivation);
    }
    else if(cmd <= 14){   /* Fetch, FetchInvalidate, FetchInvalidateX */
        mshr_->insert(baseAddr, event);
        processFetch(event, baseAddr, reActivation);
    }
    else{
        _abort(MemHierarchy::Cache, "Command not supported, cmd = %s", CommandString[cmd]);
    }

    //catch(mshrException const& e){
    //    _abort(MemHierarchy::Cache, "Limited MSHR is not supported yet, increment the number of MSHR entries\n");
        //topCC_->sendNACK(event);
    //}
}

void Cache::processUncached(MemEvent* event, Command cmd, Addr baseAddr){
    vector<mshrType> mshrEntry;
    MemEvent* memEvent;
    int i = 0;
    switch( cmd ){
        case GetS:
        case GetX:
            if(mshrUncached_->isHitAndStallNeeded(baseAddr, cmd)){
                mshrUncached_->insert(baseAddr, event);
                return;
            }
            mshrUncached_->insert(baseAddr, event);
            if(cmd == GetS) bottomCC_->forwardMessage(event, baseAddr, lineSize_, NULL);
            else bottomCC_->forwardMessage(event, baseAddr, lineSize_, &event->getPayload());
            break;
        case GetSResp:
        case GetXResp:
            mshrEntry = mshrUncached_->removeAll(baseAddr);
            for(vector<mshrType>::iterator it = mshrEntry.begin(); it != mshrEntry.end(); i++){
                memEvent = boost::get<MemEvent*>(mshrEntry.front().elem);
                topCC_->sendResponse(memEvent, DUMMY, &event->getPayload());
                delete memEvent;
                mshrEntry.erase(it);
                
            }
            delete event;
            break;
        default:
            _abort(MemHierarchy::Cache, "Command does not exist. Command: %s, Src: %s\n", CommandString[cmd], event->getSrc().c_str());
            break;
    }
}


void Cache::handlePrefetchEvent(SST::Event *event) {
    selfLink_->send(1, event);
}

void Cache::handleSelfEvent(SST::Event *event){
    MemEvent* ev = static_cast<MemEvent*>(event);
    processEvent(event, ev->isPrefetch());
}

