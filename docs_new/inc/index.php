<?php

function get_intro_index() {
?>This DLL lets you add new controls to mIRC dialogs as well as change other features like the visual style, position etc...<br />

<div class="divpink">DCX does not support mIRC controls (NOT EVEN mIRC TABS), nor anything related to mIRC normal dialogs except it's menus. DCX does not work like MDX so please read the manual and see dcx.mrc and dcx_tools.mrc for examples.</div>

The DLL creates the controls directly on the mIRC dialog and you use clone functions defined by /xdid and $xdid which behave like the regular /did
and $did to interact with the controls.<br />
<br />
The event handling in the DLL is supported by a callback alias supplied by the user when the dialog is marked before adding DCX controls. The callback alias is called from within the DLL to ask for info (if you want to modify the default behavior in the DLL thus the "callback" type) and will supply the user with the events and parameters of these events.<br />
<br />
Every function in this DLL returns a value if used with the $dll identifier. [v]D_OK [function]: [message][/v] if successful, [v]D_ERROR [function]: [message][/v] otherwise.<br />
<br />
<a class="note">Note.</a> Mandatory parameters are expressed in [p][][/p] and optional parameters in [p]()[/p].<br />
<a class="note">Note.</a> I strongly suggest you load the <b>dcx_tools.mrc</b> file included in the zip as it contains the necessary <b>/xdialog</b> and <b>/xdid</b> aliases and identifiers needed to use the DLL.
<?php
//'
}

function intro2_index() {
	ob_start();
?>
The DCX supported controls are extended controls built directly on the mIRC dialog. These are used to extend the capabilities of the existing controls in mIRC dialogs which are very limited in use for GUI design.<br />
<br/>
<a class="note">Note.</a> Please note that some features of some DCX controls are only available to Windows XP+ machines. These commands have the symbol [o]OperatingSystem+[/o] beside their listing.
<?php
	$str = ob_get_clean();
	dcxdoc_print_description("DCX Supported Controls", $str);
}

function callback_index() {
	ob_start();
?><b>Overview</b><br />
Every DCX Control fires events upon user action on the control. These events are sent to an <a>alias</a> specified when the dialog is <a>Marked</a>.<br />
<br />
<b>Why are the events not sent like regular on *:DIALOG: events?</b><br />
Well for one thing, mIRC events are linear and static in the way that you cannot return any value to the event manager, to modify the default behavior of the event. Using callback support with an <a>alias</a> enables this type of interaction.<br />
<br />
<b>Callback Event Data</b><br />
<b>Format:</b> <a class="syntax">[NAME] [EVENT] [ID] [VALUE]</a>
<table>
<?php
	$cb_param = array(
		'NAME' => '$1 = Name of the dialog that the event came from.',
		'EVENT' => '$2 = Name of the event (eg. sclick, dclick, ...)',
		'ID' => '$3 = ID of the control that fired the event.',
		'VALUE' => '$4 = Additional information, event specific (usually $null, except specified otherwise in documentation)',
	);
	$cb_notes = array(
	    "<br />Some control events support return values to change their default behaviour. When returning special values during certain events, such as [r]nosize[/r] for [e]sizing[/e], interact with the DLL and modify the default event behavior. The events that support a return value will have a description of what the return value does when returned."
	);
	
	array_walk_recursive($cb_param, "wikiData");
	array_walk_recursive($cb_notes, "wikiData");
	
	format_parameters($cb_param);
	format_notes($cb_notes);
?>
</table>
<?php
	//'
	$str = ob_get_clean();
	dcxdoc_print_description("Callback Alias Events", $str);
}

