#include "MainMenu.h"
#include "Model/Convo.hpp"
#include <Input/Input.h>
#include <Pins.hpp>
#include "Services/LoRaService.h"
#include "Elements/BatteryElement.h"
#include "Screens/FriendsScreen.h"
#include "Screens/InboxScreen.h"

const MainMenu::Item MainMenu::Items[] = {
		{ "Inbox", "Inbox" },
		{ "Friends", "Friends" },
		{ "Profile", "Profile" },
		{ "Settings", "Settings" },
};

const uint8_t MainMenu::ItemCount = sizeof(Items) / sizeof(Items[0]);

MainMenu::MainMenu() : LVScreen(){
	left = lv_obj_create(obj);
	mid = lv_obj_create(obj);
	right = lv_obj_create(obj);

	lv_obj_set_layout(obj, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

	lv_obj_set_width(left, 22);
	lv_obj_set_width(right, 19);
	lv_obj_set_height(left, lv_pct(100));
	lv_obj_set_height(right, lv_pct(100));
	lv_obj_set_height(mid, lv_pct(100));
	lv_obj_set_flex_grow(mid, 1);
	lv_obj_set_scrollbar_mode(mid, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scrollbar_mode(right, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scrollbar_mode(left, LV_SCROLLBAR_MODE_OFF);

	lv_obj_set_flex_flow(left, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_flow(mid, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_flow(right, LV_FLEX_FLOW_COLUMN);

	lv_obj_set_flex_align(left, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
	lv_obj_set_flex_align(right, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_flex_align(mid, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	lv_obj_set_style_pad_ver(left, 2, 0);
	new BatteryElement(left);

	for(const auto& item : Items){
		lv_obj_t* bigContainer = lv_obj_create(mid);
		lv_obj_t* big = lv_gif_create(bigContainer);
		lv_obj_t* bigLabel = lv_img_create(bigContainer);
		lv_obj_t* small = lv_img_create(right);

		bigs.push_back(big);
		smalls.push_back(small);

		lv_gif_set_src(big, (String("S:/Menu/Big/") + item.icon + ".gif").c_str());
		lv_gif_set_loop(big, bigs.size() == 3 ? LV_GIF_LOOP_SINGLE : LV_GIF_LOOP_ON);

		lv_img_set_src(bigLabel, (String("S:/Menu/Label/") + item.icon + ".bin").c_str());
		lv_img_set_src(small, (String("S:/Menu/Small/") + item.icon + ".bin").c_str());

		lv_obj_set_flex_flow(bigContainer, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(bigContainer, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_width(bigContainer, lv_pct(100));
		lv_obj_set_height(bigContainer, lv_pct(100));

		//lv_obj_set_style_translate_y(big, -5, LV_PART_MAIN | LV_STATE_DEFAULT);
		//lv_obj_set_style_translate_y(bigLabel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_translate_y(bigContainer, -10, LV_PART_MAIN | LV_STATE_DEFAULT);
	}

	arrowUp = lv_img_create(obj);
	arrowDown = lv_img_create(obj);
	lv_img_set_src(arrowUp, "S:/Menu/ArrowUp.bin");
	lv_img_set_src(arrowDown, "S:/Menu/ArrowDown.bin");
	lv_obj_add_flag(arrowUp, LV_OBJ_FLAG_FLOATING);
	lv_obj_add_flag(arrowDown, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_align(arrowUp, LV_ALIGN_TOP_MID);
	lv_obj_set_align(arrowDown, LV_ALIGN_BOTTOM_MID);
	lv_obj_set_style_pad_top(arrowUp, 4, 0);
	lv_obj_set_style_pad_bottom(arrowDown, 4, 0);
}

void MainMenu::setupAnimations(){
	lv_anim_init(&arrowUpAnim);
	lv_anim_init(&arrowDownAnim);
	lv_anim_set_var(&arrowUpAnim, arrowUp);
	lv_anim_set_var(&arrowDownAnim, arrowDown);
	lv_anim_set_values(&arrowUpAnim, -5, 5);
	lv_anim_set_values(&arrowDownAnim, -5, 5);
	lv_anim_set_path_cb(&arrowUpAnim, lv_anim_path_ease_in_out);
	lv_anim_set_path_cb(&arrowDownAnim, lv_anim_path_ease_in_out);
	lv_anim_set_exec_cb(&arrowUpAnim, MainMenu::arrowFloat);
	lv_anim_set_exec_cb(&arrowDownAnim, MainMenu::arrowFloat);
	lv_anim_set_repeat_count(&arrowUpAnim, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_repeat_count(&arrowDownAnim, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_playback_time(&arrowUpAnim, 800);
	lv_anim_set_playback_time(&arrowDownAnim, 800);
	lv_anim_set_time(&arrowUpAnim, 800);
	lv_anim_set_time(&arrowDownAnim, 800);
	lv_anim_set_delay(&arrowDownAnim, 800);

	lv_anim_start(&arrowUpAnim);
	lv_anim_start(&arrowDownAnim);

	for(lv_obj_t* small : smalls){
		smallAnims.emplace_back();
		lv_anim_t& anim = smallAnims.back();
		lv_anim_init(&anim);
		lv_anim_set_exec_cb(&anim, ease);
		lv_anim_set_var(&anim, small);
		lv_anim_set_time(&anim, 800);
	}

	lv_anim_init(&arrowHideAnim1);
	lv_anim_set_exec_cb(&arrowHideAnim1, arrowHide);
	lv_anim_set_time(&arrowHideAnim1, 400);

	lv_anim_init(&arrowHideAnim2);
	lv_anim_set_exec_cb(&arrowHideAnim2, arrowHide);
	lv_anim_set_time(&arrowHideAnim2, 400);
}

void MainMenu::arrowHide(void* var, int32_t value){
	lv_obj_t* obj = (lv_obj_t*) var;
	lv_obj_set_y(obj, value);
}

void MainMenu::arrowFloat(void* var, int32_t value){
	lv_obj_t* obj = (lv_obj_t*) var;
	lv_obj_set_x(obj, value);
}

void IRAM_ATTR MainMenu::ease(void* var, int32_t value){
	lv_obj_t* obj = (lv_obj_t*) var;

	const float amount = 1.1f;
	float v = (float) value / 100.0f;

	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	const float x =  v < 0.5
			   ? (pow(2.0f * v, 2.0f) * ((c2 + 1.0f) * 2.0f * v - c2)) / 2.0f
			   : (pow(2.0f * v - 2.0f, 2.0f) * ((c2 + 1.0f) * (v * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;

	lv_obj_set_style_translate_x(obj, lv_pct(round(x * amount * 100.0f)), LV_STATE_DEFAULT | LV_PART_MAIN);
}

void MainMenu::startAnim(uint8_t index, bool reverse){
	if(index >= smallAnims.size()) return;

	lv_anim_t& anim = smallAnims[index];
	lv_anim_set_values(&anim, reverse * 100, !reverse * 100);
	lv_anim_start(&anim);

	if(!reverse){
		lv_obj_t* gif = bigs[index];
		lv_gif_restart(gif);
		lv_gif_start(gif);
	}
}

void MainMenu::onStart(){
	Input::getInstance()->addListener(this);

	for(lv_obj_t* small : smalls){
		lv_obj_set_style_translate_x(small, 0, LV_STATE_DEFAULT | LV_PART_MAIN);
	}

	setupAnimations();

	// TODO: add onStarting screen event function

	selected = 0;
	startAnim(0);
	lv_obj_scroll_to(mid, 0, 0, LV_ANIM_ON);

	lv_obj_set_y(arrowUp, -(lv_obj_get_height(arrowDown) + 2));
	if(ItemCount <= 1){
		lv_obj_set_y(arrowDown, lv_obj_get_height(arrowDown) + 2);
	}
}

void MainMenu::onStop(){
	Input::getInstance()->removeListener(this);

	for(int i = 0; i < ItemCount; i++){
		lv_gif_stop(bigs[i]);
		lv_anim_del(&smallAnims[i], nullptr);
	}

	lv_anim_del(&arrowUpAnim, nullptr);
	lv_anim_del(&arrowDownAnim, nullptr);

	lv_anim_del(&arrowHideAnim1, nullptr);
	lv_anim_del(&arrowHideAnim2, nullptr);
}

void MainMenu::buttonPressed(uint i){
	if(i == BTN_LEFT){
		selectPrev();
	}else if(i == BTN_RIGHT){
		selectNext();
	}else if(i == BTN_ENTER){
		LVScreen* (* screens[])() = {
				[]() -> LVScreen*{
					return new InboxScreen();
				},
				[]() -> LVScreen*{
					return new FriendsScreen();
				},
				[]() -> LVScreen*{ return nullptr; },
				[]() -> LVScreen*{ return nullptr; },
		};

		LVScreen* screen = screens[selected]();
		if(screen){
			push(screen);
		}
	}
}

void MainMenu::selectNext(){
	if(selected + 1 >= ItemCount) return;

	startAnim(selected, true);
	selected++;
	startAnim(selected);
	lv_obj_scroll_to(mid, 0, selected * lv_obj_get_height(mid), LV_ANIM_ON);

	if(selected + 1 == ItemCount){
		lv_anim_set_var(&arrowHideAnim1, arrowDown);
		lv_anim_set_values(&arrowHideAnim1, 0, lv_obj_get_height(arrowDown) + 2);
		lv_anim_start(&arrowHideAnim1);
	}

	if(selected == 1){
		lv_anim_set_var(&arrowHideAnim2, arrowUp);
		lv_anim_set_values(&arrowHideAnim2, -(lv_obj_get_height(arrowDown) + 2), 0);
		lv_anim_start(&arrowHideAnim2);
	}
}

void MainMenu::selectPrev(){
	if(selected == 0) return;

	startAnim(selected, true);
	selected--;
	startAnim(selected);
	lv_obj_scroll_to(mid, 0, selected * lv_obj_get_height(mid), LV_ANIM_ON);

	if(selected == 0){
		lv_anim_set_var(&arrowHideAnim1, arrowUp);
		lv_anim_set_values(&arrowHideAnim1, 0, -(lv_obj_get_height(arrowDown) + 2));
		lv_anim_start(&arrowHideAnim1);
	}

	if(selected + 2 == ItemCount){
		lv_anim_set_var(&arrowHideAnim2, arrowDown);
		lv_anim_set_values(&arrowHideAnim2, lv_obj_get_height(arrowDown) + 2, 0);
		lv_anim_start(&arrowHideAnim2);
	}
}
