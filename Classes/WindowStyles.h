#pragma once

// v1.4 By Ook
//

#include <gsl/gsl>

#include "EnumConcepts.h"

// this enum is here to simplify debugging
enum class WindowMessages
	: DWORD
{
	eWM_NULL,
	eWM_CREATE,	// lParam = LPCREATESTRUCT
	eWM_DESTROY,
	eWM_MOVE,
	eWM_SIZEWAIT,
	eWM_SIZE,	// lParam = width/height
	eWM_ACTIVATE,
	eWM_SETFOCUS,
	eWM_KILLFOCUS,
	eWM_SETVISIBLE,
	eWM_ENABLE,
	eWM_SETREDRAW,
	eWM_SETTEXT,
	eWM_GETTEXT,
	eWM_GETTEXTLENGTH,
	eWM_PAINT,
	eWM_CLOSE,
	eWM_QUERYENDSESSION,
	eWM_QUIT,
	eWM_QUERYOPEN,
	eWM_ERASEBKGND,	// wParam = HDC
	eWM_SYSCOLORCHANGE,
	eWM_ENDSESSION,
	eWM_SYSTEMERROR,
	eWM_SHOWWINDOW,
	eWM_CTLCOLOR,
	eWM_WININICHANGE,
	eWM_DEVMODECHANGE,
	eWM_ACTIVATEAPP,
	eWM_FONTCHANGE,
	eWM_TIMECHANGE,
	eWM_CANCELMODE,
	eWM_SETCURSOR,
	eWM_MOUSEACTIVATE,
	eWM_CHILDACTIVATE,
	eWM_QUEUESYNC,
	eWM_GETMINMAXINFO,
	eWM_LOGOFF,
	eWM_PAINTICON,
	eWM_ICONERASEBKGND,
	eWM_NEXTDLGCTL,
	eWM_ALTTABACTIVE,
	eWM_SPOOLERSTATUS,
	eWM_DRAWITEM,
	eWM_MEASUREITEM,
	eWM_DELETEITEM,
	eWM_VKEYTOITEM,
	eWM_CHARTOITEM,
	eWM_SETFONT,
	eWM_GETFONT,
	eWM_SETHOTKEY,
	eWM_GETHOTKEY,
	eWM_SHELLNOTIFY,
	eWM_ISACTIVEICON,
	eWM_QUERYPARKICON,
	eWM_QUERYDRAGICON,
	eWM_WINHELP,
	eWM_COMPAREITEM,
	eWM_FULLSCREEN,
	eWM_CLIENTSHUTDOWN,
	eWM_DDEMLEVENT,
	eWM_GETOBJECT,
	eundefined_1,
	eundefined_2,
	eWM_TESTING,
	eWM_COMPACTING,
	eWM_OTHERWINDOWCREATED,
	eWM_OTHERWINDOWDESTROYED,
	eWM_COMMNOTIFY,
	eundefined_3,
	eWM_WINDOWPOSCHANGING,
	eWM_WINDOWPOSCHANGED,
	eWM_POWER,
	eWM_COPYGLOBALDATA,
	eWM_COPYDATA,
	eWM_CANCELJOURNAL,
	eundefined_4,
	eWM_KEYF1,
	eWM_NOTIFY,
	eWM_ACCESS_WINDOW,
	eWM_INPUTLANGCHANGEREQUEST,
	eWM_INPUTLANGCHANGE,
	eWM_TCARD,
	eWM_HELP,
	eWM_USERCHANGED,
	eWM_NOTIFYFORMAT,
	eundefined_5,
	eundefined_6,
	eundefined_7,
	eundefined_8,
	eundefined_9,
	eundefined_10,
	eundefined_11,
	eundefined_12,
	eundefined_13,
	eundefined_14,
	eundefined_15,
	eundefined_16,
	eundefined_17,
	eundefined_18,
	eundefined_19,
	eundefined_20,
	eundefined_21,
	eundefined_22,
	eundefined_23,
	eundefined_24,
	eundefined_25,
	eundefined_26,
	eundefined_27,
	eundefined_28,
	eundefined_29,
	eundefined_30,
	eWM_FINALDESTROY,
	eWM_MEASUREITEM_CLIENTDATA,
	eundefined_31,
	eundefined_32,
	eundefined_33,
	eundefined_34,
	eundefined_35,
	eundefined_36,
	eundefined_37,
	eundefined_38,
	eundefined_39,
	eWM_CONTEXTMENU,
	eWM_STYLECHANGING,
	eWM_STYLECHANGED,
	eWM_DISPLAYCHANGE,
	eWM_GETICON,
	eWM_SETICON,
	eWM_NCCREATE,	// lParam = LPCREATESTRUCT
	eWM_NCDESTROY,
	eWM_NCCALCSIZE,
	eWM_NCHITTEST,
	eWM_NCPAINT,
	eWM_NCACTIVATE,
	eWM_GETDLGCODE,
	eWM_SYNCPAINT,
	eWM_SYNCTASK,
	eundefined_40,
	eWM_KLUDGEMINRECT,
	eWM_LPKDRAWSWITCHWND,
	eundefined_41,
	eundefined_42,
	eundefined_43,
	eWM_UAHDESTROYWINDOW,
	eWM_UAHDRAWMENU,			// lParam is UAHMENU
	eWM_UAHDRAWMENUITEM,		// lParam is UAHDRAWMENUITEM
	eWM_UAHINITMENU,
	eWM_UAHMEASUREMENUITEM,		// lParam is UAHMEASUREMENUITEM
	eWM_UAHNCPAINTMENUPOPUP,
	eWM_UAHUPDATE,
	eundefined_44,
	eundefined_45,
	eundefined_46,
	eundefined_47,
	eundefined_48,
	eundefined_49,
	eundefined_50,
	eundefined_51,
	eundefined_52,
	eWM_NCMOUSEMOVE,
	eWM_NCLBUTTONDOWN,
	eWM_NCLBUTTONUP,
	eWM_NCLBUTTONDBLCLK,
	eWM_NCRBUTTONDOWN,
	eWM_NCRBUTTONUP,
	eWM_NCRBUTTONDBLCLK,
	eWM_NCMBUTTONDOWN,
	eWM_NCMBUTTONUP,
	eWM_NCMBUTTONDBLCLK,
	eundefined_53,
	eWM_NCXBUTTONDOWN,
	eWM_NCXBUTTONUP,
	eWM_NCXBUTTONDBLCLK,
	eWM_NCUAHDRAWCAPTION, // for drawing themed caption
	eWM_NCUAHDRAWFRAME,	// wParam = HDC for drawing themed window borders.
	eEM_GETSEL,
	eEM_SETSEL,
	eEM_GETRECT,
	eEM_SETRECT,
	eEM_SETRECTNP,
	eEM_SCROLL,
	eEM_LINESCROLL,
	eEM_SCROLLCARET,
	eEM_GETMODIFY,
	eEM_SETMODIFY,
	eEM_GETLINECOUNT,
	eEM_LINEINDEX,
	eEM_SETHANDLE,
	eEM_GETHANDLE,
	eEM_GETTHUMB,
	eundefined_54,
	eundefined_55,
	eEM_LINELENGTH,
	eEM_REPLACESEL,
	eEM_SETFONT,
	eEM_GETLINE,
	eEM_LIMITTEXT,
	eEM_CANUNDO,
	eEM_UNDO,
	eEM_FMTLINES,
	eEM_LINEFROMCHAR,
	eEM_SETWORDBREAK,
	eEM_SETTABSTOPS,
	eEM_SETPASSWORDCHAR,
	eEM_EMPTYUNDOBUFFER,
	eEM_GETFIRSTVISIBLELINE,
	eEM_SETREADONLY,
	eEM_SETWORDBREAKPROC,
	eEM_GETWORDBREAKPROC,
	eEM_GETPASSWORDCHAR,
	eEM_SETMARGINS,
	eEM_GETMARGINS,
	eEM_GETLIMITTEXT,
	eEM_POSFROMCHAR,
	eEM_CHARFROMPOS,
	eEM_SETIMESTATUS,
	eEM_GETIMESTATUS,
	eEM_MSGMAX,
	eundefined_56,
	eundefined_57,
	eundefined_58,
	eundefined_59,
	eundefined_60,
	eundefined_61,
	eundefined_62,
	eundefined_63,
	eundefined_64,
	eundefined_65,
	eundefined_66,
	eundefined_67,
	eundefined_68,
	eundefined_69,
	eundefined_70,
	eundefined_71,
	eundefined_72,
	eundefined_73,
	eundefined_74,
	eundefined_75,
	eundefined_76,
	eundefined_77,
	eundefined_78,
	eundefined_79,
	eundefined_80,
	eundefined_81,
	eundefined_82,
	eundefined_83,
	eundefined_84,
	eundefined_85,
	eundefined_86,
	eundefined_87,
	eundefined_88,
	eundefined_89,
	eundefined_90,
	eWM_INPUT_DEVICE_CHANGE,
	eWM_INPUT,
	eWM_KEYDOWN,
	eWM_KEYUP,
	eWM_CHAR,
	eWM_DEADCHAR,
	eWM_SYSKEYDOWN,
	eWM_SYSKEYUP,
	eWM_SYSCHAR,
	eWM_SYSDEADCHAR,
	eWM_YOMICHAR,
	eWM_UNICHAR,
	eWM_CONVERTREQUEST,
	eWM_CONVERTRESULT,
	eWM_INTERIM,
	eWM_IME_STARTCOMPOSITION,
	eWM_IME_ENDCOMPOSITION,
	eWM_IME_COMPOSITION,
	eWM_INITDIALOG,
	eWM_COMMAND,
	eWM_SYSCOMMAND,
	eWM_TIMER,
	eWM_HSCROLL,
	eWM_VSCROLL,
	eWM_INITMENU,
	eWM_INITMENUPOPUP,
	eWM_SYSTIMER,
	eWM_GESTURE,
	eWM_GESTURENOTIFY,
	eWM_GESTUREINPUT,
	eWM_GESTURENOTIFIED,
	eundefined_91,
	eundefined_92,
	eWM_MENUSELECT,
	eWM_MENUCHAR,
	eWM_ENTERIDLE,
	eWM_MENURBUTTONUP,
	eWM_MENUDRAG,
	eWM_MENUGETOBJECT,
	eWM_UNINITMENUPOPUP,
	eWM_MENUCOMMAND,
	eWM_CHANGEUISTATE,
	eWM_UPDATEUISTATE,
	eWM_QUERYUISTATE,
	eundefined_93,
	eundefined_94,
	eundefined_95,
	eundefined_96,
	eundefined_97,
	eundefined_98,
	eundefined_99,
	eWM_LBTRACKPOINT,
	eWM_CTLCOLORMSGBOX,
	eWM_CTLCOLOREDIT,
	eWM_CTLCOLORLISTBOX,
	eWM_CTLCOLORBTN,
	eWM_CTLCOLORDLG,
	eWM_CTLCOLORSCROLLBAR,
	eWM_CTLCOLORSTATIC,
	eundefined_100,
	eundefined_101,
	eundefined_102,
	eundefined_103,
	eundefined_104,
	eundefined_105,
	eundefined_106,
	eCB_GETEDITSEL,
	eCB_LIMITTEXT,
	eCB_SETEDITSEL,
	eCB_ADDSTRING,
	eCB_DELETESTRING,
	eCB_DIR,
	eCB_GETCOUNT,
	eCB_GETCURSEL,
	eCB_GETLBTEXT,
	eCB_GETLBTEXTLEN,
	eCB_INSERTSTRING,
	eCB_RESETCONTENT,
	eCB_FINDSTRING,
	eCB_SELECTSTRING,
	eCB_SETCURSEL,
	eCB_SHOWDROPDOWN,
	eCB_GETITEMDATA,
	eCB_SETITEMDATA,
	eCB_GETDROPPEDCONTROLRECT,
	eCB_SETITEMHEIGHT,
	eCB_GETITEMHEIGHT,
	eCB_SETEXTENDEDUI,
	eCB_GETEXTENDEDUI,
	eCB_GETDROPPEDSTATE,
	eCB_FINDSTRINGEXACT,
	eCB_SETLOCALE,
	eCB_GETLOCALE,
	eCB_GETTOPINDEX,
	eCB_SETTOPINDEX,
	eCB_GETHORIZONTALEXTENT,
	eCB_SETHORIZONTALEXTENT,
	eCB_GETDROPPEDWIDTH,
	eCB_SETDROPPEDWIDTH,
	eCB_INITSTORAGE,
	eCB_MSGMAX_OLD,
	eCB_MULTIPLEADDSTRING,
	eCB_GETCOMBOBOXINFO,
	eCB_MSGMAX,
	eCBEC_SETCOMBOFOCUS,
	eCBEC_KILLCOMBOFOCUS,
	eundefined_109,
	eundefined_110,
	eundefined_111,
	eundefined_112,
	eundefined_113,
	eundefined_114,
	eundefined_115,
	eundefined_116,
	eundefined_117,
	eundefined_118,
	eundefined_119,
	eundefined_120,
	eundefined_121,
	eundefined_122,
	eundefined_123,
	eundefined_124,
	eundefined_125,
	eundefined_126,
	eundefined_127,
	eundefined_128,
	eundefined_129,
	eundefined_130,
	eundefined_131,
	eundefined_132,
	eLB_ADDSTRING,
	eLB_INSERTSTRING,
	eLB_DELETESTRING,
	eLB_SELITEMRANGEEX,
	eLB_RESETCONTENT,
	eLB_SETSEL,
	eLB_SETCURSEL,
	eLB_GETSEL,
	eLB_GETCURSEL,
	eLB_GETTEXT,
	eLB_GETTEXTLEN,
	eLB_GETCOUNT,
	eLB_SELECTSTRING,
	eLB_DIR,
	eLB_GETTOPINDEX,
	eLB_FINDSTRING,
	eLB_GETSELCOUNT,
	eLB_GETSELITEMS,
	eLB_SETTABSTOPS,
	eLB_GETHORIZONTALEXTENT,
	eLB_SETHORIZONTALEXTENT,
	eLB_SETCOLUMNWIDTH,
	eLB_ADDFILE,
	eLB_SETTOPINDEX,
	eLB_GETITEMRECT,
	eLB_GETITEMDATA,
	eLB_SETITEMDATA,
	eLB_SELITEMRANGE,
	eLB_SETANCHORINDEX,
	eLB_GETANCHORINDEX,
	eLB_SETCARETINDEX,
	eLB_GETCARETINDEX,
	eLB_SETITEMHEIGHT,
	eLB_GETITEMHEIGHT,
	eLB_FINDSTRINGEXACT,
	eLBCB_CARETON,
	eLBCB_CARETOFF,
	eLB_SETLOCALE,
	eLB_GETLOCALE,
	eLB_SETCOUNT,
	eLB_INITSTORAGE,
	eLB_ITEMFROMPOINT,
	eLB_INSERTSTRINGUPPER,
	eLB_INSERTSTRINGLOWER,
	eLB_ADDSTRINGUPPER,
	eLB_ADDSTRINGLOWER,
	eLBCB_STARTTRACK,
	eLBCB_ENDTRACK,
	eLB_MSGMAX_OLD,
	eLB_MULTIPLEADDSTRING,
	eLB_GETLISTBOXINFO,
	eLB_MSGMAX,
	eundefined_133,
	eundefined_134,
	eundefined_135,
	eundefined_136,
	eundefined_137,
	eundefined_138,
	eundefined_139,
	eundefined_140,
	eundefined_141,
	eundefined_142,
	eundefined_143,
	eundefined_144,
	eundefined_145,
	eundefined_146,
	eundefined_147,
	eundefined_148,
	eundefined_149,
	eundefined_150,
	eundefined_151,
	eundefined_152,
	eundefined_153,
	eundefined_154,
	eundefined_155,
	eundefined_156,
	eundefined_157,
	eundefined_158,
	eundefined_159,
	eundefined_160,
	eundefined_161,
	eundefined_162,
	eundefined_163,
	eundefined_164,
	eundefined_165,
	eundefined_166,
	eundefined_167,
	eundefined_168,
	eundefined_169,
	eundefined_170,
	eundefined_171,
	eundefined_172,
	eundefined_173,
	eundefined_174,
	eundefined_175,
	eundefined_176,
	eMN_FIRST,
	eMN_GETHMENU,
	eMN_SIZEWINDOW,
	eMN_OPENHIERARCHY,
	eMN_CLOSEHIERARCHY,
	eMN_SELECTITEM,					// wParam = item pos or -1 when not over menu.
	eMN_CANCELMENUS,
	eMN_SELECTFIRSTVALIDITEM,
	eundefined_183,
	eundefined_184,
	eMN_GETPPOPUPMENU,
	eMN_FINDMENUWINDOWFROMPOINT,	// returns HWND or zero
	eMN_SHOWPOPUPWINDOW,
	eMN_BUTTONDOWN,
	eMN_MOUSEMOVE,
	eMN_BUTTONUP,
	eMN_SETTIMERTOOPENHIERARCHY,
	eMN_DBLCLK,
	eundefined_193,
	eundefined_194,
	eundefined_195,
	eundefined_196,
	eundefined_197,
	eundefined_198,
	eundefined_199,
	eundefined_200,
	eundefined_201,
	eundefined_202,
	eundefined_203,
	eundefined_204,
	eundefined_205,
	eundefined_206,
	eWM_MOUSEMOVE,
	eWM_LBUTTONDOWN,
	eWM_LBUTTONUP,
	eWM_LBUTTONDBLCLK,
	eWM_RBUTTONDOWN,
	eWM_RBUTTONUP,
	eWM_RBUTTONDBLCLK,
	eWM_MBUTTONDOWN,
	eWM_MBUTTONUP,
	eWM_MBUTTONDBLCLK,
	eWM_MOUSEWHEEL,
	eWM_XBUTTONDOWN,
	eWM_XBUTTONUP,
	eWM_XBUTTONDBLCLK,
	eWM_MOUSEHWHEEL,
	eundefined_207,
	eWM_PARENTNOTIFY,
	eWM_ENTERMENULOOP,
	eWM_EXITMENULOOP,
	eWM_NEXTMENU,
	eWM_SIZING,
	eWM_CAPTURECHANGED,
	eWM_MOVING,
	eundefined_208,
	eWM_POWERBROADCAST,
	eWM_DEVICECHANGE,
	eundefined_209,
	eundefined_210,
	eundefined_211,
	eundefined_212,
	eundefined_213,
	eundefined_214,
	eWM_MDICREATE,
	eWM_MDIDESTROY,
	eWM_MDIACTIVATE,
	eWM_MDIRESTORE,
	eWM_MDINEXT,
	eWM_MDIMAXIMIZE,
	eWM_MDITILE,
	eWM_MDICASCADE,
	eWM_MDIICONARRANGE,
	eWM_MDIGETACTIVE,
	eWM_DROPOBJECT,
	eWM_QUERYDROPOBJECT,
	eWM_BEGINDRAG,
	eWM_DRAGLOOP,
	eWM_DRAGSELECT,
	eWM_DRAGMOVE,
	eWM_MDISETMENU,
	eWM_ENTERSIZEMOVE,
	eWM_EXITSIZEMOVE,
	eWM_DROPFILES,
	eWM_MDIREFRESHMENU,
	eundefined_215,
	eundefined_216,
	eundefined_217,
	eWM_POINTERDEVICECHANGE,
	eWM_POINTERDEVICEINRANGE,
	eWM_POINTERDEVICEOUTOFRANGE,
	eWM_STOPINERTIA,
	eWM_ENDINERTIA,
	eWM_EDGYINERTIA,
	eundefined_218,
	eundefined_219,
	eWM_TOUCH,
	eWM_NCPOINTERUPDATE,
	eWM_NCPOINTERDOWN,
	eWM_NCPOINTERUP,
	eWM_NCPOINTERLAST,
	eWM_POINTERUPDATE,
	eWM_POINTERDOWN,
	eWM_POINTERUP,
	eWM_POINTER_reserved_248,
	eWM_POINTERENTER,
	eWM_POINTERLEAVE,
	eWM_POINTERACTIVATE,
	eWM_POINTERCAPTURECHANGED,
	eWM_TOUCHHITTESTING,
	eWM_POINTERWHEEL,
	eWM_POINTERHWHEEL,
	eWM_POINTER_reserved_250,
	eWM_POINTER_reserved_251,
	eWM_POINTER_reserved_252,
	eWM_POINTER_reserved_253,
	eWM_POINTER_reserved_254,
	eWM_POINTER_reserved_255,
	eWM_POINTER_reserved_256,
	eWM_POINTERLAST,
	eundefined_220,
	eundefined_221,
	eundefined_222,
	eundefined_223,
	eundefined_224,
	eundefined_225,
	eundefined_226,
	eundefined_227,
	eundefined_228,
	eundefined_229,
	eundefined_230,
	eundefined_231,
	eundefined_232,
	eundefined_233,
	eundefined_234,
	eundefined_235,
	eundefined_236,
	eundefined_237,
	eundefined_238,
	eundefined_239,
	eundefined_240,
	eundefined_241,
	eundefined_242,
	eundefined_243,
	eWM_VISIBILITYCHANGED,
	eWM_VIEWSTATECHANGED,
	eWM_UNREGISTER_WINDOW_SERVICES,
	eWM_CONSOLIDATED,
	eundefined_244,
	eundefined_245,
	eundefined_246,
	eundefined_247,
	eundefined_248,
	eundefined_249,
	eundefined_250,
	eundefined_251,
	eundefined_252,
	eundefined_253,
	eundefined_254,
	eundefined_255,
	eWM_IME_REPORT,
	eWM_IME_SETCONTEXT,
	eWM_IME_NOTIFY,
	eWM_IME_CONTROL,
	eWM_IME_COMPOSITIONFULL,
	eWM_IME_SELECT,
	eWM_IME_CHAR,
	eWM_IME_SYSTEM,
	eWM_IME_REQUEST,
	eWM_KANJI_reserved_289,
	eWM_KANJI_reserved_28a,
	eWM_KANJI_reserved_28b,
	eWM_KANJI_reserved_28c,
	eWM_KANJI_reserved_28d,
	eWM_KANJI_reserved_28e,
	eWM_KANJI_reserved_28f,
	eWM_IME_KEYDOWN,
	eWM_IME_KEYUP,
	eWM_KANJI_reserved_292,
	eWM_KANJI_reserved_293,
	eWM_KANJI_reserved_294,
	eWM_KANJI_reserved_295,
	eWM_KANJI_reserved_296,
	eWM_KANJI_reserved_297,
	eWM_KANJI_reserved_298,
	eWM_KANJI_reserved_299,
	eWM_KANJI_reserved_29a,
	eWM_KANJI_reserved_29b,
	eWM_KANJI_reserved_29c,
	eWM_KANJI_reserved_29d,
	eWM_KANJI_reserved_29e,
	eWM_KANJILAST,
	eWM_NCMOUSEHOVER,
	eWM_MOUSEHOVER,
	eWM_NCMOUSELEAVE,
	eWM_MOUSELEAVE,
	eWM_TRACKMOUSEEVENT__reserved_2a4,
	eWM_TRACKMOUSEEVENT__reserved_2a5,
	eWM_TRACKMOUSEEVENT__reserved_2a6,
	eWM_TRACKMOUSEEVENT__reserved_2a7,
	eWM_TRACKMOUSEEVENT__reserved_2a8,
	eWM_TRACKMOUSEEVENT__reserved_2a9,
	eWM_TRACKMOUSEEVENT__reserved_2aa,
	eWM_TRACKMOUSEEVENT__reserved_2ab,
	eWM_TRACKMOUSEEVENT__reserved_2ac,
	eWM_TRACKMOUSEEVENT__reserved_2ad,
	eWM_TRACKMOUSEEVENT__reserved_2ae,
	eWM_TRACKMOUSEEVENT_LAST,
	eundefined_256,
	eWM_WTSSESSION_CHANGE,
	eundefined_257,
	eundefined_258,
	eundefined_259,
	eundefined_260,
	eundefined_261,
	eundefined_262,
	eundefined_263,
	eundefined_264,
	eundefined_265,
	eundefined_266,
	eundefined_267,
	eundefined_268,
	eundefined_269,
	eundefined_270,
	eWM_TABLET_FIRST,
	eWM_TABLET__reserved_2c1,
	eWM_TABLET__reserved_2c2,
	eWM_TABLET__reserved_2c3,
	eWM_TABLET__reserved_2c4,
	eWM_TABLET__reserved_2c5,
	eWM_TABLET__reserved_2c6,
	eWM_TABLET__reserved_2c7,
	eWM_POINTERDEVICEADDED,
	eWM_POINTERDEVICEDELETED,
	eWM_TABLET__reserved_2ca,
	eWM_FLICK,
	eWM_TABLET__reserved_2cc,
	eWM_FLICKINTERNAL,
	eWM_BRIGHTNESSCHANGED,
	eWM_TABLET__reserved_2cf,
	eWM_TABLET__reserved_2d0,
	eWM_TABLET__reserved_2d1,
	eWM_TABLET__reserved_2d2,
	eWM_TABLET__reserved_2d3,
	eWM_TABLET__reserved_2d4,
	eWM_TABLET__reserved_2d5,
	eWM_TABLET__reserved_2d6,
	eWM_TABLET__reserved_2d7,
	eWM_TABLET__reserved_2d8,
	eWM_TABLET__reserved_2d9,
	eWM_TABLET__reserved_2da,
	eWM_TABLET__reserved_2db,
	eWM_TABLET__reserved_2dc,
	eWM_TABLET__reserved_2dd,
	eWM_TABLET__reserved_2de,
	eWM_TABLET_LAST,
	eWM_DPICHANGED,
	eundefined_271,
	eundefined_272,
	eundefined_273,
	eundefined_274,
	eundefined_275,
	eundefined_276,
	eundefined_277,
	eundefined_278,
	eundefined_279,
	eundefined_280,
	eundefined_281,
	eundefined_282,
	eundefined_283,
	eundefined_284,
	eundefined_285,
	eundefined_286,
	eundefined_287,
	eundefined_288,
	eundefined_289,
	eundefined_290,
	eundefined_291,
	eundefined_292,
	eundefined_293,
	eundefined_294,
	eundefined_295,
	eundefined_296,
	eundefined_297,
	eundefined_298,
	eundefined_299,
	eundefined_300,
	eundefined_301,
	eWM_CUT,
	eWM_COPY,
	eWM_PASTE,
	eWM_CLEAR,
	eWM_UNDO,
	eWM_RENDERFORMAT,
	eWM_RENDERALLFORMATS,
	eWM_DESTROYCLIPBOARD,
	eWM_DRAWCLIPBOARD,
	eWM_PAINTCLIPBOARD,
	eWM_VSCROLLCLIPBOARD,
	eWM_SIZECLIPBOARD,
	eWM_ASKCBFORMATNAME,
	eWM_CHANGECBCHAIN,
	eWM_HSCROLLCLIPBOARD,
	eWM_QUERYNEWPALETTE,
	eWM_PALETTEISCHANGING,
	eWM_PALETTECHANGED,
	eWM_HOTKEY,
	eWM_SYSMENU,
	eWM_HOOKMSG,
	eWM_EXITPROCESS,
	eWM_WAKETHREAD,
	eWM_PRINT,
	eWM_PRINTCLIENT,
	eWM_APPCOMMAND,
	eWM_THEMECHANGED,
	eWM_UAHINIT,
	eWM_DESKTOPNOTIFY,
	eWM_CLIPBOARDUPDATE,
	eWM_DWMCOMPOSITIONCHANGED,
	eWM_DWMNCRENDERINGCHANGED,
	eWM_DWMCOLORIZATIONCOLORCHANGED,
	eWM_DWMWINDOWMAXIMIZEDCHANGE,
	eWM_DWMEXILEFRAME,
	eWM_DWMSENDICONICTHUMBNAIL,
	eWM_MAGNIFICATION_STARTED,
	eWM_MAGNIFICATION_ENDED,
	eWM_DWMSENDICONICLIVEPREVIEWBITMAP,
	eWM_DWMTHUMBNAILSIZECHANGED,
	eWM_MAGNIFICATION_OUTPUT,
	eWM_BSDRDATA,
	eWM_DWMTRANSITIONSTATECHANGED,
	eundefined_302,
	eWM_KEYBOARDCORRECTIONCALLOUT,
	eWM_KEYBOARDCORRECTIONACTION,
	eWM_UIACTION,
	eWM_ROUTED_UI_EVENT,
	eWM_MEASURECONTROL,
	eWM_GETACTIONTEXT,
	eWM_CE_ONLY__reserved_332,
	eWM_FORWARDKEYDOWN,
	eWM_FORWARDKEYUP,
	eWM_CE_ONLY__reserved_335,
	eWM_CE_ONLY__reserved_336,
	eWM_CE_ONLY__reserved_337,
	eWM_CE_ONLY__reserved_338,
	eWM_CE_ONLY__reserved_339,
	eWM_CE_ONLY__reserved_33a,
	eWM_CE_ONLY__reserved_33b,
	eWM_CE_ONLY__reserved_33c,
	eWM_CE_ONLY__reserved_33d,
	eWM_CE_ONLY_LAST,
	eWM_GETTITLEBARINFOEX,
	eWM_NOTIFYWOW,
	eundefined_303,
	eundefined_304,
	eundefined_305,
	eundefined_306,
	eundefined_307,
	eundefined_308,
	eundefined_309,
	eundefined_310,
	eundefined_311,
	eundefined_312,
	eundefined_313,
	eundefined_314,
	eundefined_315,
	eundefined_316,
	eundefined_317,
	eundefined_318,
	eundefined_319,
	eundefined_320,
	eundefined_321,
	eundefined_322,
	eundefined_323,
	eundefined_324,
	eundefined_325,
	eWM_HANDHELDFIRST,
	eWM_HANDHELD_reserved_359,
	eWM_HANDHELD_reserved_35a,
	eWM_HANDHELD_reserved_35b,
	eWM_HANDHELD_reserved_35c,
	eWM_HANDHELD_reserved_35d,
	eWM_HANDHELD_reserved_35e,
	eWM_HANDHELDLAST,
	eWM_AFXFIRST,
	eWM_AFX_reserved_361,
	eWM_AFX_reserved_362,
	eWM_AFX_reserved_363,
	eWM_AFX_reserved_364,
	eWM_AFX_reserved_365,
	eWM_AFX_reserved_366,
	eWM_AFX_reserved_367,
	eWM_AFX_reserved_368,
	eWM_AFX_reserved_369,
	eWM_AFX_reserved_36a,
	eWM_AFX_reserved_36b,
	eWM_AFX_reserved_36c,
	eWM_AFX_reserved_36d,
	eWM_AFX_reserved_36e,
	eWM_AFX_reserved_36f,
	eWM_AFX_reserved_370,
	eWM_AFX_reserved_371,
	eWM_AFX_reserved_372,
	eWM_AFX_reserved_373,
	eWM_AFX_reserved_374,
	eWM_AFX_reserved_375,
	eWM_AFX_reserved_376,
	eWM_AFX_reserved_377,
	eWM_AFX_reserved_378,
	eWM_AFX_reserved_379,
	eWM_AFX_reserved_37a,
	eWM_AFX_reserved_37b,
	eWM_AFX_reserved_37c,
	eWM_AFX_reserved_37d,
	eWM_AFX_reserved_37e,
	eWM_AFXLAST,
	eWM_PENWINFIRST,
	eWM_PENWIN_reserved_381,
	eWM_PENWIN_reserved_382,
	eWM_PENWIN_reserved_383,
	eWM_PENWIN_reserved_384,
	eWM_PENWIN_reserved_385,
	eWM_PENWIN_reserved_386,
	eWM_PENWIN_reserved_387,
	eWM_PENWIN_reserved_388,
	eWM_PENWIN_reserved_389,
	eWM_PENWIN_reserved_38a,
	eWM_PENWIN_reserved_38b,
	eWM_PENWIN_reserved_38c,
	eWM_PENWIN_reserved_38d,
	eWM_PENWIN_reserved_38e,
	eWM_PENWINLAST,
	eWM_COALESCE_FIRST,
	eWM_COALESCE__reserved_391,
	eWM_COALESCE__reserved_392,
	eWM_COALESCE__reserved_393,
	eWM_COALESCE__reserved_394,
	eWM_COALESCE__reserved_395,
	eWM_COALESCE__reserved_396,
	eWM_COALESCE__reserved_397,
	eWM_COALESCE__reserved_398,
	eWM_COALESCE__reserved_399,
	eWM_COALESCE__reserved_39a,
	eWM_COALESCE__reserved_39b,
	eWM_COALESCE__reserved_39c,
	eWM_COALESCE__reserved_39d,
	eWM_COALESCE__reserved_39e,
	eWM_COALESCE_LAST,
	eWM_MM_RESERVED_FIRST,
	eWM_MM_RESERVED__reserved_3a1,
	eWM_MM_RESERVED__reserved_3a2,
	eWM_MM_RESERVED__reserved_3a3,
	eWM_MM_RESERVED__reserved_3a4,
	eWM_MM_RESERVED__reserved_3a5,
	eWM_MM_RESERVED__reserved_3a6,
	eWM_MM_RESERVED__reserved_3a7,
	eWM_MM_RESERVED__reserved_3a8,
	eWM_MM_RESERVED__reserved_3a9,
	eWM_MM_RESERVED__reserved_3aa,
	eWM_MM_RESERVED__reserved_3ab,
	eWM_MM_RESERVED__reserved_3ac,
	eWM_MM_RESERVED__reserved_3ad,
	eWM_MM_RESERVED__reserved_3ae,
	eWM_MM_RESERVED__reserved_3af,
	eWM_MM_RESERVED__reserved_3b0,
	eWM_MM_RESERVED__reserved_3b1,
	eWM_MM_RESERVED__reserved_3b2,
	eWM_MM_RESERVED__reserved_3b3,
	eWM_MM_RESERVED__reserved_3b4,
	eWM_MM_RESERVED__reserved_3b5,
	eWM_MM_RESERVED__reserved_3b6,
	eWM_MM_RESERVED__reserved_3b7,
	eWM_MM_RESERVED__reserved_3b8,
	eWM_MM_RESERVED__reserved_3b9,
	eWM_MM_RESERVED__reserved_3ba,
	eWM_MM_RESERVED__reserved_3bb,
	eWM_MM_RESERVED__reserved_3bc,
	eWM_MM_RESERVED__reserved_3bd,
	eWM_MM_RESERVED__reserved_3be,
	eWM_MM_RESERVED__reserved_3bf,
	eWM_MM_RESERVED__reserved_3c0,
	eWM_MM_RESERVED__reserved_3c1,
	eWM_MM_RESERVED__reserved_3c2,
	eWM_MM_RESERVED__reserved_3c3,
	eWM_MM_RESERVED__reserved_3c4,
	eWM_MM_RESERVED__reserved_3c5,
	eWM_MM_RESERVED__reserved_3c6,
	eWM_MM_RESERVED__reserved_3c7,
	eWM_MM_RESERVED__reserved_3c8,
	eWM_MM_RESERVED__reserved_3c9,
	eWM_MM_RESERVED__reserved_3ca,
	eWM_MM_RESERVED__reserved_3cb,
	eWM_MM_RESERVED__reserved_3cc,
	eWM_MM_RESERVED__reserved_3cd,
	eWM_MM_RESERVED__reserved_3ce,
	eWM_MM_RESERVED__reserved_3cf,
	eWM_MM_RESERVED__reserved_3d0,
	eWM_MM_RESERVED__reserved_3d1,
	eWM_MM_RESERVED__reserved_3d2,
	eWM_MM_RESERVED__reserved_3d3,
	eWM_MM_RESERVED__reserved_3d4,
	eWM_MM_RESERVED__reserved_3d5,
	eWM_MM_RESERVED__reserved_3d6,
	eWM_MM_RESERVED__reserved_3d7,
	eWM_MM_RESERVED__reserved_3d8,
	eWM_MM_RESERVED__reserved_3d9,
	eWM_MM_RESERVED__reserved_3da,
	eWM_MM_RESERVED__reserved_3db,
	eWM_MM_RESERVED__reserved_3dc,
	eWM_MM_RESERVED__reserved_3dd,
	eWM_MM_RESERVED__reserved_3de,
	eWM_MM_RESERVED_LAST,
	eWM_INTERNAL_DDE_FIRST,
	eWM_INTERNAL_DDE__reserved_3e1,
	eWM_INTERNAL_DDE__reserved_3e2,
	eWM_INTERNAL_DDE__reserved_3e3,
	eWM_INTERNAL_DDE__reserved_3e4,
	eWM_INTERNAL_DDE__reserved_3e5,
	eWM_INTERNAL_DDE__reserved_3e6,
	eWM_INTERNAL_DDE__reserved_3e7,
	eWM_INTERNAL_DDE__reserved_3e8,
	eWM_INTERNAL_DDE__reserved_3e9,
	eWM_INTERNAL_DDE__reserved_3ea,
	eWM_INTERNAL_DDE__reserved_3eb,
	eWM_INTERNAL_DDE__reserved_3ec,
	eWM_INTERNAL_DDE__reserved_3ed,
	eWM_INTERNAL_DDE__reserved_3ee,
	eWM_INTERNAL_DDE_LAST,
	eWM_CBT_RESERVED_FIRST,
	eWM_CBT_RESERVED__reserved_3f1,
	eWM_CBT_RESERVED__reserved_3f2,
	eWM_CBT_RESERVED__reserved_3f3,
	eWM_CBT_RESERVED__reserved_3f4,
	eWM_CBT_RESERVED__reserved_3f5,
	eWM_CBT_RESERVED__reserved_3f6,
	eWM_CBT_RESERVED__reserved_3f7,
	eWM_CBT_RESERVED__reserved_3f8,
	eWM_CBT_RESERVED__reserved_3f9,
	eWM_CBT_RESERVED__reserved_3fa,
	eWM_CBT_RESERVED__reserved_3fb,
	eWM_CBT_RESERVED__reserved_3fc,
	eWM_CBT_RESERVED__reserved_3fd,
	eWM_CBT_RESERVED__reserved_3fe,
	eWM_CBT_RESERVED_LAST,
};