function credits_index() {
	ob_start();
?>
<table style="table-layout: fixed;">
	<tr>
		<td style="width: 150px;"><a>Lead Developer</a></td>
		<td>ClickHeRe</td>
	</tr>
	<tr>
		<td><a>Supporting Developers</a></td>
		<td>
			hixxy<br />
			twig*
		</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Document Generator</a></td>
		<td>
			twig*
		</td>
	</tr>
	<tr>
		<td><a>Demo Script</a></td>
		<td>elizio</td>
	</tr>
	<tr>
		<td><a>General Testing</a></td>
		<td>bl0x</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Translations</a></td>
		<td>
			<table style="width: auto;">
				<tr>
					<td>Dutch</td>
					<td>Mpdreamz</td>
				</tr>
				<tr>
					<td>Finnish</td>
					<td>Mkz</td>
				</tr>
				<tr>
					<td>French</td>
					<td>dunkelzahn</td>
				</tr>
				<tr>
					<td>German</td>
					<td>Duplex</td>
				</tr>
				<tr>
					<td>Hebrew</td>
					<td>seec77</td>
				</tr>
				<tr>
					<td>Italian</td>
					<td><a href="mailto:clickhere@scriptsdb.org">Still need someone!</a></td>
				</tr>
				<tr>
					<td>Malaysian</td>
					<td>da^hype</td>
				</tr>
				<tr>
					<td>Portuguese (Brazil)</td>
					<td>Henrique</td>
				</tr>
				<tr>
					<td>Portuguese (Portugal)</td>
					<td>Theorem</td>
				</tr>
				<tr>
					<td>Spanish</td>
					<td>PaTaGoN</td>
				</tr>
				<tr>
					<td>Swedish</td>
					<td>Rixxon</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Support</a></td>
		<td>
			#scriptsdb.org @ Undernet<br />
			#scriptsdb.org @ Webnet<br />
			#genscripts @ VidGameChat
		</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Website</a></td>
		<td>
			<a href="http://scriptsdb.org">http://scriptsdb.org</a><br />
			<a href="http://dcx.scriptsdb.org">http://dcx.scriptsdb.org</a>
		</td>
	</tr>
</table>
<?php
	$str = ob_get_clean();
	dcxdoc_print_description("Credits", $str);
}


