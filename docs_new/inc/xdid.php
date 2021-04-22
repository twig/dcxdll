<?php

function get_intro_xdid() {
?>
The DCX supported controls are extended controls built directly on the DCX marked dialog. These are used to extend the capabilities of the existing controls in mIRC dialogs which are very limited in use for GUI design.<br />
<br />
DCX's /xdid commands now support multiple control IDs, similarly to mIRC's syntax: [v]/xdid [cmd] id-id [options][/v] and [v]/xdid [cmd] id,id-id [options][/v].<br />
<br/>
[n]Please note that some features of some DCX controls are only available to Windows XP+ machines. These commands have the symbol [o]OperatingSystem+[/o] beside their listing.[/n]<br />
<?php
}


function get_styles_xdid(&$STYLES) {
	$STYLES = array(
		"tabstop" => "Specifies a control that can receive the keyboard focus when the user presses the TAB key. Pressing the TAB key changes the keyboard focus to the next control with the [s]tabstop[/s] style.",
		"group" => "Specifies the first control of a group of controls. The group consists of this first control and all controls defined after it, up to the next control with the [s]group[/s] style. The first control in each group usually has the tabstop style so that the user can move from group to group. The user can subsequently change the keyboard focus from one control in the group to the next control in the group by using the direction keys.",
                'hidden' => 'The control is not visible.',
		"disabled" => "The control is created disabled.",
		"notheme" => "Removes XP Themes on controls.",
                "transparent" => "Does not draw the control background so the parent background will show through. (Applies to all controls but only some controls will actually process this style)",
	);
}

function get_xdid_xdid(&$XDID) {
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
			'__eg' => "+abs ansi 10 Tahoma",
			'__params' => array(
			    '+FLAGS' => array(
			        '__desc' => "Font style flags.",
                    '__values' => array(
                        'a' => "Font is anti-aliased.",
						'b' => "Font is bold.",
						'd' => "Default font.",
						'i' => "Font is italic.",
						's' => "Font is strikedout.",
						'u' => "Font is underlined.",
					),
				),
				'CHARSET' => "Can be any of the following: [v]ansi[/v], [v]baltic[/v], [v]chinesebig[/v], [v]default[/v], [v]easteurope[/v], [v]gb2312[/v], [v]greek[/v], [v]hangul[/v], [v]mac[/v], [v]oem[/v], [v]russian[/v], [v]shiftjis[/v], [v]symbol[/v], [v]turkish[/v], or [v]vietnamese[/v].",
				'SIZE' => "Font size in points.",
				'FONTNAME' => "Font name. Ex: Tahoma, Courier New, Arial, ...",
			),
			'__notes' => array(
				"Font names are limited to 31 chars (system definition).",
				'When using [v]+d[/v], [p]CHARSET[/p] [p]SIZE[/p] and [p]FONTNAME[/p] can be omitted.'
			),
		),
		"h" => array(
			'__desc' => "This command lets you hide the control.",
			'__cmd' => "(+FLAGS) (DELAY)",
			'__eg' => array(
                                '+c 1000',
                                '+ov 300',
                        ),
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Animation style flags.",
					'__values' => array(
						'b' => "Alpha-Blending fade effect.",
						'c' => "Makes the control appear to collapse inward.",
						'o' => "Animates the control from left to right. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v].",
						'n' => "Animates the control from right to left. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v].",
						's' => "Uses slide animation. It is ignored when used with [v]+c[/v].",
						'v' => "Animates the control from top to bottom. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v].",
						'u' => "Animates the control from bottom to top. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v].",
					),
				),
				'DELAY' => "Delay in <b>millseconds</b> of the animation. (typical system value is 200)",
			),
			'__notes' => array(
			    "The default effect is a roll animation.",
                            'Animation on region clipped controls will fail on operating systems prior to XP.',
			),
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
		"J" => array(
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
			'__cmd' => "(+FLAGS) (DELAY)",
			'__eg' => array(
                                '+c 1000',
                                '+nu 300'
                        ),
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Animation style flags.",
					'__values' => array(
						'b' => "Alpha-Blending fade effect.",
						'c' => "Makes the control appear to expand outward.",
						'o' => "Animates the control from left to right. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v].",
						'n' => "Animates the control from right to left. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v].",
						's' => "Uses slide animation. It is ignored when used with [v]+c[/v].",
						'v' => "Animates the control from top to bottom. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v].",
						'u' => "Animates the control from bottom to top. This flag can be used with roll or slide animation. It is ignored when used with [v]+c[/v].",
					),
				),
				'DELAY' => "Delay in <b>millseconds</b> of the animation. (typical system value is 200)",
			),
			'__notes' => array(
			    "The default effect is a roll animation.",
                            'Animation on region clipped controls will fail on operating systems prior to XP.',
			),
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
			'__eg' => '+bk $rgb(0,0,255)',
			'__params' => array(
				'+FLAGS' => array(
                                        '__desc' => "Color Flags.",
                                        '__values' => array(
                                                'b' => "Background color.<br />Affects: Edit, Radio, Check, Box, Scroll, Text, List, Panel, Image, Trackbar",
                                                'g' => 'Sets the start gradient color.',
                                                'G' => 'Sets the end gradient color.',
                                                'k' => "Text background Color.<br />Affects: Text, Edit, List",
                                                'r' => "Border color.<br />Affects: Box (only with [s]rounded[/s] style)",
                                                't' => "Text color.<br />Affects: Text, Edit, List, Box",
                                        ),
                                ),
                        ),
			'__notes' => 'When using [v]+b[/v] on the trackbar, call /xdid -U on the control to force a redraw. /xdialog -j will not work. This is an API bug.',
		),
		"F" => array(
			'__desc' => "This command lets you set the focus on the control.",
		),
		"M" => array(
			'__desc' => "This command lets you add additional information to a control that can be retrieved later.",
			'__cmd' => "[INFO]",
			'__eg' => "Mark info here",
		),
        "R" => array(
            '__desc' => "This command lets you clip the control region.",
            '__cmd' => "[+FLAGS] (ARGS)",
            '__eg' => array(
                '+f $rgb(255,0,255) control_shape.bmp',
                '+c',
            ),
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Region flags.",
                    '__values' => array(
                        'f' => "Region is defined in bitmap file. The bitmap is resized to match control size before the region is applied if [f]+R[/f] is supplied.",
                        'o' => 'OR new region with existing one. (Can be used with [f]c[/f], [f]p[/f] or [f]r[/f])',
                        'a' => 'AND new region with existing one. (Can be used with [f]c[/f], [f]p[/f] or [f]r[/f])',
                        'i' => 'DIFF new region with existing one. (Can be used with [f]c[/f], [f]p[/f] or [f]r[/f])',
                        'x' => 'XOR new region with existing one. (Can be used with [f]c[/f], [f]p[/f] or [f]r[/f])',
                        
                        'c' => "Sets an ellipse region.",
                        'p' => 'Sets a polygon region.',
                        'r' => "Sets a rounded rectangle region.",
                        'n' => 'Resets the region.',

			'b' => 'Sets the alpha blending state of the control.',
                    ),
                ),
                '__args' => array(
                    'b' => array(
                        '__cmd' => '[1|0] [VALUE]',
                        '__params' => array(
                            'VALUE' => 'The opacity of the control. This can be a value between [v]0[/v]-[v]255[/v].',
                        ),
                    ),
                    'f' => array(
                        '__cmd' => "[TRANSPARENT_COLOR] [FILENAME]",
                        '__params' => array(
                                'TRANSPARENT_COLOR' => 'The color in the image which is made transparent. Use [v]-1[/v] to have no transparent color.',
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
        ),
                "T" => array(
                        '__desc' => "This command lets you set the tooltip information for controls that support tooltips.",
                        '__cmd' => "[TEXT]",
                        '__eg' => "This is the tooltip text",
                        '__notes' => array(
                                'You must call [f]/xdialog -T[/f] to create the tooltip before you can use tooltips with any control. (Except controls which naturally have their own tooltips, such as treeview, listview, toolbar, etc)',
                                'The listview has its own [f]/xdid -T[/f] command.',
                        ),
                ),
		"U" => 'This command lets you remove focus from the control.',
		"Z" => array(
			'__desc' => "This command lets you scroll a control in the vertical direction where [p]POS[/p] is a percentage expressing the area to scroll (0 being top and 100 being bottom).",
			'__cmd' => "[POS]",
			'__eg' => "50",
			'__notes' => 'This behaviour only affects the listview control in [s]report[/s] mode. We are working on a fix for the other modes.',
		),
	);
}



