<?php

function xdialog_load($page) {
	global $XDIALOG, $XDIALOGPROPS;
	
	loadSection($XDIALOG, "get_xdialog_$page");
	loadSection($XDIALOGPROPS, "get_xdialogprops_$page");
}

function xdialog_unload() {
	global $XDIALOG, $XDIALOGPROPS;
	
	$XDIALOG = array();
	$XDIALOGPROPS = array();
}

function xdialog_layout($page, $pagelabel) {
	global $SECTION, $XDIALOG, $XDIALOGPROPS;
	
	dcxdoc_print_intro($page);
	
	// /xdialog commands
	if ($XDIALOG) {
		$SECTION = SECTION_XDIALOG;
		$count = 1;

		dcxdoc_print_description("/xdialog flags", "The /xdialog command is used to modify a DCX marked dialog.");

		foreach ($XDIALOG as $flag => $data) {
       		dcxdoc_format_xdialog($flag, $data, $count);
      		$count++;
		}
	}

	// $xdialog properties
	if ($XDIALOGPROPS) {
		$SECTION = SECTION_XDIALOGPROPS;
		$count = 1;

		dcxdoc_print_description('$xdialog() Properties', 'The $xdialog identifier is a given mIRC alias that communicates with the DCX DLL to extract information in DCX dialogs.');

		foreach ($XDIALOGPROPS as $prop => $data) {
	        dcxdoc_format_xdialogprop($prop, $data, $count);
	        $count++;
  		}
	}
	
	// this page still has generic /xdid, $xdid, and events
	gen_xdid($page, $pagelabel);
	gen_xdidprops($page, $pagelabel);
	gen_events($page, $pagelabel);
}


function get_intro_xdialog() {
?>
Once marked, a regular mIRC dialog becomes a DCX Marked Dialog. This enables DCX to manipulate the dialog and also allows creation of DCX controls on the dialog.<br />
<?php
}