function get_general_index(&$GENERAL) {
	$GENERAL = array(
		"Mark" => array(
			'__desc' => "This command prepares the dialog for DCX-based commands.",
			'__cmd' => "[NAME] [CALLBACK_ALIAS]",
			'__eg' => "dlgDCX cb_alias",
			'__params' => array(
				'NAME' => "Dialog window name.",
				'CALLBACK_ALIAS' => "Callback Event alias name.",
			),
			'__notes' => array(
			    "This command must be called before every other command or else nothing will work.",
			),
		),
		"Version" => array(
			'__desc' => "Returns information on the DLL Version.",
			'__isid' => true,
		),
		"GetSystemColor" => array(
			'__desc' => "Returns the RGB value of a specific system color.",
			'__isid' => true,
			'__cmd' => "[ATTRIBUTE]",
			'__eg' => "COLOR_WINDOW",
			'__params' => array(
				'+ATTRIBUTE' => array(
					'__desc' => "The type of color. (From MSDN <a href='http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/getsyscolor.asp'>GetSysColor</a>)",
					'__values' => array(
						'COLOR_3DDKSHADOW' => "Dark shadow for three-dimensional display elements.",
						'COLOR_3DFACE' => "Face color for three-dimensional display elements and for dialog box backgrounds.",
						'COLOR_3DHIGHLIGHT' => "Highlight color for three-dimensional display elements (for edges facing the light source)",
						'COLOR_3DHILIGHT' => "Highlight color for three-dimensional display elements (for edges facing the light source)",
						'COLOR_3DLIGHT' => "Light color for three-dimensional display elements (for edges facing the light source)",
						'COLOR_3DSHADOW' => "Shadow color for three-dimensional display elements (for edges facing away from the light source)",
						'COLOR_ACTIVEBORDER' => "Active window border.",
						'COLOR_ACTIVECAPTION' => "Active window title bar. (Specifies the left side color in the color gradient of an active window's title bar if the gradient effect is enabled) [o]98+[/o]",
						'COLOR_APPWORKSPACE' => "Background color of multiple document interface (MDI) applications.",
						'COLOR_BACKGROUND' => "Desktop.",
						'COLOR_BTNFACE' => "Face color for three-dimensional display elements and for dialog box backgrounds.",
						'COLOR_BTNHIGHLIGHT' => "Highlight color for three-dimensional display elements (for edges facing the light source)",
						'COLOR_BTNSHADOW' => "Shadow color for three-dimensional display elements (for edges facing away from the light source)",
						'COLOR_BTNTEXT' => "Text on push buttons.",
						'COLOR_CAPTIONTEXT' => "Text in caption, size box, and scroll bar arrow box.",
						'COLOR_DESKTOP' => "Desktop.",
						'COLOR_GRADIENTACTIVECAPTION' => "Right side color in the color gradient of an active window's title bar. [o]98+[/o]",
						'COLOR_GRADIENTINACTIVECAPTION' => "Right side color in the color gradient of an inactive window's title bar. [o]98+[/o]",
						'COLOR_GRAYTEXT' => "Grayed (disabled) text. This color is set to 0 if the current display driver does not support a solid gray color.",
						'COLOR_HIGHLIGHT' => "Item(s) selected in a control.",
						'COLOR_HIGHLIGHTTEXT' => "Text of item(s) selected in a control.",
						'COLOR_HOTLIGHT' => "Color for a hyperlink or hot-tracked item. [o]98+[/o]",
						'COLOR_INACTIVEBORDER' => "Inactive window border.",
						'COLOR_INACTIVECAPTION' => "Inactive window caption. (Specifies the left side color in the color gradient of an inactive window's title bar if the gradient effect is enabled)",
						'COLOR_INACTIVECAPTIONTEXT' => "Color of text in an inactive caption.",
						'COLOR_INFOBK' => "Background color for tooltip controls.",
						'COLOR_INFOTEXT' => "Text color for tooltip controls.",
						'COLOR_MENU' => "Menu background.",
						'COLOR_MENUHILIGHT' => "The color used to highlight menu items when the menu appears as a flat menu. The highlighted menu item is outlined with [v]COLOR_HIGHLIGHT[/v]. [o]XP+[/o]",
						'COLOR_MENUBAR' => "The background color for the menu bar when menus appear as flat menus. However, [v]COLOR_MENU[/v] continues to specify the background color of the menu popup. [o]XP+[/o]",
						'COLOR_MENUTEXT' => "Text in menus.",
						'COLOR_SCROLLBAR' => "Scroll bar gray area.",
						'COLOR_WINDOW' => "Window background.",
						'COLOR_WINDOWFRAME' => "Window frame.",
						'COLOR_WINDOWTEXT' => "Text in windows.",
					),
				),
			),
		),
		"ColorDialog" => array(
			'__desc' => "Opens up the color picker dialog and returns the selected color.",
			'__cmd' => "(DEFAULT) (STYLES)",
			'__eg' => //array(
// EXAMPLE
				"\$rgb(255,0,0) fullopen owner dcx anycolor",
//				"\$rgb(0,0,255) owner \$window(@test).hwnd solidonly",
//			),
            '__isid' => true,
			'__params' => array(
				'DEFAULT' => "The color selected when the dialog opens.",
				'STYLES' => array(
					'__desc' => "Various options for the color dialog.",
					'__values' => array(
						'anycolor' => "Displays all available colors in the set of basic colors.",
						'fullopen' => "Displays the additional controls that allow the user to create custom colors.",
						'nofullopen' => "Disables the <b>Define Custom Color</b> button.",
						'solidonly' => "Display only solid colors in the set of basic colors.",
						'owner' => "Allows you to specific the parent control. (Must also specify [v]DNAME[/v] or [v]HWND[/v])",
					),
				),
			),
			'__notes' => array(
			    '<br />By default, the main mIRC window is the parent. Dialog name OR a hWnd MUST follow after the [s]owner[/s] style. Enter 0 for the hWnd to make it modeless.<br />
To get hWnd values, use $window().hwnd, $dialog().hwnd, $chan().hwnd, $chat().hwnd, $get().hwnd, $query().hwnd, $send().hwnd, and $timer().hwnd',
			),
		),
		'OpenDialog/SaveDialog' => array(
			'__desc' => "Opens up the Open/Save file dialog and returns the selected file.",
			'__cmd' => '(STYLES) $chr(9) (FILENAME) $chr(9) (FILTER)',
			'__eg' => //array(
// EXAMPLE
				'showhidden filemustexist hidereadonly $chr(9) c:\boot.ini $chr(9) All Files (*.*)|*.*',
//				'createprompt overwriteprompt owner dcx $chr(9) c:\blah.txt $chr(9) All Files (*.*)|*.*|Music Files|*.mp3;*.wav;*.ogg',
//			),
            '__isid' => true,
			'__params' => array(
				'STYLES' => array(
					'__desc' => "Various options for the color dialog.",
					'__values' => array(
						'createprompt' => "If the user enters a filename that does not exist, the dialog asks the user if they want to create it. Returns the filename (without creating it) if they click yes. (OpenDialog only)",
						'enablesizing' => "The dialog is resizable.",
						'filemustexist' => "If the file doesn't exist in the current directory, the user will get an error prompt. (OpenDialog only)",
						'showhidden' => "Forces the showing of system and hidden files, overriding the user setting to show or not show hidden files. A file that is marked both system and hidden is not shown. [o]XP+[/o]",
						'noreadonly' => 'Hides the "Open as Read Only" checkbox.',
						'nochangedir' => 'Restores the current directory to its original value if the user changed the directory while searching for files. (SaveDialog only)',
						'multisel' => 'Allows user to select multiple files. (OpenDialog only)',
						'getshortcuts' => 'Allows dialog box to return the path and file name of the selected shortcut (.LNK files)',
						'nonetwork' => 'Hides the network button.',
						'norecent' => "Does not add a link to the selected file in the file system directory that contains the user's most recently used documents.",
						'novalidate' => "The dialog does not validate filenames to check if they're legal.",
						'overwriteprompt' => 'If the user selects a file that already exists, the dialog asks the user if they want to overwrite it. Returns the filename (without clearing) if they click yes. (SaveDialog only)',
						'pathmustexist' => "The dialog does not validate filenames to check if they're legal.",
						'owner' => "Allows you to specific the parent control. (Must also specify [v]DNAME[/v] or [v]HWND[/v])",
					),
				),
				'FILENAME' => "If you specify this parameter, it will be the filename in the box when the dialog opens.",
				'FILTER' => 'The FILTER parameter is the type of files the user can select, it should be formatted like "Description|Type|Description|Type"',
			),
			'__notes' => array(
				'Returns "" when cancelled.',
				"[v]filemustexist[/v] automatically applies [v]pathmustexist[v] when style used.",
				"[v]multisel[/v] returns files in this format [PATH]|[FILE1]|[FILE2]|..., where the first token will be the path, and the files are seperated by the | (pipe) character.",
			    '<br />By default, the main mIRC window is the parent. Dialog name OR a hWnd MUST follow after the [s]owner[/s] style. Enter 0 for the hWnd to make it modeless.<br />
To get hWnd values, use $window().hwnd, $dialog().hwnd, $chan().hwnd, $chat().hwnd, $get().hwnd, $query().hwnd, $send().hwnd, and $timer().hwnd',
			),
		),
		'GetTaskbarPos' => array(
			'__desc' => "Retreives the position of the taskbar on your desktop.",
			'__isid' => true,
			'__notes' => array(
				'Returns X Y W H when successful.',
				"Returns 'D_ERROR: GetTaskbarPos' when unsuccessful.",
			),
		),
	);
}