// Window Styles Helper enums.

enum class WindowStyle
	: DWORD
{
	// General styles
	None,
	Overlapped = WS_OVERLAPPED,
	OverlappedWindow = WS_OVERLAPPEDWINDOW,
	Popup = WS_POPUP,
	PopupWindow = WS_POPUPWINDOW,
	Child = WS_CHILD,
	Minimize = WS_MINIMIZE,
	Visible = WS_VISIBLE,
	Disabled = WS_DISABLED,
	ClipSiblings = WS_CLIPSIBLINGS,
	ClipChildren = WS_CLIPCHILDREN,
	Maximize = WS_MAXIMIZE,
	Caption = WS_CAPTION,
	Border = WS_BORDER,
	DLGFrame = WS_DLGFRAME,
	VScroll = WS_VSCROLL,
	HScroll = WS_HSCROLL,
	SYSMenu = WS_SYSMENU,
	ThickFrame = WS_THICKFRAME,
	Group = WS_GROUP,
	TabStop = WS_TABSTOP,
	MinimizeBox = WS_MINIMIZEBOX,
	MaximizeBox = WS_MAXIMIZEBOX,

	// ListBox
	LBS_MultiSel = LBS_MULTIPLESEL,
	LBS_ExtendedSel = LBS_EXTENDEDSEL,
	LBS_UseTabStops = LBS_USETABSTOPS,

	// Button
	BS_Bitmap = BS_BITMAP,
	BS_OwnerDraw = BS_OWNERDRAW,

	// Static
	SS_EndEllipsis = SS_ENDELLIPSIS,
	SS_PathEllipsis = SS_PATHELLIPSIS,
	SS_NoPrefix = SS_NOPREFIX,
	SS_LeftNoWordWrap = SS_LEFTNOWORDWRAP,
	SS_Center = SS_CENTER,
	SS_Right = SS_RIGHT,

	// Edit
	ES_MultiLine = ES_MULTILINE,
	ES_Password = ES_PASSWORD,
	ES_ReadOnly = ES_READONLY,

	// Treeview
	TVS_CheckBoxes = TVS_CHECKBOXES,
	TVS_FullRowSelect = TVS_FULLROWSELECT,
	TVS_TrackSelect = TVS_TRACKSELECT,

	// ?
	CCS_NoParentAlign = CCS_NOPARENTALIGN,
	CCS_NoResize = CCS_NORESIZE,

	// Tab
	TCS_ForceLeftAlign = TCS_FORCELABELLEFT,
	TCS_Buttons = TCS_BUTTONS,

	// ProgressBar
	PBS_Vertical = PBS_VERTICAL,

	// Month Calander
	MCS_MultiSelect = MCS_MULTISELECT,

	// ListView
	LVS_SingleSelect = LVS_SINGLESEL,
	LVS_ShowSelAlways = LVS_SHOWSELALWAYS,
	LVS_Report = LVS_REPORT,
	LVS_SmallIcon = LVS_SMALLICON,
	LVS_Icon = LVS_ICON,
	LVS_List = LVS_LIST,

	// ComboBox
	CBS_DropDown = CBS_DROPDOWN,
	CBS_DropDownList = CBS_DROPDOWNLIST,
	CBS_Simple = CBS_SIMPLE,

	// DateTime
	DTS_ShowNone = DTS_SHOWNONE
};

