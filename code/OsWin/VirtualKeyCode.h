/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#define LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>

namespace Os
{
	using VKeyCode = uint8_t;

	class VKeyCodes
	{
	public:
		static const VKeyCode LButton = VK_LBUTTON;							// Left Mouse button
		static const VKeyCode RButton = VK_RBUTTON;							// Right mouse button
		static const VKeyCode Cancel = VK_CANCEL;							// Control - break processing
		static const VKeyCode MButton = VK_MBUTTON;							// Middle mouse button
		static const VKeyCode XButton1 = VK_XBUTTON1;						// X1 mouse button
		static const VKeyCode XButton2 = VK_XBUTTON2;						// X2 mouse button

		static const VKeyCode Back = VK_BACK;								// BACKSPACE key
		static const VKeyCode Tab = VK_TAB;									// TAB key

		static const VKeyCode Clear = VK_CLEAR;								// CLEAR key
		static const VKeyCode Return = VK_RETURN;							// ENTER key
		
		static const VKeyCode Shift = VK_SHIFT;								// SHIFT key
		static const VKeyCode Control = VK_CONTROL;							// CTRL key
		static const VKeyCode Alt = VK_MENU;									// ALT key
		static const VKeyCode Pause = VK_PAUSE;								// PAUSE key
		static const VKeyCode Capital = VK_CAPITAL;							// CAPS LOCK key
		static const VKeyCode Kana = VK_KANA;								// IME Kana mode
		static const VKeyCode Hangul = VK_HANGUL;							// IME Hangul mode
		static const VKeyCode ImeOn = VK_IME_ON;								// IME On
		static const VKeyCode Junja = VK_JUNJA;								// IME Junja mode
		static const VKeyCode Final = VK_FINAL;								// IME final mode
		static const VKeyCode Hanja = VK_HANJA;								// IME Hanja mode
		static const VKeyCode Kanji = VK_KANJI;								// IME Kanji mode
		static const VKeyCode ImeOff = VK_IME_OFF;							// IME Off
		static const VKeyCode Esc = VK_ESCAPE;								// ESC key
		static const VKeyCode Convert = VK_CONVERT;							// IME convert
		static const VKeyCode NonConvert = VK_NONCONVERT;					// IME nonconvert
		static const VKeyCode Accept = VK_ACCEPT;							// IME accept
		static const VKeyCode ModeChange = VK_MODECHANGE;					// IME mode change request
		static const VKeyCode Space = VK_SPACE;								// SPACEBAR

		static const VKeyCode PageUp = VK_PRIOR;								// PAGE UP key
		static const VKeyCode PageDown = VK_NEXT;							// PAGE DOWN key
		static const VKeyCode End = VK_END;									// END key
		static const VKeyCode Home = VK_HOME;								// HOME key
		static const VKeyCode Left = VK_LEFT;								// LEFT ARROW key
		static const VKeyCode Up = VK_UP;									// UP ARROW key
		static const VKeyCode Right = VK_RIGHT;								// RIGHT ARROW key
		static const VKeyCode Down = VK_DOWN;								// DOWN ARROW key
		static const VKeyCode Select = VK_SELECT;							// SELECT key
		static const VKeyCode Print = VK_PRINT;								// PRINT key
		static const VKeyCode Execute = VK_EXECUTE;							// EXECUTE key
		static const VKeyCode Snapshot = VK_SNAPSHOT;						// PRINT SCREEN key
		static const VKeyCode Insert = VK_INSERT;							// INS key
		static const VKeyCode Delete = VK_DELETE;							// DEL key
		static const VKeyCode Help = VK_HELP;								// HELP key

		static const VKeyCode Vk_0 = '0';									// 0 key
		static const VKeyCode Vk_1 = '1';									// 1 key
		static const VKeyCode Vk_2 = '2';									// 2 key
		static const VKeyCode Vk_3 = '3';									// 3 key
		static const VKeyCode Vk_4 = '4';									// 4 key
		static const VKeyCode Vk_5 = '5';									// 5 key
		static const VKeyCode Vk_6 = '6';									// 6 key
		static const VKeyCode Vk_7 = '7';									// 7 key
		static const VKeyCode Vk_8 = '8';									// 8 key
		static const VKeyCode Vk_9 = '9';									// 9 key
		