function get_xdidprops_xdid(&$XDIDPROPS) {
        $XDIDPROPS = array(
                "alpha" => 'This property returns [v]$true[/v] if the control is using the [s]alpha[/s] style.',
                "enabled" => "This property retrieves if a control is enabled or disabled.",
		"visible" => "This property retrieves if a control is visible or hidden.",
		"pos" => "This property retrieves the position and size of a control in the format [X] [Y] [W] [H] relative to the parent (which can be the dialog).",
		"dpos" => "This property retrieves the position and size of a control in the format [X] [Y] [W] [H] relative to the dialog.",
		"mark" => "This property retrieves the additional info given to a control with /xdid -M.",
		"hwnd" => "This property retrieves the window handle (HWND) of a control.",
		"mouse" => "This property retrieves the mouse position from the control viewpoint.",
		"pid" => "This property retrieves the control ID of the parent of a control (0 if a dialog is the parent).",
		"type" => "This property retrieves the control type.",
		"font" => array(
			'__desc' => "This property returns the font used for writing on the control.",
			'__notes' => 'The return value is in the format [v]+flags[/v] [v]charset[/v] [v]size[/v] [v]fontname[/v], where [v]+flags[/v] is specfied in /xdid -f',
		),
		"tooltipbgcolor" => "This property retrieves the tooltip background color.",
		"tooltiptextcolor" => "This property retrieves the tooltip text color.",
                'textcolor' => 'This property retrieves the text color.',
                'textbgcolor' => 'This property retrieves the text bcakground color.',
                'bgcolor' => 'This property retrieves the background color.',
	);
}




function get_events_xdid(&$EVENTS) {
	$EVENTS = array(
		"sizing" => array(
			'__desc' => 'When the control is resized, where $3 is the new width and $4 is the new height.',
			'__cmd' => 'W H',
		    '__eg' => '90 60',
		),
		"mouse" => 'When the mouse hovers a control where $3 is the ID of the control.',
		"mouseenter" => 'When the mouse enters a control where $3 is the ID of the control.',
		"mouseleave" => 'When the mouse leaves a control where $3 is the ID of the control.',
		"focus" => 'Triggered when a control gains the focus where $3 is the control ID.',
		"focusout" => 'Triggered when a control loses the focus where $3 is the control ID.',
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
		),
	);
}
?>
