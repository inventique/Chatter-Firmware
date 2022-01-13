#ifndef CHATTER_FIRMWARE_LVSCREEN_H
#define CHATTER_FIRMWARE_LVSCREEN_H

#include <lvgl.h>
#include "LVObject.h"

class LVScreen : public LVObject {
public:
	LVScreen();
	virtual ~LVScreen();

	virtual void onStart(){};
	virtual void onStop(){};

	void start();
	void stop();

	lv_group_t* getInputGroup();
	void onLvScreenDelete();


protected:
	lv_group_t* inputGroup;

};


#endif //CHATTER_FIRMWARE_LVSCREEN_H