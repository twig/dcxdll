<?php
function dcx_load($page) {
	global $GENERAL;
	
	loadSection($GENERAL, "get_general_$page");
}

function dcx_unload() {
	global $GENERAL;
	
	$GENERAL = array();
}


function dcx_layout($page, $pagelabel) {
	global $SECTION, $GENERAL;
	
	dcxdoc_print_intro($page);
	
	// general commands
	if ($GENERAL) {
		$SECTION = SECTION_GENERAL;
		$count = 1;

		dcxdoc_print_description("General Commands", "These commands are general DCX commands and not oriented on any particular DCX controls.");

		foreach ($GENERAL as $cmd => $data) {
       		dcxdoc_format_general($cmd, $data, $count);
      		$count++;
		}
	}
}




function get_general_dcx(&$GENERAL) {
	$GENERAL = array(
		"ActiveWindow" => array(
			'__desc' => "Returns the size and title of the active window.",
			'__isid' => true,
			'__cmd' => "[PROP]",
			'__eg' => "caption",
			'__params' => array(
				'caption' => 'Returns the caption of the active window.',
				'hwnd' => 'Returns the handle for the active window.',
				'x' => "The left position of the active window.",
				'y' => "The bottom position of the active window.",
				'w' => "The width of the active window.",
				'h' => "The height of the active window.",
			),
		),
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
			'__eg' => array(
				'$rgb(255,0,0) fullopen owner dcx anycolor',
				'$rgb(0,0,255) owner $window(@test).hwnd solidonly',
			),
			'__isid' => true,
			'__params' => array(
				'DEFAULT' => "The color selected when the dialog opens.",
				'STYLES' => array(
					'__desc' => "Various options for the color dialog.",
					'__values' => array(
						'anycolor' => "Displays all available colors in the set of basic colors.",
						'fullopen' => "Displays the additional controls that allow the user to create custom colors.",
						'nofullopen' => "Disables the <b>Define Custom Color</b> button.",
						'returndefault' => "Returns the default color if cancel is clicked.",
						'solidonly' => "Display only solid colors in the set of basic colors.",
						'owner' => "Allows you to specific the parent dialog/window. (Must also specify [v]DNAME[/v] or [v]HWND[/v])",
					),
				),
			),
			'__notes' => array(
			    '<br />By default, the main mIRC window is the parent. Dialog name OR a hwnd MUST follow after the [s]owner[/s] style.<br />
To get hWnd values, use $window().hwnd, $dialog().hwnd, $chan().hwnd, $chat().hwnd, $get().hwnd, $query().hwnd, $send().hwnd, and $timer().hwnd',
			),
		),
		'OpenDialog/SaveDialog' => array(
			'__desc' => "Opens up the Open/Save file dialog and returns the selected file.",
			'__cmd' => '(STYLES) $chr(9) (FILENAME) $chr(9) (FILTER)',
			'__eg' => array(
				'showhidden filemustexist hidereadonly $chr(9) c:\boot.ini $chr(9) All Files (*.*)|*.*',
				'createprompt overwriteprompt owner dcx $chr(9) c:\blah.txt $chr(9) All Files (*.*)|*.*|Music Files|*.mp3;*.wav;*.ogg',
			),
            '__isid' => true,
			'__params' => array(
				'STYLES' => array(
					'__desc' => "Various options for the Open/Save file dialog.",
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
						'owner' => "Allows you to specific the parent dialog/window. (Must also specify [v]DNAME[/v] or [v]HWND[/v])",
					),
				),
				'FILENAME' => "If you specify this parameter, it will be the filename in the box when the dialog opens.",
				'FILTER' => 'The FILTER parameter is the type of files the user can select, it should be formatted like "Description|Type|Description|Type"',
			),
			'__notes' => array(
				'Returns $null when cancelled.',
				"[v]filemustexist[/v] automatically applies [v]pathmustexist[/v] when style used.",
				"[v]multisel[/v] returns files in this format [PATH]|[FILE1]|[FILE2]|..., where the first token will be the path, and the files are seperated by the | (pipe) character.",
                                'By default, the main mIRC window is the parent. Dialog name OR a hWnd MUST follow after the [s]owner[/s] style.<br />
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
                
                'GhostDrag' => array(
                        '__desc' => 'This command allows for ghosted dragging for the main mIRC window.',
                        '__cmd' => '[VALUE]',
                        '__eg' => '150',
                        '__params' => array(
                            'VALUE' => 'The opacity of the main mIRC window when dragged. [p]VALUE[/p] has a range of [v]0[/v]-[v]255[/v].',
                        ),
                ),
        'IsUnloadSafe' => array(
            '__desc' => 'This is used by /udcx to determine if /dll -u can be used or not.',
            '__isid' => true,
        ),
		'FontDialog' => array(
			'__desc' => "Opens up the Font dialog and returns the selected font options.",
			'__cmd' => '[(PARAM) (VALUE) [$chr(9) (PARAM2) (VALUE2) [$chr(9) (PARAM3) (VALUE3)]]]',
			'__eg' => 'default +is ansi 9 Verdana $chr(9) color $rgb(255,0,0) $chr(9) owner dname $chr(9) flags +eh',
            '__isid' => true,
			'__params' => array(
				'default' => array(
					'__desc' => "Allows you to specify the default font selected by the font dialog. Value must be in the following format:<br />[p]+flags charset size fontname[/p]",
					'__values' => array(
						'+flags' => 'Font style flags. These affect the "default values" of the font selected when the FontDialog is first shown.
<table>
	<tr>
		<td class="value">a</td>
		<td>Font is anti-aliased.</td>
	</tr>
	<tr>

		<td class="value">b</td>
		<td>Font is bold.</td>
	</tr>
	<tr>
		<td class="value">d</td>
		<td>Default font.</td>
	</tr>

	<tr>
		<td class="value">i</td>
		<td>Font is italic.</td>
	</tr>
	<tr>
		<td class="value">s</td>
		<td>Font is strikedout.</td>

	</tr>
	<tr>
		<td class="value">u</td>
		<td>Font is underlined.</td>
	</tr>
</table>',
						'charset' => "Can be any of the following: ansi, baltic, chinesebig, default, easteurope, gb2312, greek, hangul, mac, oem, russian, shiftjis, symbol, turkish, vietnamese.",
						'size' => "Font size in points.",
						'fontname' => "Font name.",
					),
				),
				'flags' => array(
					'__desc' => "Allows you to specify flags to modify options for the actual font dialog.",
					'__values' => array(
						'a' => 'Prevent the dialog box from displaying an initial selection for the font name combo box.',
						'b' => 'Allows selection of fonts for all non-OEM and Symbol character sets, as well as the ANSI character set.',
						'c' => 'Allow only the selection of scalable fonts. (Vector fonts, scalable printer fonts, TrueType fonts, and fonts scaled by other technologies.)',
						'e' => 'Display the controls that allow the user to specify strikeout, underline, and text color options.',
						'f' => 'Indicates an error condition if the user attempts to select a font or style that does not exist.',
						'h' => 'Disables the Script (charset) combo box.',
						'i' => 'Dialog should not allow graphics device interface (GDI) font simulations. (Not really sure what it does)',
						'm' => 'Limits font selections to a specific charset.',
						'n' => "List only the fonts supported by the printer.",
						'p' => "Select only fixed-pitch fonts.",
						'r' => 'List only horizontally oriented fonts.',
						's' => "List only the screen fonts supported by the system. (default)",
						't' => 'Only list and allow the selection of TrueType fonts.',
						'v' => "Will not allow vector font selections.",
						'w' => "Allow only the selection of fonts available on both the printer and the display.",
						'y' => 'Prevent the dialog box from displaying an initial selection for the font style combo box. (Bold, italic, etc)',
						'z' => "Prevent the dialog box from displaying an initial selection for the font size combo box.",
				    ),
				),
				'color' => array(
					'__desc' => "Allows you to specify the default color (in RGB) to the font dialog.",
					'__values' => array(
						'rgb' => 'Value of the color in $rgb()',
					),
				),
				'minmaxsize' => array(
					'__desc' => "Allows you to specify the selectable font size range in the font dialog.",
					'__values' => array(
						'min' => 'Minimum font value.',
						'max' => 'Maximum font value.',
					),
				),
				'owner' => "Allows you to specific the parent dialog/window. (Must also specify [v]DNAME[/v] or [v]HWND[/v])",
			),
			'__return' => 'The information returned is in the following format: [v]RGB[/v] [v]+FLAGS[/v] [v]CHARSET[/v] [v]SIZE[/v] [v]FONTNAME[/v]',
			'__notes' => array(
				'Returns $null when cancelled.',
				"You do not have to specify all parameters, only the ones you wish to change.",
				'The parameter [p]minmaxsize[/p] is limited to a range of 8-72. Any sizes other than that will be ignored.',
                                'By default, the main mIRC window is the parent. Dialog name OR a hWnd MUST follow after the [s]owner[/s] style.<br />
To get hWnd values, use $window().hwnd, $dialog().hwnd, $chan().hwnd, $chat().hwnd, $get().hwnd, $query().hwnd, $send().hwnd, and $timer().hwnd',
			),
		),
		'IsThemedXP' => array(
			'__desc' => 'Returns [v]$true[/v] or [v]$false[/v] depending on whether a visual style has been on the Windows system or not. [o]XP+[/o]',
			'__isid' => true,
		),
        'IsUsingDirectX' => array(
            '__desc' => 'Returns [v]$true[/v] if the DLL includes DcxDirectShow. [v]$false[/v] otherwise.',
            '__isid' => true,
        ),
        'IsUsingGDI' => array(
            '__desc' => 'Returns [v]$true[/v] or [v]$false[/v] depending on whether the GDI+ library is being used or not.',
            '__isid' => true,
        ),
		
		'PickIcon' => array(
			'__desc' => "Displays an icon select dialog. [o]XP+[/o]",
			'__cmd' => '[INDEX] [FILENAME]',
			'__eg' => '2 $mircdir $+ mirc.exe',
			'__isid' => true,
			'__params' => array(
					'INDEX' => "Default index to select.",
					'FILENAME' => 'File to display icons for.',
				),
			'__return' => "[v]D_OK SELECTED_INDEX SELECTED_FILENAME[/v] if user clicked OK.<br />
[v]D_ERROR DEFAULT_INDEX DEFAULT_FILENAME[/v] if user cancelled.",
			'__notes' => array(
				'[p]DEFAULT_INDEX[/p] is the pre-selected icon index and [p]SELECTED_INDEX[/p] is the index selected by the user.',
				'[p]FILENAME[/p] and [p]SELECTED_FILENAME[/p] may be different as the user is able to browse.',
			),
		),
		
		'MsgBox' => array(
			'__desc' => "Displays a messagebox which could be used to retrieve feedback from the user.",
			'__cmd' => '[STYLES] $chr(9) [TITLE] $chr(9) [MESSAGE]',
			'__eg' => 'yesnocancel warning defbutton3 $chr(9) Title Of The MessageBox $chr(9) Hello there',
            '__isid' => true,
			'__params' => array(
				'STYLES' => array(
					'__desc' => "Various options for the messagebox.",
					'__values' => array(
						'ok' => "Displays a single OK button. This is the default.",
						'okcancel' => "Displays two buttons, OK and CANCEL.",
						'retrycancel' => 'Displays two buttons, RETRY and CANCEL.',
						'yesno' => 'Displays two buttons, YES and NO.',
						'yesnocancel' => 'Displays three buttons, YES, NO and CANCEL.',
						'exclamation' => 'Shows an exclamation icon.',
						'warning' => 'Shows a warning icon.',
						'information' => 'Shows an information icon.',
						'asterisk' => 'Shows an asterisk icon.',
						'question' => 'Shows a question icon.',
						'stop' => 'Shows a stop icon.',
						'error' => 'Shows an error icon.',
						'hand' => 'Shows a hand icon.',
						'defbutton2' => 'Makes the second button default upon display.',
						'defbutton3' => 'Makes the third button default upon display.',
						'defbutton4' => 'Makes the fourth button default upon display.',
						'modal' => 'The user must respond to the message box before continuing work with the owner dialog/window. However, the user can move other dialogs/windows.',
						'sysmodal' => 'Same as [s]modal[/s], but will take focus.',
						'taskmodal' => 'Same as [s]modal[/s], but this will halt mIRC and prevent access to any windows until the messagebox is cleared.',
						'right' => 'The text is right aligned.',
						'rtl' => 'Displays message and caption text using right-to-left reading order on Hebrew and Arabic systems.',
						'foreground' => 'The message box becomes the foreground window.',
						'topmost' => 'The message box is on top.',
                                                'owner' => "Allows you to specific the parent dialog/window. (Must also specify [v]DNAME[/v] or [v]HWND[/v])",
					),
				),
				'TITLE' => "The title of the messagebox.",
				'MESSAGE' => 'The message displayed in the messagebox.',
			),
			'__return' => 'The return value can be one of the following: [v]abort[/v] [v]cancel[/v] [v]continue[/v] [v]ignore[/v] [v]no[/v] [v]ok[/v] [v]retry[/v] [v]tryagain[/v] [v]yes[/v]',
			'__notes' => array(
				'While it is possible to use this command during events, avoid using MsgBox in events as it will block some scripts from executing.',
				'MsgBox can be used as an identifier or as a direct call. (eg. as $dcx(MsgBox) or /dcx MsgBox)',
                                'By default, the main mIRC window is the parent. Dialog name OR a hWnd MUST follow after the [s]owner[/s] style.<br />
To get hWnd values, use $window().hwnd, $dialog().hwnd, $chan().hwnd, $chat().hwnd, $get().hwnd, $query().hwnd, $send().hwnd, and $timer().hwnd',
			),
		),
		"xSignal" => array(
			'__desc' => 'This command enables or disables signals from DCX whenever docked windows/dialogs/mIRC windows are resized, or when XStatusBar/XTray icons are clicked.',
			'__cmd' => '[BOOL] (+FLAGS)',
			'__eg' => array(
                                '1',
                                '0 +st',
                        ),
			'__params' => array(
				'BOOL' => 'Either [v]0[/v] or [v]1[/v], stating on or off respectively.',
				'+FLAGS' => array(
					'__desc' => 'Flags to finegrain the xSignal settings.',
					'__values' => array(
						'd' => 'Enable or disable XDock signals.',
						's' => 'Enable or disable XStatusBar signals.',
						't' => 'Enable or disable XTray signals.',
					),
				),
                                
			),
			'__notes' => array(
                                'If [p]+FLAGS[/p] is not specified, the setting specified in [p]BOOL[/p] is set on all signal types.',
				'This must be enabled in order to receive sizing events upon non-DCX windows resizing (used with [f]/xdock[/f], where you cannot [f]/dcx Mark[/f] @windows or #channels).',
				"Regular [f]/dcx Mark[/f]'d dialogs will still receive events in the callback aliases.",
				'A signal will only be sent for a window being resized if it is the main mIRC window, docked, or contains a docked window.',
				'Refer to the section [f]XDock/XStatusBar/XTray Signals[/f] documentation for more information on feature specific messages.',
			),
		),
		'WindowProps' => array(
			'__desc' => 'This command allows you to set the window title text, icon, and remove themes.',
			'__cmd' => '[HWND] [+FLAGS] [ARGS]',
			'__eg' => array(
				'$dialog(dcx).hwnd +t Hello',
				'$dialog(dcx).hwnd +i 3 mirc.exe',
				'$dialog(dcx).hwnd +T',
			),
			'__params' => array(
			    'HWND' => 'The HWND of the window.',
				'+FLAGS' => array(
					'__desc' => 'Options to apply to the window.',
					'__values' => array(
						't' => 'Set the title text.',
						'i' => 'Set the window icon.',
						'g' => 'Set the window icon in grayscale.',
                                                'r' => 'Simulates a right click on the specified window',
						'T' => 'Remove any themes on the specified window.',
					),
				),
				'__args' => array(
                                        'r' => array(
						'__cmd' => '[X] [Y]',
					),
					't' => array(
						'__cmd' => '(TEXT)',
						'__params' => array(
							'TEXT' => 'The window title text',
						),
					),
					'i' => array(
						'__cmd' => '[INDEX] [FILENAME]',
						'__params' => array(
				            'INDEX' => 'Icon index in icon archive',
							'FILENAME' => 'Icon archive filename',
						),
					),
				),
			),
			'__notes' => "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
		),
	);
}
?>