function get_xdialog_index(&$XDIALOG) {
	global $CLA;
	
	$XDIALOG = array(
		"a" => array(
			'__desc' => "This command lets you animate an opening/closing dialog.",
			'__cmd' => "[+FLAGS] [DELAY]",
			'__eg' => "+ab 1000",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Animation style flags.",
					'__values' => array(
						'a' => "Shows the dialog.",
						'b' => "Alpha-Blending fade effect.",
						'c' => "Makes the window appear to collapse inward if [v]+h[/v] is used or expand outward if the [v]+a[/v] is used. The various direction flags have no effect.",
						'h' => "Hides the dialog.",
						'o' => "Animates the window from left to right. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v] or [v]+a[/v].",
						'n' => "Animates the window from right to left. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v] or [v]+a[/v].",
						's' => "Uses slide animation. It is ignored when used with [v]+c[/v].",
						'v' => "Animates the window from top to bottom. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v] or [v]+a[/v].",
						'u' => "Animates the window from bottom to top. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v] or [v]+a[/v].",
					),
				),
				'DELAY' => "Delay in <b>millseconds</b> of the animation. (typical system value is 200)",
			),
			'__notes' => array(
			    "The default effect is a roll animation.",
			),
		),
		"b" => array(
			'__desc' => "This command lets modify border and style attributes on a dialog.",
			'__cmd' => "[+FLAGS]",
			'__eg' => "+tyszh",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Border/Window style flags.",
					'__values' => array(
						'b' => "Regular Border.",
						'c' => "Clientedge border.",
						'd' => "Dialog frame border.",
						'f' => "Dialog modal frame border. (Similar to [v]+d[/v])",
						'h' => "Context help <b>?</b> button. (Needs [v]+y[/v] and can't be used with [v]+m[/v] or [v]+n[/v])",
						'm' => "Maximize button. (Needs [v]+y[/v] and can't be used with [v]+m[/v] or [v]+n[/v])",
						'n' => "Minimize button. (Needs [v]+y[/v] and can't be used with [v]+m[/v] or [v]+n[/v])",
						'o' => "Tool window.",
						's' => "Staticedge border.",
						't' => "Has a titlebar.",
						'w' => "Windowedge border.",
						'y' => "Sysmenu (the little X button)",
						'z' => "Resizeable border.",
					),
					'DELAY' => "Delay in <b>millseconds</b> of the animation. (typical system value is 200)",
				),
			),
		),
		"c" => array(
			'__desc' => "This command lets you create a DCX control on your dialog.",
			'__cmd' => "[ID] [TYPE] [X] [Y] [W] [H] (OPTIONS)",
			'__eg' => "2 pbar 140 30 100 20 smooth notheme",
			'__params' => array(
			    'ID' => "Dialog control ID that will serve as a host for the DCX Control.",
				'TYPE' => array(
					'__desc' => "The type of DCX Control to be created.",
					'__values' => array(
						'pbar' => "Creates a Progressbar control.",
						'treeview' => "Creates a TreeView control.",
						'toolbar' => "Creates a Toolbar control.",
						'statusbar' => "Creates a Statusbar control.",
						'comboex' => "Creates a ComboEx control.",
						'listview' => "Creates a Listview control.",
						'trackbar' => "Creates a Trackbar control.",
						'rebar' => "Creates a Rebar control.",
						'colorcombo' => "Creates a ColorCombo control.",
						'button' => "Creates a Button control.",
						'richedit' => "Creates a RichEdit control.",
						'ipaddress' => "Creates a IpAddress control.",
						'updown' => "Creates an UpDown control.",
						'webctrl' => "Creates a Web control.",
						'&nbsp;' => '&nbsp;',
						'line' => "Creates a Line control.",
						'box' => "Creates a Box control.",
						'radio' => "Creates a Radio control.",
						'check' => "Creates a Check control.",
						'edit' => "Creates a Edit control.",
						'scroll' => "Creates a Scroll control.",
						'image' => "Creates a Image control.",
						'list' => "Creates a List control.",
						'link' => "Creates a Link control.",
						'text' => "Creates a Text control.",
						'&nbsp;&nbsp;' => '&nbsp;',
						'divider' => "Creates a Divider control.",
						'panel' => "Creates a Panel control.",
						'tab' => "Creates a Tab control.",
						'&nbsp;&nbsp;' => '&nbsp;',
						'window' => "Docks a mIRC @window. The first parameter in the [p](OPTIONS)[/p] must be the @window name.",
						'dialog' => "Docks a mIRC dialog. The first parameter in the [p](OPTIONS)[/p] must be the dialog name.",
					),
				),
				'X' => "X position of control.",
				'Y' => "Y position of control.",
				'W' => "Width of control.",
				'H' => "Height of control.",
				'OPTIONS' => "Optional styles and options available on each individual controls.",
			),
		),
		"d" => array(
			'__desc' => "This command lets you delete a DCX control on the dialog.",
			'__cmd' => "[ID]",
			'__eg' => "15",
		),
		"f" => array(
			'__desc' => "This command lets you flash a dialog window.",
			'__cmd' => "[+FLAGS] [COUNT] [TIMEOUT]",
			'__eg' => "+at 0 1000",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Flash style flags.",
					'__values' => array(
						'a' => "Flash both the window caption and taskbar button. This is equivalent in using the [v]+c[/v] and [v]+r[/v] flags.",
						'c' => "Flash the window caption.",
						'f' => "Flash continuously until the window comes to the foreground.",
						'r' => "Flash the taskbar button.",
						's' => "Stop flashing. The system restores the window to its original state.",
						't' => "Flash continuously, until the [v]+s[/v] flag is used.",
					),
				),
				'COUNT' => "Number of times to flash the window.",
				'TIMEOUT' => "Rate at which the window is to be flashed, in <b>milliseconds</b>.",
			),
			'__notes' => array(
			    "If [p]TIMEOUT[/p] is zero, the function uses the default cursor blink rate.",
			),
		),
		"g" => array(
			'__desc' => "This command lets you modify the background attributes of the dialog.",
			'__cmd' => "[+FLAGS] [EXTRA]",
			'__eg' => "+b \$rgb(0,0,255)",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Background flags.",
					'__values' => array(
						'b' => "Specifies background color.",
						'i' => 'Specifies background image.',
						'c' => 'Center the background image. (Used with [v]+i[/v] flag)',
						's' => 'Stretch the background image. (Used with [v]+i[/v] flag)',
						't' => 'Tile the background image. (Used with [v]+i[/v] flag)',
						'n' => 'Reset the background image position. (Used with [v]+i[/v] flag)',
					),
				),
				'EXTRA' => 'If [p]+FLAGS[/p] is [v]+b[/v], then [p]EXTRA[/p] specifies the background color in a $rgb() format.<br />
				If [p]+FLAGS[/p] is used with [v]+i[/v], then [p]EXTRA[/p] specfies the BMP filename.'
			),
			'__notes' => 'Only bitmap (BMP) files can be loaded for the background image.'
		),
		"h" => array(
			'__desc' => "This command lets you hide the dialog.",
		),
		"j" => array(
			'__desc' => "This command redraws of all the controls in a dialog.",
		),
		"l" => $CLA,
		"m" => array(
			'__desc' => "This command lets you maximize the dialog.",
		),
		"n" => array(
			'__desc' => "This command lets you minimize the dialog.",
		),
		"q" => array(
			'__desc' => "This command lets you change the dialog cursor.",
			'__cmd' => "[+FLAGS] [CURSOR|FILENAME]",
			'__eg' => "+r wait",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Cursor flags.",
					'__values' => array(
						'f' => "Cursor is defined in [p]FILENAME[/p]",
						'r' => "Cursor is a system cursor name in the list below.",
					),
				),
				'CURSOR' => array(
                    '__desc' => "Cursor system names",
					'__values' => array(
						'appstarting' => "Standard arrow and small hourglass",
						'arrow' => "Standard arrow.",
						'cross' => "Crosshair.",
						'hand' => "Hand.",
						'help' => "Arrow and question mark.",
						'ibeam' => "I-beam.",
						'no' => "Slashed circle.",
						'sizeall' => "Four-pointed arrow pointing north, south, east, and west.",
						'sizenesw' => "Double-pointed arrow pointing northeast and southwest.",
						'sizens' => "Double-pointed arrow pointing north and south.",
						'sizenwse' => "Double-pointed arrow pointing northwest and southeast.",
						'sizewe' => "Double-pointed arrow pointing west and east.",
						'uparrow' => "Vertical arrow.",
						'wait' => "Hourglass.",
					),
				),
				'FILENAME' => "Filename containing the cursor.",
			),
			'__notes' => array(
			    "Cursor files need to be .cur or .ani.",
			),
		),
		"r" => array(
			'__desc' => "This command lets you restore the dialog.",
		),
		"s" => array(
			'__desc' => "This command lets you show the dialog.",
		),
		"t" => array(
			'__desc' => "This command allows you to set the transparency color for the background image.",
			'__cmd' => '[COLOR]',
			'__eg' => '$rgb(255,0,255)',
		),
		"x" => array(
			'__desc' => "This command lets you kill (not trigger a close like /dialog -x) the dialog.",
			'__notes' => array(
			    "You will have to use this command to close a dialog if you return [r]noclose[/r] in the [e]close[/e] event. mIRC's /dialog -x will be ineffective to close the dialog."
			),
		),
	);
}

