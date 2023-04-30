
bool menuOpen = false;
int curOption = 0;
enum options {
	OPTION_GIVEMONEY,
	OPTION_XP,
	OPTION_GODMODEON,
	OPTION_GODMODEOFF,
	OPTION_SPAWNADDER,
	OPTION_AUTOSAVE,
	OPTION_OPTIONCOUNT
};
void goUp() {
	playSound("NAV_UP_DOWN");
	curOption--;
	if (curOption < 0) {
		curOption = OPTION_OPTIONCOUNT-1;
	}
}
void goDown() {
	playSound("NAV_UP_DOWN");
	curOption++;
	if (curOption >= OPTION_OPTIONCOUNT) {
		curOption = 0;
	}
}
bool isSelected(int i) {
	return i == curOption;
}
bool optionPressed(int i) {
	if (isSelected(i)) {
		if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, BUTTON_X)) {
			playSound("SELECT");
			return true;
		}
	}
	return false;
}
void drawMenuText(char *text, int index) {
	bool selected = isSelected(index);
	drawText(text,0,0.1,0.2+index*0.02,0.3,0.3,255,selected?0:255,selected?0:255,255,false,false);
}

void drawMenuTitle(char *text) {
	drawText(text,1,0.1,0.2-0.03,0.5,0.5,255,255,255,255,false,false);
}

void main_loop() {
	outputTestGlobalVals();
}

void menu_open_loop() {
	drawButtonTester();
}

void draw_menu() {
	main_loop();
	if (menuOpen) {
		exclusiveInputs();
		if (IsTimerReady(20)) {
			drawMenuTitle("~b~gopro_2027's Playground 1.12");
			drawRectXY(0.1,0.2,0.3,0.5,0,255,255,70);
			for (int i = 0; i < OPTION_OPTIONCOUNT; i++) {
			switch(i) {
			case OPTION_GIVEMONEY: {
				drawMenuText("Give Money",i);
				if (optionPressed(i)) {
					NETWORKCASH::NETWORK_EARN_FROM_ROCKSTAR(500000);
				}
				break;
								   }
			case OPTION_XP: {
				drawMenuText("Give XP",i);
				if (optionPressed(i)) {
					int hash = GAMEPLAY::GET_HASH_KEY("MP0_CHAR_XP_FM");
					int prev = 0;
					STATS::STAT_GET_INT(hash,&prev,1);
					STATS::STAT_SET_INT(hash,prev+2000, 1);
				}
				break;
							}
			case OPTION_GODMODEON: {
				drawMenuText("Godmode On",i);
				if (optionPressed(i)) {
					PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(),true);
				}
				break;
								   }
			case OPTION_GODMODEOFF: {
				drawMenuText("Godmode Off",i);
				if (optionPressed(i)) {
					PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(),false);
				}
				break;
								   }
			case OPTION_SPAWNADDER: {
				drawMenuText("Spawn ADDER",i);
				if (optionPressed(i)) {
					spawnVehicleOnMe("adder");
				}
				break;
								   }
			case OPTION_AUTOSAVE: {
				drawMenuText("Autosave",i);
				if (optionPressed(i)) {
					GAMEPLAY::DO_AUTO_SAVE();
				}
				break;
								   }
			}
			}
			if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, BUTTON_CIRCLE)) {
				playSound("Back");
				menuOpen = false;
			}
			if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, DPAD_UP)) {
				goUp();
			}
			if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, DPAD_DOWN)) {
				goDown();
			}


			menu_open_loop();
		}
	} else {
		if (CONTROLS::IS_CONTROL_PRESSED(0, BUTTON_R1) && CONTROLS::IS_CONTROL_JUST_PRESSED(0, DPAD_RIGHT)) {
			menuOpen = true;
		}
	}
}