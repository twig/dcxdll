<?php
function get_intro_toolbar() {
	echo "This control enables you to create a toolbar.";
}


function get_styles_toolbar(&$STYLES) {
	$STYLES = array(
		"nodivider" => "The 2 pixel line divider is not included in the toolbar.",
		"flat" => "Toolbar style is flat with buttons having a hot mode (hover). This style also includes the [s]transparent[/s] style.",
		"list" => "Toolbar item text is to the right of the icon.",
		'tooltips' => "Toolbar items support tooltips.",
		"wrap" => 'Toolbar can have multiple lines of buttons. Toolbar buttons "break" off to the next line when the toolbar becomes too narrow to include all buttons on the same line. The break will occur on the rightmost separator item or the rightmost button if there are no separators on the bar.',
		"arrows" => "Displays a dropdown arrow on the buttons having the [v]+v[/v] flag set.",
		"top" => "The toolbar is auto-positioned at top of dialog window.",
		'bottom' => "The toolbar is auto-positioned at bottom of dialog window.",
		'left' => "Toolbar is auto-positioned to the left side of the dialog (makes the toolbar vertical).",
		'right' => "Toolbar is auto-positioned to the right side of the dialog (makes the toolbar vertical).",
		'noauto' => "Prevents the toolbar from being auto-positioned and resized by the parent window. The user will be responsible for the positioning and resizing of the toolbar.",
		'alpha' => 'Control is alpha blended.',
	);
}


