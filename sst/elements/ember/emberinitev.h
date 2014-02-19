
#ifndef _H_EMBER_INIT_EVENT
#define _H_EMBER_INIT_EVENT

#include "emberevent.h"

namespace SST {
namespace Ember {

class EmberInitEvent : public EmberEvent {

public:
	EmberInitEvent();
	~EmberInitEvent();
	EmberEventType getEventType();
	std::string getPrintableString();

};

}
}

#endif