function get_xdialog_xdialog(&$XDIALOG) {
	global $CLA;

	$XDIALOG = array(
		"a" => array(
			'__desc' => "This command lets you animate an opening/closing dialog.",
			'__cmd' => "[+FLAGS] [DELAY]",
			'__eg' => array(
                                '+ab 1000',
                                '+hnu 300',
                        ),
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
			    'The default effect is a roll animation.',
                            'Animation on region clipped dialogs will fail on operating systems prior to XP.',
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
						'm' => "Maximize button. (Requires [v]+y[/v] and can't be used with [v]+m[/v] or [v]+n[/v])",
						'n' => "Minimize button. (Requires [v]+y[/v] and can't be used with [v]+m[/v] or [v]+n[/v])",
						'o' => "Tool window. (Can't be used with [v]+h[/v], [v]+m[/v] or [v]n[/v])",
						's' => "Staticedge border.",
						't' => "Has a titlebar.",
						'v' => "Vista Aero styled dialog. [o]2k+[/o]",
						'w' => "Windowedge border.",
						'x' => 'Enables composite window rendering. [o]XP+[/o]',
						'y' => "Sysmenu (the little X button)",
						'z' => "Resizeable border.",
					),
					'DELAY' => "Delay in <b>millseconds</b> of the animation. (typical system value is 200)",
				),
			),
            '__notes' => 'Using [f]+x[/f] flag can have adverse effects on some controls, most notably the listview.',
		),
		"c" => array(
			'__desc' => "This command lets you create a DCX control on your dialog.",
			'__cmd' => "[ID] [TYPE] [X] [Y] [W] [H] (OPTIONS)",
			'__eg' => "4 pbar 140 30 100 20 smooth notheme",
			'__params' => array(
			    'ID' => "Dialog control ID that will serve as a host for the DCX Control.",
				'TYPE' => array(
					'__desc' => "The type of DCX Control to be created.",
					'__values' => array(
                                                'button' => "Creates a Button control.",
                                                'calendar' => 'Creates a Calendar control.',
                                                'colorcombo' => "Creates a ColorCombo control.",
                                                'comboex' => "Creates a ComboEx control.",
                                                'datetime' => 'Creates a DateTime Picker control.',
                                                'directshow' => "Creates a Directshow control.",
                                                'ipaddress' => "Creates a IpAddress control.",
                                                'listview' => "Creates a Listview control.",
                                                'pbar' => "Creates a Progressbar control.",
                                                'richedit' => "Creates a RichEdit control.",
                                                'statusbar' => "Creates a Statusbar control.",
                                                'toolbar' => "Creates a Toolbar control.",
                                                'trackbar' => "Creates a Trackbar control.",
                                                'treeview' => "Creates a TreeView control.",
                                                'updown' => "Creates an UpDown control.",
                                                'webctrl' => "Creates a Web control.",
						'&nbsp;' => '&nbsp;',
                                                
                                                'box' => "Creates a Box control.",
                                                'check' => "Creates a Check control.",
                                                'edit' => "Creates a Edit control.",
                                                'image' => "Creates a Image control.",
                                                'line' => "Creates a Line control.",
                                                'link' => "Creates a Link control.",
                                                'list' => "Creates a List control.",
                                                'radio' => "Creates a Radio control.",
                                                'scroll' => "Creates a Scroll control.",
                                                'text' => "Creates a Text control.",
						'&nbsp;&nbsp;' => '&nbsp;',
                                                
                                                'divider' => "Creates a Divider control.",
                                                'pager' => "Creates a Pager control.",
                                                'panel' => "Creates a Panel control.",
                                                'rebar' => "Creates a Rebar control.",
                                                'stacker' => 'Creates a Stacker control.',
                                                'tab' => "Creates a Tab control.",
						'&nbsp;&nbsp;&nbsp;' => '&nbsp;',
                                                
                                                'dialog' => "Docks a mIRC dialog. The first parameter in the [p](OPTIONS)[/p] must be the dialog name.",
						'window' => "Docks a mIRC @window. The first parameter in the [p](OPTIONS)[/p] must be the @window name.",
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
			'__eg' => "4",
		),
		"E" => array(
			'__desc' => "This command lets you enable/disable callback events.",
			'__cmd' => "[+FLAGS] [-FLAGS]",
			'__eg' => "+c -d",
			'__params' => array(
				'FLAGS' => array(
					'__desc' => "Flags for events.",
					'__values' => array(
						'c' => "Click events (eg. sclick, dclick, rclick, ...).",
						'd' => "Drag events (eg. dragbegin, ect..).",
						'e' => "Edit events (eg. edit, keydown, keyup, ...).",
						'f' => "Focus events (eg. focus and focusout).",
						'h' => "Help event.",
						'm' => "Mouse events (eg. mouseleave, mouseenter, mouse, ...).",
						's' => "Sizing events (eg. sizing, min, max, ...).",
						't' => "Theme event (eg. themechanged).",
						'C' => "Close event.",
						'M' => "Move events (eg. beginmove, endmove, ...).",
					),
				),
			),
			'__notes' => array(
				"The first parameter [p]+FLAGS[/p] allows the you to specify which events should be enabled. The second flag parameter specifies which events to disable.",
				"If you use the same event flag to [v]+[/v] and [v]-[/v] flags then the events is disabled.",
			),
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
			'__cmd' => "[+FLAGS] [ARGS]",
			'__eg' => array(
				'+b $rgb(0,0,255)',
				'+is dialog_bg.bmp',
			),
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Background flags.",
					'__values' => array(
						'b' => "Specifies background color.",
						'i' => 'Specifies background image.',
						'n' => 'Reset the background image position to top left. (Used with [v]+i[/v] flag)',
						't' => 'Tile the background image. (Used with [v]+i[/v] flag)',
						's' => 'Stretch the background image. (Used with [v]+i[/v] flag)',
						'c' => 'Center the background image horizontally. (Used with [v]+i[/v] flag)',
						'v' => 'Center the background image vertically. (Used with [v]+i[/v] flag)',
						'r' => 'Right-align the background image. (Used with [v]+i[/v] flag)',
						'o' => 'Bottom-align the background image. (Used with [v]+i[/v] flag)',
					),
				),
				'__args' => array(
					'b' => array(
						'__cmd' => '[COLOR]',
						'__params' => array(
							'COLOR' => 'Specifies the background color in a $rgb() format.',
						),
					),
					'i' => array(
						'__cmd' => '[IMAGE]',
						'__params' => array(
							'IMAGE' => 'Specfies the image filename.',
						),
					),
				),
			),
			'__notes' => 'Only formats supported by GDI+ can be loaded as a background image.'
		),
		"h" => array(
			'__desc' => "This command lets you hide the dialog.",
		),
		"j" => array(
			'__desc' => "This command forces a redraw of the control specified.",
			'__cmd' => "(ID)",
			'__eg' => "4",
			'__notes' => 'If no control ID is specified, then all controls will be updated.',
		),
		"l" => $CLA,
		"m" => array(
			'__desc' => "This command lets you maximize the dialog.",
		),
		"n" => array(
			'__desc' => "This command lets you minimize the dialog.",
		),
                "P" => array(
                        '__desc' => 'This command lets you use XPopup commands on dialog menus.',
			'__cmd' => '[-XPOPUP_FLAG] [ARGS]',
			'__eg' => '-t officexp',
			'__params' => array(
				'XPOPUP_FLAG' => 'Valid flags are listed on the [f]XPopup[/f] page.',
				'ARGS' => 'Parameters for this command will depend on the [p]XPOPUP_FLAG[/p] specified.',
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
						'a' => "Cursor is limited to [v]Caption[/v] Area",
						'b' => "Cursor is limited to [v]Bottom[/v] Area",
						'B' => "Cursor is limited to [v]Bottom Right[/v] Area",
						'c' => "Cursor is limited to [v]Client[/v] Area",
						'C' => "Cursor is limited to [v]Close Button[/v] Area",
						'h' => "Cursor is limited to [v]Horizontal Scroll[/v] Area",
						'H' => "Cursor is limited to [v]Help Button[/v] Area",
						'l' => "Cursor is limited to [v]Left[/v] Area",
						'L' => "Cursor is limited to [v]Top Left[/v] Area",
						'm' => "Cursor is limited to [v]Menu[/v] Area",
						'M' => "Cursor is limited to [v]Min Button[/v] Area",
						'n' => "Cursor is limited to [v]Nowhere[/v] Area",
						'Q' => "Cursor is limited to [v]Top Right[/v] Area",
						'R' => "Cursor is limited to [v]Right[/v] Area",
						's' => "Cursor is limited to [v]Sysmenu[/v] Area",
						'S' => "Cursor is limited to [v]Size Button[/v] Area",
						't' => "Cursor is limited to [v]Top[/v] Area",
						'T' => "Cursor is limited to [v]Bottom Left[/v] Area",
						'v' => "Cursor is limited to [v]Vertical Scroll[/v] Area",
						'x' => "Cursor is limited to [v]Max Button[/v] Button Area",
						'X' => "Cursor is limited to [v]Border[/v] Button Area",
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
		"R" => array(
			'__desc' => "This command lets you clip the dialog region.",
			'__cmd' => "[+FLAGS] (ARGS)",
			'__eg' => array(
				'+f $rgb(255,0,255) dialog_shape.bmp',
				'+p 0,0 $+($dialog(dcxtest).w,$chr(44),0) $+($calc($dialog(dcxtest).w /2),$chr(44),$dialog(dcxtest).h)',
			),
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Region flags.",
					'__values' => array(
                        'd' => 'Allows dragging of the window by clicking anywhere on it.',						
                        'f' => "Region is defined in bitmap file.",
                        'g' => 'Allows the dialog to be ghosted while dragging.',
                        's' => 'Adds a shadow effect to the dialog.',
                        
                        'o' => 'OR new region with existing one. (Can be used with [f]c[/f], [f]p[/f] or [f]r[/f])',
                        'a' => 'AND new region with existing one. (Can be used with [f]c[/f], [f]p[/f] or [f]r[/f])',
                        'i' => 'DIFF new region with existing one. (Can be used with [f]c[/f], [f]p[/f] or [f]r[/f])',
                        'x' => 'XOR new region with existing one. (Can be used with [f]c[/f], [f]p[/f] or [f]r[/f])',
                        
                        'c' => "Sets an ellipse region.",
                        'p' => 'Sets a polygon region.',
						'r' => "Sets a rounded rectangle region.",
						'n' => 'Resets the region.',
					),
				),
				'__args' => array(
					'd' => "[1|0]",
					'f' => "[TRANSPARENT_COLOR] [FILENAME]",
                    'g' => array(
                        '__cmd' => '[VALUE]',
                        '__params' => array(
                            'VALUE' => 'The opacity of the dragged window. This can be a value between [v]0[/v]-[v]255[/v].',
                        ),
                    ),
                    's' => array(
                        '__cmd' => '[COLOUR] [SHARPNESS] [DARKNESS] [SIZE] [XOFFSET] [YOFFSET]',
                        '__params' => array(
                            'COLOR' => 'The color of the shadow in $rgb() format.',
                            'SHARPNESS' => 'The fuzziness of the shadow. Can range from [v]0[/v]-[v]20[/v].',
                            'DARKNESS' => 'The shadow intensity. Can range from [v]0[/v]-[v]255[/v].',
                            'SIZE' => 'The size of the shadow. Can range from [v]-20[/v]-[v]20[/v].',
                            'XOFFSET' => 'The horizontal distance the shadow will be placed away from the dialog. Can range from [v]-20[/v]-[v]20[/v].',
                            'YOFFSET' => 'The vertical distance the shadow will be placed away from the dialog. Can range from [v]-20[/v]-[v]20[/v].',
                        ),
                    ),
					'p' => array(
						'__cmd' => 'x1,y1 x2,y2 x3,y3 ... x12,y12',
						'__params' => array(
							'COORDINATES' => 'A list of at least 3 coordinates, each coordinate in the format of [v]X,Y[/v].',
						),
					),
                    'r' => array(
                        '__cmd' => "(ROUNDNESS)",
                        '__params' => array(
                            'ROUNDNESS' => 'An integer defining how round the rectangle should be.',
                        ),
                    ),
				),
			),
            '__notes' => array(
                'If [f]+s[/f] is used without parameters, it will remove the shadow.',
                'The flag [f]+s[/f] does not rely on Windows XP themes at all, so it can be used on older operating systems.'
            ),
		),
		"s" => array(
			'__desc' => "This command lets you show the dialog.",
		),
		"S" => array(
			'__desc' => "This command allows you to set the size and position of the window. This differs from mIRC's resizing functionality as the height and width does not include the window border.",
			'__cmd' => '[X Y] [W H]',
			'__eg' => array(
				'300 100 -1 -1',
				'-1 75 50 100',
                                '100 200 300 -1',
				'15 30 45 60',
			),
			'__params' => array(
				'X Y' => 'The X and Y coordinates of the window.',
                                'W H' => 'The width and height of the window.',
			),
			'__notes' => 'You can specify [v]-1[/v] for any value that you do not wish to modify.',
		),
		"t" => array(
			'__desc' => "This command allows you to set the transparency color for the background image.",
			'__cmd' => '[SWITCH] [VALUE]',
			'__eg' => array(
				'transparentcolor $rgb(255,0,255)',
				'alpha 70',
			),
			'__params' => array(
				'SWITCH' => array(
					'__desc' => "The type of transparency to be applied.",
					'__values' => array(
						'alpha' => "Sets the transparency level.",
						'clickthrough' => 'User is able to click through the dialog as if it were not there.',
						'transparentcolor' => "This will cause the color [p]VALUE[/p] on the dialog to be hollowed out.",
						'bgcolor' => "Sets the transparent background color.",
					),
				),
				'VALUE' => "The transparent color or alpha value (values range from [v]0[/v] to [v]255[/v] if [v]alpha[/v] is used). [v]none[/v] can be used to remove the effects.",
			),
			// TODO: update this when its implemented.
			'__notes' => '[v]clickthrough[/v] is still experimental. Give us feedback on any problems you encounter with this feature.',
		),
		"T" => array(
			'__desc' => "This command lets you create a tooltip.",
			'__cmd' => "[+FLAGS]",
			'__eg' => "+p",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Tooltip flags.",
					'__values' => array(
						'a' => "Tooltip will always be displayed, even if mIRC is not the active application.",
						'b' => 'Tooltip uses the balloon shape.',
//						'c' => 'UNKNOWN FLAG',
						'f' => 'No fade-in effect.',
						'p' => 'Allows the ampersand character (&amp;) to be displayed',
						's' => 'No slide-in effect.',
					),
				),
			),
			'__notes' => 'You must call this function if you wish to use tooltips on any control with this dialog. (Except controls which naturally have their own tooltips, such as treeview, listview, toolbar, etc)'
		),
		"V" => array(
			'__desc' => "This command allows you to set the Vista Aero glass area.",
			'__cmd' => "[LEFT] [RIGHT] [TOP] [BOTTOM]",
			'__eg' => "10 10 20 10",
		),
		"w" => array(
			'__desc' => "This command lets you change the dialog icon.",
			'__cmd' => "[+FLAGS] [INDEX] [FILENAME]",
			'__eg' => "+ 2 shell32.dll",
			'__params' => array(
				// +FLAGS
				'+FLAGS' => array(
					'__values' => array(
						'b' => 'Uses the big icon.',
						's' => 'Uses the small icon.',
					),
				),
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => 'Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.',
		),
		"x" => array(
			'__desc' => "This command lets you kill (not trigger a close like /dialog -x) the dialog.",
			'__notes' => "You will have to use this command to close a dialog if you return [r]noclose[/r] in the [e]close[/e] event. mIRC's /dialog -x will be ineffective to close the dialog.",
		),
        "z" => array(
            '__desc' => "This command lets you manage controls on the dialog in a tab-like manner. The internal list works in a very similar fashion to icon lists in controls such as the toolbar.",
            '__cmd' => "[+FLAG] [N]",
            '__eg' => "+a 2",
            '__params' => array(
                '+FLAG' => array(
                    '__desc' => "Command flags.",
                    '__values' => array(
                        'a' => "Adds a control to the internal list. This will also auto-hide the control which was added.",
                        'p' => "Positions all controls in the list to the same position as the specified control ID.",
                        's' => "Shows the control found on the specified internal list index. This will also auto-hide the previous control displayed.",
                    ),
                ),
            ),
            '__notes' => array(
                "When used with [f]+a[/f], [p]N[/p] is the control ID to add.",
                "When used with [f]+p[/f], all controls in the internal list will be moved to where control ID [p]N[/p] is positioned.",
                "When used with [f]+s[/f], [p]N[/p] is the <strong>internal list index</strong> to show, NOT the control ID!",
            ),
        ),
	);
	
	writeDcxLoadIcon($XDIALOG, 'w', '+FLAGS', 1);
}