function get_xdid_toolbar(&$XDID) {
	$XDID = array(
	    'a' => array(
	        '__desc' => 'This command lets you add an item to the toolbar.',
	        '__cmd' => '[N] [+FLAGS] [WIDTH] [ICON] [COLOR] (Button Text) [TAB] (Tooltip Text)',
	        '__eg' => '1 +cv 100 1 $rgb(255,0,0) Button $chr(9) Tooltip',
	        '__params' => array(
	            'N' => 'Button index number.',
	            '+FLAGS' => array(
	                '__desc' => 'Item flags.',
	                '__values' => array(
		                'a' => 'The button is auto-sized to fit in the icon and text.',
						'b' => 'The button text is bold. (Only works with XP themes disabled)',
						'c' => 'The button text is colored, defined by [p]COLOR[/p]. (Only works with XP themes disabled)',
						'd' => 'The button is disabled.',
						'g' => 'The button is part of a checkgroup (radio checks) (used with [f]-k[/f] flag).',
						'h' => 'The button is hidden (but still part of the toolbar).',
						'i' => 'The button is in indeterminate state (greyed out).',
						'k' => 'The button acts like a checkbox.',
						'l' => 'Specifies that [p]WIDTH[/p] is the button width.',
						'p' => 'The button appears like pressed.',
						'u' => 'Item text is underlined. (Only works with XP themes disabled)',
						'v' => 'The button acts like a dropdown button. (If the [s]arrows[/s] style is specified, a dropdown arrow will be drawn beside the button)',
						'w' => 'The button after (and all other buttons following) a button with this style will be forced to the next line.',
						'x' => 'The button is checked.',
					),
				),
				'WIDTH' => 'Button width (Use 0 if not used).',
				'ICON' => 'Icon displayed when item is not selected.',
				'COLOR' => 'Toolbar button text color (Only works with XP themes disabled).',
			),
	        '__notes' => array(
				"[p]Button Text[/p] and [p]Tooltip Text[/p] parameters are optional.",
                "If [p]Button Text[/p] is '[v]-[/v]' the button will be a separator item.",
				"If the button is a separator item, the value used for [p]ICON[/p] will be the separator width in pixels.",
				"Icons must be added before it can be used in the toolbar. (see <a>/xdid -l</a>)",
				'Colors will only work if the computer has no visual styles applied (XP themes). To use this on machines with visual styles enabled, apply the [s]notheme[/s] style.',
			),
		),
		'c' => array(
	        '__desc' => "This command lets you change the Nth toolbar button (or tooltip) colors and text styles.",
	        '__cmd' => '[N] [+FLAGS] [COLOR]',
	        '__eg' => '3 +bc $rgb(0,0,255)',
	        '__params' => array(
	            'N' => 'The index of the button to modify.',
				'+FLAGS' => array(
	           		'__desc' => 'Properties to set to the button.',
	                '__values' => array(
		                'b' => 'The text is bold.',
						'c' => 'The text is in a color defined by [p]COLOR[/p].',
						'u' => 'The text is underlined.',
						'x' => 'The background color of the toolbar tooltip.',
						'z' => 'The text color of the toolbar tooltip.',
					),
				),
				'COLOR' => 'The color to assign the button (or tooltip)'
			),
	        '__notes' => 'When using flags +x and +z, you MUST use [p]N[/p] to be [v]0[/v].'
		),
		'd' => array(
	        '__desc' => 'This command lets you delete the Nth toolbar button.',
	        '__cmd' => '[N]',
	        '__eg' => '6',
		),
		'i' => array(
	        '__desc' => 'This command lets you change the Nth toolbar button icon.',
	        '__cmd' => '[N] [ICON]',
	        '__eg' => "7 2",
	        '__notes' => "Use 0 for no icon.",
		),
		'j' => array(
	        '__desc' => 'This command lets you set the minimum and maximum width of toolbar buttons.',
	        '__cmd' => '[MIN] [MAX]',
	        '__eg' => '50 100',
	        '__notes' => "Only applies on toolbars with the [s]list[/s] style set.",
		),
		'l' => array(
	        '__desc' => 'This command lets you set the icon size of the toolbar image lists.',
   	        '__cmd' => '[SIZE]',
	        '__eg' => '16',
	        '__params' => array(
	            'SIZE' => array(
	                '__desc' => 'Image list image size',
	                '__values' => array(
		                '16' => 'Icons are 16x16 pixels.',
						'24' => 'Icons are 24x24 pixels.',
						'32' => 'Icons are 32x32 pixels.',
					)
				)
			),
			'__notes' => array(
			    "This command can also be used to clear the images lists.",
				"This command must be issued before adding icons to the toolbar image lists."
			),
		),
		'm' => array(
			'__desc' => 'This command lets you toggle the toolbar autostretch feature. This feature resizes automatically the toolbar buttons to stretch the full width of the toolbar when the control width changes. Does not work on vertical toolbars.',
			'__cmd' => '[1|0]',
			'__eg' => '1',
		),
		'q' => array(
	        '__desc' => "This command lets you change the Nth button tooltip text.",
	        '__cmd' => '[N] (TEXT)',
	        '__eg' => '2 New TipText',
	        '__notes' => '[p]TEXT[/p] is optional.'
		),
		'r' => 'This command lets you clear all the toolbar items.',
		't' => array(
	        '__desc' => 'This command lets you change the state of a toolbar button.',
	        '__cmd' => '[N] [+FLAGS]',
	        '__eg' => array(
				'2 +d',
				'2-7 +d',
			),
	        '__params' => array(
	            'N' => 'Button index number',
				'+FLAGS' => array(
					'__desc' => 'Item flags.',
					'__values' => array(
					    'd' => 'The button is disabled.',
						'h' => 'The button is hidden (but still part of the toolbar).',
						'i' => 'The button is in indeterminate state (greyed out).',
						'p' => 'The button appears like pressed.',
						'w' => 'The button after (and all other buttons following) a button with this style will be forced to the next line.',
						'x' => 'The button is checked.',
					),
				),
			),
	        '__notes' => array(
				"A flags value of [v]+[/v] resets the button to it's normal state.",
				'[N] can be a single button or a range of buttons eg: 1,2,8-12',
			),
		),
		'u' => array(
	        '__desc' => 'This command lets change the toolbar default button size.',
	        '__cmd' => '[WIDTH] [HEIGHT]',
	        '__eg' => '20 15',
		),
		'v' => array(
	        '__desc' => 'This command lets you change the Nth button text (text is optional).',
	        '__cmd' => '[N] (TEXT)',
	        '__eg' => '2 New Text',
		),
		'w' => array(
	        '__desc' => 'This command lets you add an icon to the toolbar image lists.',
	        '__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
	        '__eg' => '+n 113 shell32.dll',
	        '__params' => array(
	            '+FLAGS' => array(
	                '__desc' => "Image list flags.",
	                '__values' => array(
	                	// +dcxLoadIcon() flags
						'd' => 'Disabled icon list.',
						'n' => 'Normal icon list.',
						'h' => 'Hottrack icon list.',
					),
				),
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
		),
	);
	
	writeDcxLoadIcon($XDID, 'w', '+FLAGS', 1);
}

function get_xdidprops_toolbar(&$XDIDPROPS) {
	$XDIDPROPS = array(
		'dropdownpoint' => array(
		    '__desc' => 'This property lets you retreive the coordinates of the Nth toolbar button. These coordinates can be used to position a dropdown menu without the use of the [e]dropdown[/e] event.',
		    '__cmd' => 'N',
		    '__eg' => '3',
		),
		"num" => "This property lets you retreive the total number of buttons on the toolbar.",
		"text" => array(
		    '__desc' => 'This property lets you retreive the Nth button text.',
		    '__cmd' => 'N',
		    '__eg' => '1',
		),
		"state" => array(
		    '__desc' => 'This property lets you retreive the state of the Nth toolbar button. The flags returned are in the same format specified in <a>/xdid -t</a>.',
		    '__cmd' => 'N',
		    '__eg' => '6',
		),
		"icon" => array(
		    '__desc' => "This property lets you retreive the Nth button icon.",
		    '__cmd' => 'N',
		    '__eg' => '1',
		),
		"tooltip" => array(
		    '__desc' => "This property lets you retreive the Nth toolbar button tooltip.",
		    '__cmd' => 'N',
		    '__eg' => '1',
		),
		"mouseitem" => array(
		    '__desc' => "Returns the item which the mouse is currently over.",
		    '__notes' => 'Returns [v]-1[/v] when mouse is not over an item, or is currently over a disabled item.',
		),
		"tooltipbgcolor" => "This property lets you retreive the tooltip background color.",
		"tooltiptextcolor" => "This property lets you retreive the tooltip text color.",
	);
}

function get_events_toolbar(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => array(
			'__desc' => "When a button is clicked.",
			'__cmd' => 'BTN_ID',
			'__eg' => '2',
		),
	    "rclick" => array(
			'__desc' => "When a the right mouse button is clicked over a button.",
			'__cmd' => 'BTN_ID Bottom_X Bottom_Y Top_X Top_Y',
			'__eg' => '3 165 73 165 27',
			'__params' => array(
				'BTN_ID' => 'Button that triggered the event.',
				'Bottom_X' => 'Coordinates of a possible popup menu aligned under the button where the rclick was fired.',
				'Bottom_Y' => 'Coordinates of a possible popup menu aligned under the button where the rclick was fired.',
				'Top_X' => 'Coordinates of a possible popup menu aligned over the button where the rclick was fired.',
				'Top_Y' => 'Coordinates of a possible popup menu aligned over the button where the rclick was fired.',
			),
		),
		"dropdown" => array(
			'__desc' => "When a dropdown button (or arrow if [s]arrow[/s] style is set) event is fired on a button.",
			'__cmd' => 'BTN_ID Bottom_X Bottom_Y Top_X Top_Y',
			'__eg' => '10 586 73 586 27',
			'__params' => array(
				'BTN_ID' => 'Button that triggered the event.',
				'Bottom_X' => 'Coordinates of a possible popup menu aligned under the button where the dropdown was fired.',
				'Bottom_Y' => 'Coordinates of a possible popup menu aligned under the button where the dropdown was fired.',
				'Top_X' => 'Coordinates of a possible popup menu aligned over the button where the dropdown was fired.',
				'Top_Y' => 'Coordinates of a possible popup menu aligned over the button where the dropdown was fired.',
			),
		),
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