function get_xdid_index(&$XDID) {
	$XDID = array(
		"b" => array(
			'__desc' => "This command lets you disable the control.",
		),
		"e" => array(
			'__desc' => "This command lets you enable the control.",
		),
		"f" => array(
			'__desc' => "This command lets you change a control font.",
			'__cmd' => "[+FLAGS] [CHARSET] [SIZE] [FONTNAME]",
			'__eg' => "+abs 10 Tahoma",
			'__params' => array(
			    '+FLAGS' => array(
			        '__desc' => "Font style flags.",
                    '__values' => array(
                        'a' => "Font is anti-aliased.",
						'b' => "Font is bold.",
						'd' => "Default font.([CHARSET], [SIZE] and [FONTNAME] can be omitted)",
						'i' => "Font is italic.",
						's' => "Font is strikedout.",
						'u' => "Font is underlined.",
					),
				),
				'CHARSET' => "Can be any of the following: ansi, baltic, chinesebig, default, easteurope, gb2312, greek, hangul, mac, oem, russian, shiftjis, symbol, turkish, vietnamese.",
				'SIZE' => "Font size in points.",
				'FONTNAME' => "Font name. Ex: Tahoma, Courier New, Arial, ...",
			),
			'__notes' => array(
				"Font names are limited to 31 chars (system definition)."
			),
		),
		"h" => array(
			'__desc' => "This command lets you hide the control.",
		),
		"p" => array(
			'__desc' => "This command lets you move/resize the control.",
			'__cmd' => "[X] [Y] [W] [H]",
			'__eg' => "10 10 200 120",
			'__params' => array(
				'X' => "X position of control.",
				'Y' => "Y position of control.",
				'W' => "Width of control.",
				'H' => "Height of control.",
			),
			'__notes' => array(
				"This command will send a [e]sizing[/e] event.",
			),
		),
		"q" => array(
			'__desc' => "This command lets you change the dialog cursor.",
			'__cmd' => "[+FLAGS] [CURSOR|FILENAME]",
			'__eg' => "+r wait",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Cursor flags.",
					'__values' => array(
						'f' => "Cursor is defined in [p]FILENAME[/p]",
						'r' => "Cursor is a system cursor name in the list below.",
					),
				),
				'CURSOR' => array(
                    '__desc' => "Cursor system names",
					'__values' => array(
						'appstarting' => "Standard arrow and small hourglass",
						'arrow' => "Standard arrow.",
						'cross' => "Crosshair.",
						'hand' => "Hand.",
						'help' => "Arrow and question mark.",
						'ibeam' => "I-beam.",
						'no' => "Slashed circle.",
						'sizeall' => "Four-pointed arrow pointing north, south, east, and west.",
						'sizenesw' => "Double-pointed arrow pointing northeast and southwest.",
						'sizens' => "Double-pointed arrow pointing north and south.",
						'sizenwse' => "Double-pointed arrow pointing northwest and southeast.",
						'sizewe' => "Double-pointed arrow pointing west and east.",
						'uparrow' => "Vertical arrow.",
						'wait' => "Hourglass.",
					),
				),
				'FILENAME' => "Filename containing the cursor.",
			),
			'__notes' => array(
			    "Cursor files need to be .cur or .ani.",
			    "You can't change the cursor on the divider control.",
			),
		),
		"s" => array(
			'__desc' => "This command lets you show the control.",
		),
		"x" => array(
			'__desc' => "This command lets you modify the border styles of the control.",
			'__cmd' => "[+FLAGS]",
			'__eg' => "+w",
			'__params' => array(
				'+FLAGS' => array(
			        '__desc' => "Border style flags.",
                    '__values' => array(
                        'b' => "Regular Border.",
						'c' => "Clientedge border.",
						'd' => "Dialog frame border.",
						'f' => "Dialog modal frame border. (Similar to [v]+d[/v])",
						's' => "Staticedge border.",
						'w' => "Windowedge border.",
					),
				),
			),
		),
		"C" => array(
			'__desc' => "This command lets you modify the colors of the control.",
			'__cmd' => "[+FLAGS] [COLOR]",
			'__eg' => "+bk $rgb(0,0,255)",
			'__params' => array(
				'+FLAGS' => array(
			        '__desc' => "Color Flags.",
                    '__values' => array(
                        'b' => "Background Color.<br />Affects: Edit, Radio, Check, Box, Scroll, Text, List, Panel, Image",
						'k' => "Text Background Color.<br />Affects: Text, Edit, List, Box (Border Color)",
						't' => "Text Color.<br />Affects: Text, Edit, List, Box",
					),
				),
			),
		),
		"F" => array(
			'__desc' => "This command lets you set the focus on the control.",
		),
		"M" => array(
			'__desc' => "This command lets you add additional information to a control that can be retrieved later.",
			'__cmd' => "[INFO]",
			'__eg' => "Mark info here",
		),
		"U" => array(
			'__desc' => "This command lets you remove focus from the control.",
		),
		"Z" => array(
			'__desc' => "This command lets you scroll a control in the vertical direction where [p]POS[/p] is a percentage expressing the area to scroll (0 being top and 100 being bottom).",
			'__cmd' => "[POS]",
			'__eg' => "50",
		),
	);
}