		static const VKeyCode Vk_A = 'A';									//A key
		static const VKeyCode Vk_B = 'B';									// B key
		static const VKeyCode Vk_C = 'C';									// C key
		static const VKeyCode Vk_D = 'D';									// D key
		static const VKeyCode Vk_E = 'E';									// E key
		static const VKeyCode Vk_F = 'F';									// F key
		static const VKeyCode Vk_G = 'G';									// G key
		static const VKeyCode Vk_H = 'H';									// H key
		static const VKeyCode Vk_I = 'I';									// I key
		static const VKeyCode Vk_J = 'J';									// J key
		static const VKeyCode Vk_K = 'K';									// K key
		static const VKeyCode Vk_L = 'L';									// L key
		static const VKeyCode Vk_M = 'M';									// M key
		static const VKeyCode Vk_N = 'N';									// N key
		static const VKeyCode Vk_O = 'O';									// O key
		static const VKeyCode Vk_P = 'P';									// P key
		static const VKeyCode Vk_Q = 'Q';									// Q key
		static const VKeyCode Vk_R = 'R';									// R key
		static const VKeyCode Vk_S = 'S';									// S key
		static const VKeyCode Vk_T = 'T';									// T key
		static const VKeyCode Vk_U = 'U';									// U key
		static const VKeyCode Vk_V = 'V';									// V key
		static const VKeyCode Vk_W = 'W';									// W key
		static const VKeyCode Vk_X = 'X';									// X key
		static const VKeyCode Vk_Y = 'Y';									// Y key
		static const VKeyCode Vk_Z = 'Z';									// Z key

		static const VKeyCode LeftWin = VK_LWIN;								// Left Windows key
		static const VKeyCode RightWin = VK_RWIN;							// Right Windows key
		static const VKeyCode Apps = VK_APPS;								// Applications key

		static const VKeyCode Sleep = VK_SLEEP;								// Computer Sleep key
		static const VKeyCode Numpad_0 = VK_NUMPAD0;							// Numeric keypad 0 key
		static const VKeyCode Numpad_1 = VK_NUMPAD1;							// Numeric keypad 1 key
		static const VKeyCode Numpad_2 = VK_NUMPAD2;							// Numeric keypad 2 key
		static const VKeyCode Numpad_3 = VK_NUMPAD3;							// Numeric keypad 3 key
		static const VKeyCode Numpad_4 = VK_NUMPAD4;							// Numeric keypad 4 key
		static const VKeyCode Numpad_5 = VK_NUMPAD5;							// Numeric keypad 5 key
		static const VKeyCode Numpad_6 = VK_NUMPAD6;							// Numeric keypad 6 key
		static const VKeyCode Numpad_7 = VK_NUMPAD7;							// Numeric keypad 7 key
		static const VKeyCode Numpad_8 = VK_NUMPAD8;							// Numeric keypad 8 key
		static const VKeyCode Numpad_9 = VK_NUMPAD9;							// Numeric keypad 9 key
		static const VKeyCode Multiply = VK_MULTIPLY;						// Multiply key
		static const VKeyCode Add = VK_ADD;									// Add key
		static const VKeyCode Separator = VK_SEPARATOR;						// Separator key
		static const VKeyCode Subtract = VK_SUBTRACT;						// Subtract key
		static const VKeyCode Decimal = VK_DECIMAL;							// Decimal key
		static const VKeyCode Divide = VK_DIVIDE;							// Divide key

		static const VKeyCode Vk_F1 = VK_F1;									//	F1 key
		static const VKeyCode Vk_F2 = VK_F2;									//	F2 key
		static const VKeyCode Vk_F3 = VK_F3;									//	F3 key
		static const VKeyCode Vk_F4 = VK_F4;									//	F4 key
		static const VKeyCode Vk_F5 = VK_F5;									//	F5 key
		static const VKeyCode Vk_F6 = VK_F6;									//	F6 key
		static const VKeyCode Vk_F7 = VK_F7;									//	F7 key
		static const VKeyCode Vk_F8 = VK_F8;									//	F8 key
		static const VKeyCode Vk_F9 = VK_F9;									//	F9 key
		static const VKeyCode Vk_F10 = VK_F10;								//	F10 key
		static const VKeyCode Vk_F11 = VK_F11;								//	F11 key
		static const VKeyCode Vk_F12 = VK_F12;								//	F12 key
		static const VKeyCode Vk_F13 = VK_F13;								//	F13 key
		static const VKeyCode Vk_F14 = VK_F14;								//	F14 key
		static const VKeyCode Vk_F15 = VK_F15;								//	F15 key
		static const VKeyCode Vk_F16 = VK_F16;								//	F16 key
		static const VKeyCode Vk_F17 = VK_F17;								//	F17 key
		static const VKeyCode Vk_F18 = VK_F18;								//	F18 key
		static const VKeyCode Vk_F19 = VK_F19;								//	F19 key
		static const VKeyCode Vk_F20 = VK_F20;								//	F20 key
		static const VKeyCode Vk_F21 = VK_F21;								//	F21 key
		static const VKeyCode Vk_F22 = VK_F22;								//	F22 key
		static const VKeyCode Vk_F23 = VK_F23;								//	F23 key
		static const VKeyCode Vk_F24 = VK_F24;								//	F24 key
		