enum class WindowExStyle
	: DWORD
{
	None,
	DialogModalFrame = WS_EX_DLGMODALFRAME,
	ParentNotify = WS_EX_NOPARENTNOTIFY,
	TopMost = WS_EX_TOPMOST,
	AcceptFiles = WS_EX_ACCEPTFILES,
	Transparent = WS_EX_TRANSPARENT,
	MDIChild = WS_EX_MDICHILD,
	ToolWindow = WS_EX_TOOLWINDOW,
	WindowEdge = WS_EX_WINDOWEDGE,
	ControlParent = WS_EX_CONTROLPARENT,
	ClientEdge = WS_EX_CLIENTEDGE,
	Composited = WS_EX_COMPOSITED,
	Layered = WS_EX_LAYERED,
	NoActivate = WS_EX_NOACTIVATE
};

enum class WindowAnimStyle
	: DWORD
{
	None
};

template <EnumConcepts::IsNumeric T>
constexpr WindowStyle to_WindowStyle(_In_ T other) noexcept
{
	return gsl::narrow_cast<WindowStyle>(other);
}
template <EnumConcepts::IsNumeric T>
constexpr WindowExStyle to_WindowExStyle(_In_ T other) noexcept
{
	return gsl::narrow_cast<WindowExStyle>(other);
}