function get_events_index(&$EVENTS) {
	$EVENTS = array(
        "close" => array(
            '__desc' => "When the X button is clicked on a dialog.",
            '__notes' => array(
                "You will have to use /xdialog -x to close the dialog instead of mIRC's /dialog -x if you return [r]noclose[/r] on this event.",
			),
			'__return' => "[r]noclose[/r] Return this value to block the window closing.",
		),
		"beginsize" => array(
			'__desc' => "When the dialog is about to be resized.",
			'__return' => "[r]nosize[/r] Return this value to block the window resizing.",
		),
		"sizing" => array(
			'__desc' => 'When the dialog is being resized where $3 is the new width and $4 is the new height.',
			'__return' => "[r]nosize[/r] Return this value to block the window resizing.",
		),
		"endsize" => "When the dialog has finished being resized.",
		"beginmove" => array(
			'__desc' => "When the dialog is about to be moved.",
			'__return' => "[r]nomove[/r] Return this value to block the window moving.",
		),
		"moving" => "When the dialog is being moved.",
		"endmove" => "When the dialog has finished being moved.",
		"mouse" => "When the mouse hovers a control where $3 is the ID of the control (dialog: ID of 0).",
		"mouseenter" => 'When the mouse enters a control where $3 is the ID of the control (dialog: ID of 0).',
		"mouseleave" => 'When the mouse leaves a control where $3 is the ID of the control (dialog: ID of 0).',
		"sclick" => "When the left mouse button is clicked.",
		"rclick" => "When the right mouse button is clicked.",
		"dclick" => "When the left mouse button is double-clicked.",
		"lbdown" => "When the left mouse button is pressed.",
		"lbup" => "When the left mouse button is released.",
		"lbdblclk" => "When the left mouse button is double-clicked.",
		"rbdown" => "When the right mouse button is pressed.",
		"rbup" => "When the right mouse button is released.",
		"rbdblclk" => "When the right mouse button is double-clicked.",
		"mbdown" => "When the middle mouse button is pressed.",
		"mbup" => "When the middle mouse button is released.",
		"mbdblclk" => "When the middle mouse button is double-clicked.",
		"focus" => 'Triggered when a control gains the focus where $3 is the control ID.',
		"focusout" => 'Triggered when a control loses the focus where $3 is the control ID.',
		"min" => array(
			'__desc' => "When the dialog is about to be minimized.",
			"__return" => "[r]stop[/r] Return this value to block the window minimizing.",
		),
		"max" => array(
			'__desc' => "When the dialog is about to be maximized.",
			"__return" => "[r]stop[/r] Return this value to block the window maximizing.",
		),
		"restore" => "When the dialog is about to be restored.",
		'dragbegin' => array(
		    '__desc' => "Triggers when one or more files are dropped onto the control.",
		    '__cmd' => 'COUNT',
		    '__eg' => '3',
            '__params' => array(
            	'COUNT' => "Total number of files dragged into the control.",
			),
			'__return' => '[r]cancel[/r] to stop the drag drop events from occuring.',
		),
		'dragfile' => array(
			'__desc' => "The filename of the file that has",
			'__cmd' => 'FILENAME',
			'__eg' => 'C:\mIRC\blah.txt',
		),
		'dragfinish' => "Event triggered when processing of drag drop is complete.",
	);
}




