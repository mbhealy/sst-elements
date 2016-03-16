// Copyright 2013-2015 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2013-2015, Sandia Corporation
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#include <sst_config.h>

#include <string.h>
#include <sstream>

#include <sst/core/link.h>

#include "functionSM.h"
#include "ctrlMsg.h"

using namespace SST::Firefly;

const char* FunctionSM::m_functionName[] = {
    FOREACH_FUNCTION(GENERATE_STRING)
};

class DriverEvent : public SST::Event {
  public:
    DriverEvent( MP::Functor* _retFunc, int _retval ) :
        Event(),
        retFunc( _retFunc ),
        retval( _retval )
    { }
    MP::Functor* retFunc;
    int retval;
  private:
};

FunctionSM::FunctionSM( SST::Params& params, SST::Component* obj,
        ProtocolAPI* proto ) :
    m_sm( NULL ),
    m_params( params ),
    m_owner( obj ),
    m_proto( proto )
{

    m_dbg.init("@t:FunctionSM::@p():@l ", 
            params.find_integer("verboseLevel",0),
            0,
            Output::STDOUT );

    m_toDriverLink = obj->configureSelfLink("ToDriver", "1 ps",
        new Event::Handler<FunctionSM>(this,&FunctionSM::handleToDriver));

    m_fromDriverLink = obj->configureSelfLink("FromDriver", "1 ps",
        new Event::Handler<FunctionSM>(this,&FunctionSM::handleStartEvent));
    assert( m_fromDriverLink );

    m_toMeLink = obj->configureSelfLink("ToMe", "1 ns",
        new Event::Handler<FunctionSM>(this,&FunctionSM::handleEnterEvent));
    assert( m_toMeLink );

    m_proto->setRetLink( m_toMeLink );
}

FunctionSM::~FunctionSM()
{
    m_dbg.verbose(CALL_INFO,1,0,"\n");
    for ( unsigned int i=0; i < m_smV.size(); i++ ) {
        delete m_smV[i];
    }
}

void FunctionSM::printStatus( Output& out )
{
    m_sm->printStatus(out); 
}

void FunctionSM::setup( Info* info )
{
    char buffer[100];
    int nodeId =  info->worldRank();
    snprintf(buffer,100,"@t:%d:FunctionSM::@p():@l ", nodeId );
    m_dbg.setPrefix(buffer);

    m_smV.resize( NumFunctions );

    Params defaultParams;
    defaultParams.enableVerify(false);
    defaultParams[ "module" ] = m_params.find_string("defaultModule","firefly");
    defaultParams[ "enterLatency" ] = 
                        m_params.find_string("defaultEnterLatency","0");
    defaultParams[ "returnLatency" ] = 
                        m_params.find_string("defaultReturnLatency","0");
    defaultParams[ "verboseLevel" ] = m_params.find_string("verboseLevel","0"); 
    std::ostringstream tmp;
    tmp <<  nodeId; 
    defaultParams[ "nodeId" ] = tmp.str();

    for ( int i = 0; i < NumFunctions; i++ ) {
        std::string name = functionName( (FunctionEnum) i );
        Params tmp = m_params.find_prefix_params( name + "." );  
        defaultParams[ "name" ] = name;
        initFunction( m_owner, info, (FunctionEnum) i,
                                        name, defaultParams, tmp ); 
    }
}

void FunctionSM::initFunction( SST::Component* obj, Info* info,
    FunctionEnum num, std::string name, Params& defaultParams, Params& params)
{
    std::string module = params.find_string("module"); 
    if ( module.empty() ) {
        module = defaultParams["module"];
    }

    m_dbg.verbose(CALL_INFO,3,0,"func=`%s` module=`%s`\n",
                            name.c_str(),module.c_str());

    if ( params.find_string("name").empty() ) {
        params["name"] = defaultParams[ "name" ];
    }

    if ( params.find_string("verboseLevel").empty() ) {
        params["verboseLevel"] = defaultParams[ "verboseLevel" ];
    }

    if ( params.find_string("enterLatency").empty() ) {
        params["enterLatency"] = defaultParams[ "enterLatency" ];
    }

    if ( params.find_string("returnLatency").empty() ) {
        params["returnLatency"] = defaultParams[ "returnLatency" ];
    }

    params["nodeId"] = defaultParams[ "nodeId" ];

    m_smV[ num ] = (FunctionSMInterface*)obj->loadModule( module + "." + name,
                             params );

    assert( m_smV[ Init ] );
    m_smV[ num ]->setInfo( info ); 

    if ( ! m_smV[ num ]->protocolName().empty() ) {
        m_smV[ num ]->setProtocol( m_proto ); 
    }
}

void FunctionSM::enter( )
{
    m_dbg.verbose(CALL_INFO,3,0,"%s\n",m_sm->name().c_str());
    m_toMeLink->send( NULL );
}

void FunctionSM::start( int type, MP::Functor* retFunc, SST::Event* e )
{
    assert( e );
    m_retFunc = retFunc;
    assert( ! m_sm );
    m_sm = m_smV[ type ];
    m_dbg.verbose(CALL_INFO,3,0,"%s enter\n",m_sm->name().c_str());
    m_fromDriverLink->send( m_sm->enterLatency(), e );
}

void FunctionSM::handleStartEvent( SST::Event* e )
{
    Retval retval;
    assert( e );
    assert( m_sm );
    m_dbg.verbose(CALL_INFO,3,0,"%s\n",m_sm->name().c_str());
    m_sm->handleStartEvent( e, retval );
    processRetval( retval );
}


void FunctionSM::handleEnterEvent( SST::Event* e )
{
    assert( m_sm );
    m_dbg.verbose(CALL_INFO,3,0,"%s\n",m_sm->name().c_str());
    Retval retval;
    m_sm->handleEnterEvent( retval );
    processRetval( retval );
}

void FunctionSM::processRetval(  Retval& retval )
{
    if ( retval.isExit() ) {
        m_dbg.verbose(CALL_INFO,3,0,"Exit %" PRIu64 "\n", retval.value() );
        DriverEvent* x = new DriverEvent( m_retFunc, retval.value() );
        m_toDriverLink->send( m_sm->returnLatency(), x ); 
    } else if ( retval.isDelay() ) {
        m_dbg.verbose(CALL_INFO,3,0,"Delay %" PRIu64 "\n", retval.value() );
        m_toMeLink->send( retval.value(), NULL ); 
    } else {
    }
}

void FunctionSM::handleToDriver( Event* e )
{
    assert( e );
    m_dbg.verbose(CALL_INFO,3,0," returning\n");
    DriverEvent* event = static_cast<DriverEvent*>(e);
    if ( (*event->retFunc)( event->retval ) ) {
        delete event->retFunc;
    }    
    m_sm = NULL;
    delete e;
}