function get_xdialogprops_xdialog(&$XDIALOGPROPS) {
	$XDIALOGPROPS = array(
	    "isid" => array(
			'__desc' => "This property retrieves if the given ID exists in the dialog.",
			'__cmd' => "ID",
			'__eg' => "4",
		),
		"ismarked" => 'This property returns $true if the dialog has been marked by DCX, and $false if not marked.',
		"nextid" => "This property retrieves the next free ID in the dialog.",
		"id" => array(
			'__desc' => "This property retrieves the Nth used ID in the dialog, or the total number of used IDs if N is 0.",
			'__cmd' => "N",
			'__eg' => "5",
		),
		"ismenu" => "This property retrieves if the dialog displays a menu or not.",
		"mouseid" => "This property retrieves which control the mouse currently hovers.",
		"focusid" => "This property retrieves which control currently has the focus.",
		"mouse" => "This property retrieves the mouse position from the dialog viewpoint.",
		'alias' => 'This property returns the callback alias name supplied when the dialog was marked.',
		'visible' => 'This property returns $true if the dialog is visible, and $false if it is not visible.',
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
						'512' => "Left Arrow",
						'1024' => "Left Arrow",
						'2048' => "Right Arrow",
						'4096' => "Down Arrow",
						'8192' => "Caps Lock",
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
		"parent" => "This property retrieves the name of the parent dialog (only if this dialog is docked using [f]/xdialog -c[/f] or [f]/xdid -c[/f] and if it has been marked).",
        "zlayer" => array(
            '__desc' => "This property retrieves the control ID found at the Nth internal list index.",
            '__cmd' => "N",
            '__eg' => "5",
            '__notes' => 'When [p]N[/p] is [v]0[/v], this will return the number of controls found in the internal list.',
        ),
        'zlayercurrent' => 'This property retrieves the currently selected internal list index.',
	);
}