function get_xdialogprops_index(&$XDIALOGPROPS) {
	$XDIALOGPROPS = array(
	    "isid" => array(
			'__desc' => "This property retrieves if the given ID exists in the dialog.",
			'__cmd' => "ID",
			'__eg' => "4",
		),
		"nextid" => array(
			'__desc' => "This property retrieves the next free ID in the dialog.",
		),
		"id" => array(
			'__desc' => "This property retrieves the next free ID in the dialog.",
			'__cmd' => "N",
			'__eg' => "5",
		),
		"ismenu" => array(
			'__desc' => "This property retrieves if the dialog displays a menu or not.",
		),
		"mouseid" => array(
			'__desc' => "This property retrieves which control the mouse currently hovers.",
		),
		"focusid" => array(
			'__desc' => "This property retrieves which control currently has the focus.",
		),
		"mouse" => array(
			'__desc' => "This property retrieves the mouse position from the dialog viewpoint.",
		),
		"key" => array(
			'__desc' => "This property retrieves the selected state of certain input keys during events.",
			'__params' => array(
				'KEY_NUM' => array(
					'__desc' => "Virtual key.",
					'__values' => array(
						'1' => "LEFT Mouse Button",
						'2' => "RIGHT Mouse Button",
						'4' => "MIDDLE Mouse Button",
						'8' => "Left SHIFT",
						'16' => "Left CONTROL",
						'32' => "Left ALT",
						'64' => "Right SHIFT",
						'128' => "Right CONTROL",
						'256' => "Right ALT",
					),
				),
			),
			'__notes' => array(
			    "The return value is binary, which means that you have to use the following code fragment to know if the key identified by the listed number below is currently selected :<br />
<pre>if (\$xdialog(dname).key &amp; KEY_NUM) {
  ; do action here
}</pre>",
			),
		),
	);
}