inline WindowStyle dcxGetWindowStyle(_In_ HWND Hwnd) noexcept
{
	return to_WindowStyle(GetWindowLong(Hwnd, GWL_STYLE));
}

inline WindowExStyle dcxGetWindowExStyle(_In_ HWND Hwnd) noexcept
{
	return to_WindowExStyle(GetWindowLong(Hwnd, GWL_EXSTYLE));
}

inline WindowStyle dcxSetWindowStyle(_In_ HWND Hwnd, _In_ WindowStyle style) noexcept
{
	return to_WindowStyle(SetWindowLongPtr(Hwnd, GWL_STYLE, gsl::narrow_cast<LONG>(style)));
}

inline WindowExStyle dcxSetWindowExStyle(_In_ HWND Hwnd, _In_ WindowExStyle style) noexcept
{
	return to_WindowExStyle(SetWindowLongPtr(Hwnd, GWL_EXSTYLE, gsl::narrow_cast<LONG>(style)));
}

/// <summary>
/// CreateWindowEx wrapper.
/// </summary>
/// <param name="ExStyles">- Window extended styles</param>
/// <param name="szClass">- Window class</param>
/// <param name="Styles">- Window styles</param>
/// <param name="rc">- The RECT defining the window area</param>
/// <param name="hParent">- The windows parent</param>
/// <param name="uID">- The windows ID, this is a size_t to better fit 64bit compiles.</param>
/// <param name="pthis">- The DcxControl object for this window.</param>
/// <returns>- The windows HWND on success or nullptr</returns>
inline HWND dcxCreateWindow(_In_ const WindowExStyle ExStyles, _In_opt_z_ const TCHAR* const szClass, _In_ const WindowStyle Styles, _In_ const RECT* const rc, _In_opt_ HWND hParent, _In_opt_ const size_t uID, _In_opt_ const void* const pthis = nullptr) noexcept
{
	if (!rc)
		return nullptr;

	return CreateWindowExW(
		gsl::narrow_cast<DWORD>(ExStyles),
		szClass,
		nullptr,
		gsl::narrow_cast<DWORD>(Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		hParent,
		reinterpret_cast<HMENU>(uID),
		GetModuleHandle(nullptr),
		const_cast<LPVOID>(pthis));
}

inline UINT dcxSetWindowID(_In_ HWND Hwnd, _In_ const UINT uID) noexcept
{
	return gsl::narrow_cast<UINT>(SetWindowLongPtr(Hwnd, GWLP_ID, gsl::narrow_cast<LONG>(uID)));
}
