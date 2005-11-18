#ifndef _REMOTEEVENT_H
#define _REMOTEEVENT_H

class cRemoteEvent
{
public:
	virtual void Flatten(void**, int *) = 0;
	virtual void DestroyFlatEvent(void *) = 0;
	virtual int EvtId() = 0;

	// A ctor taking an array of char must be defined to create a class from a
	// flatenned one.
	// cRemoteEvent(char*);
};

#endif