function get_events_xdialog(&$EVENTS) {
	$EVENTS = array(
                "close" => array(
                        '__desc' => 'When the dialog is closed (including by pressing escape).',
                        '__notes' => array(
                                "You will have to use [f]/xdialog -x[/f] to close the dialog instead of mIRC's /dialog -x if you return [r]noclose[/r] on this event.",
                                'Should only be used with non-modal dialogs (modal dialogs are created using $dialog). If you are using a modal dialog, refer to [e]scclose[/e]. Returning [r]noclose[/r] here will cause the modal dialog to be unclosable.',
                                'If you wish to close docked dialogs within this event, you must use a timer with /xdialog -x to close the docked dialog.',
			),
                        '__isdialog' => true,
			'__return' => '[r]noclose[/r] Return this value to block the window closing.',
		),
		"beginsize" => array(
			'__desc' => "When the dialog is about to be resized.",
			'__return' => "[r]nosize[/r] Return this value to block the window resizing.",
            '__isdialog' => true,
		),
		"sizing" => array(
			'__desc' => 'When the dialog is being resized where $3 is the new width and $4 is the new height.',
			'__cmd' => 'W H',
			'__eg' => '90 60',
			'__isdialog' => true,
		),
		"changing" => array(
			'__desc' => 'When the dialog is about to be resized or moved. The parameters passed are the dimensions which the dialog is about to become.',
			'__cmd' => 'TYPE X Y W H',
			'__eg' => 'sizing 50 30 300 150',
			'__params' => array(
				'TYPE' => array(
					'__desc' => "Type of changing event.",
					'__values' => array(
						'moving' => 'Position is changing. [p]W[/p] and [p]H[/p] can be ignored as they are not changing.',
						'sizing' => 'Size is changing. [p]X[/p] and [p]Y[/p] can be ignored as they are not changing.',
                                                'both' => 'Both position and size is changing.',
					),
				),
			),
			'__return' => array(
				'nosize' => 'Return this value to block the window from resizing.',
				'nomove' => 'Return this value to block the window from moving.',
				'nochange' => 'Return this value to block the window from moving and resizing.',
			),
			'__isdialog' => true,
		),
		"endsize" => array(
            '__desc' => "When the dialog has finished being resized.",
            '__isdialog' => true,
        ),
		"beginmove" => array(
			'__desc' => "When the dialog is about to be moved.",
			'__return' => "[r]nomove[/r] Return this value to block the window moving.",
            '__isdialog' => true,
		),
		"moving" => array(
            '__desc' => "When the dialog is being moved.",
            '__isdialog' => true,
        ),
		"endmove" => array(
            '__desc' => "When the dialog has finished being moved.",
            '__isdialog' => true,
        ),
                'isverbose' => array(
                        '__desc' => 'Sent when the dialog is marked. This allows the user to enable or disable verbose error messages within their script. By default, the return value is [v]$true[/v].',
                        '__isdialog' => true,
                        '__return' => '[r]$false[/r] to disable error messages in the status window.',
                ),
		"mouse" => array(
            '__desc' => 'When the mouse hovers over the dialog.',
            '__isdialog' => true,
        ),
		"mouseenter" => array(
            '__desc' => 'When the mouse enters the dialog.',
            '__isdialog' => true,
        ),
		"mouseleave" => array(
            '__desc' => 'When the mouse leaves the dialog.',
            '__isdialog' => true,
        ),
                'scclose' => array(
                        '__desc' => 'Only triggered when the dialog is closed using the X button or the system menu.',
                        '__notes' => array(
                                'This is provided as a way to stop the user from closing a modal dialog. Pressing escape will still close the dialog.',
                                'The normal mIRC command /dialog -x will still work.',
			),
                        '__isdialog' => true,
			'__return' => '[r]noclose[/r] Return this value to block the window closing.',
		),
		"sclick" => array(
            '__desc' => "When the left mouse button is clicked.",
            '__isdialog' => true,
        ),
		"rclick" => array(
            '__desc' => "When the right mouse button is clicked.",
            '__isdialog' => true,
        ),
		"dclick" => array(
            '__desc' => "When the left mouse button is double-clicked.",
            '__isdialog' => true,
        ),
		"denter" => array(
            '__desc' => "Event triggered when the mouse enters the dialog client area.",
            '__isdialog' => true,
        ),
		"dleave" => array(
            '__desc' => "Event triggered when the mouse leaves the dialog client area.",
            '__isdialog' => true,
        ),
		"lbdown" => array(
            '__desc' => "When the left mouse button is pressed.",
            '__isdialog' => true,
        ),
		"lbup" => array(
            '__desc' => "When the left mouse button is released.",
            '__isdialog' => true,
        ),
		"lbdblclk" => array(
            '__desc' => "When the left mouse button is double-clicked.",
            '__isdialog' => true,
        ),
		"rbdown" => array(
            '__desc' => "When the right mouse button is pressed.",
            '__isdialog' => true,
        ),
		"rbup" => array(
            '__desc' => "When the right mouse button is released.",
            '__isdialog' => true,
        ),
		"rbdblclk" => array(
            '__desc' => "When the right mouse button is double-clicked.",
            '__isdialog' => true,
        ),
		"mbdown" => array(
            '__desc' => "When the middle mouse button is pressed.",
            '__isdialog' => true,
        ),
		"mbup" => array(
            '__desc' => "When the middle mouse button is released.",
            '__isdialog' => true,
        ),
		"mbdblclk" => array(
            '__desc' => "When the middle mouse button is double-clicked.",
            '__isdialog' => true,
        ),
		"focus" => array(
            '__desc' => 'Triggered when the dialog gains the focus.',
            '__isdialog' => true,
        ),
		"focusout" => array(
            '__desc' => 'Triggered when the dialog loses the focus.',
            '__isdialog' => true,
        ),
		"min" => array(
			'__desc' => "When the dialog is about to be minimized.",
			"__return" => "[r]stop[/r] Return this value to block the window minimizing.",
            '__isdialog' => true,
		),
		"max" => array(
			'__desc' => "When the dialog is about to be maximized.",
			"__return" => "[r]stop[/r] Return this value to block the window maximizing.",
            '__isdialog' => true,
		),
		"restore" => array(
            '__desc' => "When the dialog is about to be restored.",
            '__isdialog' => true,
        ),
		'dragbegin' => array(
		    '__desc' => "Triggers when one or more files are dropped onto the control.",
		    '__cmd' => 'COUNT',
		    '__eg' => '3',
            '__params' => array(
            	'COUNT' => "Total number of files dragged into the control.",
			),
			'__return' => '[r]cancel[/r] to stop the drag drop events from occuring.',
            '__isdialog' => true,
		),
		'dragfile' => array(
			'__desc' => "The filename of the file that has",
			'__cmd' => 'FILENAME',
			'__eg' => 'C:/mIRC/blah.txt',
            '__isdialog' => true,
		),
		'dragfinish' => array(
            '__desc' => "Event triggered when processing of drag drop is complete.",
            '__isdialog' => true,
        ),
		'themechanged' => array(
            '__desc' => 'Event triggered when aspects of the windows theme is changed.',
            '__isdialog' => true,
        ),
		'activate' => array(
            '__desc' => "Event triggered when dialog is activated.",
            '__isdialog' => true,
        ),
		'deactivate' => array(
            '__desc' => 'Event triggered when dialog is deactivated.',
            '__isdialog' => true,
        ),
		'mwheel' => array(
            '__desc' => 'Event triggered when the mouse wheel is scrolled.',
            '__cmd' => '+FLAGS DIRECTION',
			'__eg' => '+cs up',
            '__params' => array(
            	'+FLAGS' => array(
			        '__desc' => "Keystate flags during scroll.",
                    '__values' => array(
                        'c' => 'Control key.',
						'l' => 'Left mouse button.',
						'm' => 'Middle mouse button.',
						'r' => 'Right mouse button.',
						's' => 'Shift key.',
					),
				),
            	'DIRECTION' => array(
            		'__desc' => 'The direction which the wheel is scrolling.',
                    '__values' => array(
                        'up' => 'Upwards.',
						'down' => 'Downwards.',
					),
				),
			),
                        '__isdialog' => true,
		),
                'zlayershow' => array(
                        '__desc' => 'Event triggered when a control is about to be displayed by calling [f]/xdialog -z[/f] with the [f]+s[/f] flag.',
                        '__cmd' => 'ZINDEX CID',
                        '__eg' => '1 4',
                        '__params' => array(
                                'ZINDEX' => 'The internal control index being shown.',
                                'CID' => 'The ID of the control being shown.',
                        ),
                        '__isdialog' => true,
                ),
	);
}
?>