function get_xdidprops_index(&$XDIDPROPS) {
	$XDIDPROPS = array(
	    "enabled" => array(
			'__desc' => "This property retrieves if a control is enabled or disabled.",
		),
		"visible" => array(
			'__desc' => "This property retrieves if a control is visible or hidden.",
		),
		"pos" => array(
			'__desc' => "This property retrieves the position and size of a control in the format [X] [Y] [W] [H] relative to the parent (which can be the dialog).",
		),
		"dpos" => array(
			'__desc' => "This property retrieves the position and size of a control in the format [X] [Y] [W] [H] relative to the dialog.",
		),
		"mark" => array(
			'__desc' => "This property retrieves the additional info given to a control with /xdid -M.",
		),
		"hwnd" => array(
			'__desc' => "This property retrieves the window handle (HWND) of a control.",
		),
		"mouse" => array(
			'__desc' => "This property retrieves the mouse position from the control viewpoint.",
		),
		"pid" => array(
			'__desc' => "This property retrieves the control ID of the parent of a control (0 if a dialog is the parent).",
		),
		"type" => array(
			'__desc' => "This property retrieves the control type.",
		),
	);
}


function get_styles_index(&$STYLES) {
	$STYLES = array(
		"tabstop" => "Specifies a control that can receive the keyboard focus when the user presses the TAB key. Pressing the TAB key changes the keyboard focus to the next control with the [s]tabstop[/s] style.",
		"group" => "Specifies the first control of a group of controls. The group consists of this first control and all controls defined after it, up to the next control with the [s]group[/s] style. The first control in each group usually has the tabstop style so that the user can move from group to group. The user can subsequently change the keyboard focus from one control in the group to the next control in the group by using the direction keys.",
		"disabled" => "The control is created disabled.",
		"notheme" => "Removes XP Themes on controls.",
	);
}

?>