		static const VKeyCode NumLock = VK_NUMLOCK;							// NUM LOCK key
		static const VKeyCode Scroll = VK_SCROLL;							// SCROLL LOCK key
		
		static const VKeyCode LeftShift = VK_LSHIFT;							// Left SHIFT key
		static const VKeyCode RightShift = VK_RSHIFT;						// Right SHIFT key
		static const VKeyCode LeftControl = VK_LCONTROL;						// Left CONTROL key
		static const VKeyCode RightControl = VK_RCONTROL;					// Right CONTROL key
		static const VKeyCode LeftAlt = VK_LMENU;							// Left ALT key
		static const VKeyCode RightAlt = VK_RMENU;							// Right ALT key

		static const VKeyCode BrowserBack = VK_BROWSER_BACK;					// Browser Back key
		static const VKeyCode BrowserForward = VK_BROWSER_FORWARD;			// Browser Forward key
		static const VKeyCode BrowserRefresh = VK_BROWSER_REFRESH;			// Browser Refresh key
		static const VKeyCode BrowserStop = VK_BROWSER_STOP;					// Browser Stop key
		static const VKeyCode BrowserSearch = VK_BROWSER_SEARCH;				// Browser Search key
		static const VKeyCode BrowserFavorites = VK_BROWSER_FAVORITES;		// Browser Favorites key
		static const VKeyCode BrowserHome = VK_BROWSER_HOME;					// Browser Start and Home key

		static const VKeyCode VolumeMute = VK_VOLUME_MUTE;					// Volume Mute key
		static const VKeyCode VolumeDown = VK_VOLUME_DOWN;					// Volume Down key
		static const VKeyCode VolumeUp = VK_VOLUME_UP;						// Volume Up key
		static const VKeyCode MediaNextTrack = VK_MEDIA_NEXT_TRACK;			// Next Track key
		static const VKeyCode MediaPrevTrack = VK_MEDIA_PREV_TRACK;			//Previous Track key
		static const VKeyCode MediaStop = VK_MEDIA_STOP;						// Stop Media key
		static const VKeyCode MediaPlayPause = VK_MEDIA_PLAY_PAUSE;			// Play / Pause Media key
		static const VKeyCode LaunchMail = VK_LAUNCH_MAIL;					//  Start Mail key
		static const VKeyCode LaunchMediaSelect = VK_LAUNCH_MEDIA_SELECT;	// Select Media key
		static const VKeyCode LaunchApp1 = VK_LAUNCH_APP1;					// Start Application 1 key
		static const VKeyCode LaunchApp2 = VK_LAUNCH_APP2;					// Start Application 2 key
		
		static const VKeyCode Oem1 = VK_OEM_1;								// Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the; : key
		static const VKeyCode OemPlus = VK_OEM_PLUS;							// For any country / region, the + key
		static const VKeyCode OemComma = VK_OEM_COMMA;						// For any country / region, the, key
		static const VKeyCode OemMinus = VK_OEM_MINUS;						// For any country / region, the - key
		static const VKeyCode OemPeriod = VK_OEM_PERIOD;						// For any country / region, the.key
		static const VKeyCode Oem2 = VK_OEM_2;								// Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the / ? key
		static const VKeyCode Oem3 = VK_OEM_3;								// Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the `~ key
		static const VKeyCode Oem4 = VK_OEM_4;								// Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the[{ key
		static const VKeyCode Oem5 = VK_OEM_5;								// Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the \\ | key
		static const VKeyCode Oem6 = VK_OEM_6;								// Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the]} key
		static const VKeyCode Oem7 = VK_OEM_7;								// Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the '" key
		static const VKeyCode Oem8 = VK_OEM_8;								// Used for miscellaneous characters; it can vary by keyboard.
	
		static const VKeyCode Oem102 = VK_OEM_102;							// The <> keys on the US standard keyboard, or the \\ | key on the non - US 102 - key keyboard

		static const VKeyCode ProcessKey = VK_PROCESSKEY;					// IME PROCESS key
		
		static const VKeyCode Packet = VK_PACKET;							// Used to pass Unicode characters as if they were keystrokes.The VK_PACKET key is the low word of a 32 - bit Virtual Key value used for non - keyboard input methods.For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
		
		static const VKeyCode Attn = VK_ATTN;								// Attn key
		static const VKeyCode CrSel = VK_CRSEL;								// CrSel key
		static const VKeyCode ExSel = VK_EXSEL;								// ExSel key
		static const VKeyCode EraseEOF = VK_EREOF;							// Erase EOF key
		static const VKeyCode Play = VK_PLAY;								// Play key
		static const VKeyCode Zoom = VK_ZOOM;								// Zoom key
		static const VKeyCode NoName = VK_NONAME;							// Reserved
		static const VKeyCode Pa1 = VK_PA1;									// PA1 key
		static const VKeyCode OemClear = VK_OEM_CLEAR;						// Clear key
	};
}
