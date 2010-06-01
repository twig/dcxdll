<?php
function get_intro_rebar() {
	echo "This control enables you to create a rebar control. This control acts as a container for other controls. The rebar control is consisted of <b>bands</b> that host an icon, text and a child control.";
}


function get_styles_rebar(&$STYLES) {
	$STYLES = array(
		'borders' => 'Borders appear between rebar bands.',
		'varheight' => 'The rebar control displays bands at the minimum required height, when possible. Without this style, the rebar control displays all bands at the same height, using the height of the tallest visible band to determine the height of other bands. Even if this style is applied, bands on the same row will display the height of the highest band on that row.',
		'dblclktoggle' => 'Double click on band text toggles min/max resizing of the band. (Single click is default)',
		'fixedorder' => 'Bands are always in the same order but can be dragged on different lins/columns.',
		'tooltips' => 'The rebar bands have tooltips.',
		'verticalgrip' => 'The size grip will be displayed vertically instead of horizontally in a vertical rebar control. (must be used with [s]vertical[/s])',
		'vertical' => 'The rebar is auto-positioned to the left side of the dialog (gives a vertical rebar).',
		'bottom' => 'The rebar is auto-positioned at bottom of dialog window.',
		'right' => 'The rebar is auto-positioned to the right side of the dialog (must be used with [s]vertical[/s] style).',
		'noresize' => 'The rebar isnt auto-resized when dialogs size changes.',
		'noparentalign' => 'The rebar isnt auto-positioned when dialogs size changes.',
	);
}


function get_xdid_rebar(&$XDID) {
	global $CLA;

	$XDID = array(
		"a" => array(
			'__desc' => "This command lets you add bands to the rebar. These bands can optionally host a supported child control.",
			'__cmd' => "[N] [+FLAGS] [CX] [CY] [WIDTH] [ICON] [COLOR] (TEXT) [TAB] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS) [TAB] (TOOLTIP)",
			'__eg' => '0 +f 0 25 20 2 ComboEx $chr(9) 12 comboex 10 10 400 25 dropedit $chr(9) Tooltip',
			'__params' => array(
			    'N' => 'Band position index number (Use [v]0[/v] to insert at the end).',
				'+FLAGS' => array(
					'__desc' => "Item flags.",
					'__values' => array(
						'c' => 'The button text is colored ([COLOR] is the text color). (Only works with XP themes disabled)',
						'e' => 'The band has an edge at the top and bottom of the child window.',
						'f' => 'The band cant be sized. With this style, the sizing grip is not displayed on the band.',
						'g' => 'The band will always have a sizing grip, even if it is the only band in the rebar.',
						'h' => 'The band will not be visible.',
						'n' => 'The band will never have a sizing grip, even if there is more than one band in the rebar.',
						'w' => 'The band is on a new line.',
					),
				),
				'CX' => 'Minimal width to which the rebar band child control can be resized.',
				'CY' => 'Minimal height to which the rebar band child control can be resized.',
				'WIDTH' => 'Width of the rebar band when created.',
			    'ICON' => 'Icon index to be displayed (Use [v]0[/v] for no icon).',
			    'COLOR' => 'Rebar band text color (Only works with XP themes disabled).',
			    'TEXT' => 'Band text.',
			    'ID' => "Unique control ID for the DCX Control. <b>Must be unique for all the controls of the dialog!</b>",
				'CONTROL' => array(
					'__desc' => "The type of DCX Control to be created.",
					'__values' => array(
						'pbar' => "Creates a Progressbar control.",
						'treeview' => "Creates a TreeView control.",
						'toolbar' => "Creates a Toolbar control.",
						'statusbar' => "Creates a Statusbar control.",
						'comboex' => "Creates a ComboEx control.",
						'listview' => "Creates a Listview control.",
						'trackbar' => "Creates a Trackbar control.",
						'colorcombo' => "Creates a ColorCombo control.",
						'button' => "Creates a Button control.",
						'richedit' => "Creates a RichEdit control.",
						'&nbsp;' => '&nbsp;',
						'divider' => "Creates a Divider control.",
						'panel' => "Creates a Panel control.",
						'tab' => "Creates a Tab control.",
					),
				),
				'X' => "X position of control.",
				'Y' => "Y position of control.",
				'W' => "Width of control.",
				'H' => "Height of control.",
				'OPTIONS' => "Optional styles and options available on each individual controls.",
				'TOOLTIP' => 'Band tooltip. Not functional yet.',
			),
			'__notes' => array(
				'The control is optional. You can use the tab control without child controls as a toolbar (like bar of buttons).',
				'If you are using the vertical style, [p]CX[/p], [p]CY[/p], and [p]WIDTH[/o] are the minimal height, minimal width, and height values.',
				'If the band is the last one created on a band row, the [p]WIDTH[/p] value is ignored and the band stretches to the end of the rebar control.',
				'The control part of the command is optional and can be empty if you do not wish to have a control in the band.',
			),
		),
		'd' => array(
	        '__desc' => "This command lets you delete the Nth rebar band.",
	        '__cmd' => '[N]',
	        '__eg' => '2',
		),
		'i' => array(
	        '__desc' => "This command lets you hide the Nth rebar band.",
	        '__cmd' => '[N]',
	        '__eg' => '2',
		),
		'j' => array(
	        '__desc' => "This command lets you show the Nth rebar band.",
	        '__cmd' => '[N]',
	        '__eg' => '2',
		),
		'k' => array(
	        '__desc' => "This command lets you change the icon on a rebar band.",
	        '__cmd' => '[N] [ICON]',
	        '__eg' => '2 0',
	        '__params' => array(
	            'N' => 'Band position index number.',
				'ICON' => 'Icon index number from the icon list ([v]0[/v] for no icon).',
			),
		),
		'l' => array(
	        '__desc' => "This command lets you lock the Nth rebar band or all the bands.",
	        '__cmd' => '[N|all]',
	        '__eg' => 'all',
		),
		'm' => array(
	        '__desc' => "This command lets you maximize the Nth rebar band.",
	        '__cmd' => '[N]',
	        '__eg' => '2',
		),
		'n' => array(
	        '__desc' => "This command lets you minimize the Nth rebar band.",
	        '__cmd' => '[N]',
	        '__eg' => '2',
		),
		't' => array(
	        '__desc' => "This command lets you change the text on a rebar band.",
	        '__cmd' => '[N] (Itemtext)',
	        '__eg' => '2 New Text',
		),
		'u' => array(
	        '__desc' => "This command lets you unlock the Nth rebar band or all the bands.",
	        '__cmd' => '[N|all]',
	        '__eg' => 'all',
		),
		'u' => array(
	        '__desc' => "This command lets you move the Nth rebar band to the Mth position.",
	        '__cmd' => '[N] [M]',
	        '__eg' => '3 1',
		),
		'w' => array(
	        '__desc' => 'This command lets you add an icon to the tab image list.',
	        '__cmd' => '[INDEX] [FILENAME]',
	        '__eg' => '113 C:/mIRC/shell.dll',
	        '__params' => array(
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => array(
	            "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
			),
		),
		'y' => 'This command lets you clear the rebar image list.',
	);
}


function get_xdidprops_rebar(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive a Nth rebar band text.",
	        '__cmd' => 'N',
	        '__eg' => '1',
		),
		'num' => 'This property lets you retreive the number of bands of a rebar.',
		"childid" => array(
		    '__desc' => 'This property lets you retreive Nth rebar band child control ID ($null if no child control on the band).',
	        '__cmd' => 'N',
	        '__eg' => '2',
		),
	);
}

function get_events_rebar(&$EVENTS) {
	$EVENTS = array(
	    "help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
		'sclick' => array(
		    '__desc' => 'When a sclick event is fired over a rebar band (excluding the area occupied by a child control).',
	        '__cmd' => 'BAND',
	        '__eg' => '2',
	        '__params' => array(
	            'BAND' => 'Rebar band item index number over which the event was triggered.',
			),
		),
		'rclick' => array(
		    '__desc' => 'When a rclick event is fired over a rebar band (excluding the area occupied by a child control).',
	        '__cmd' => 'BAND',
	        '__eg' => '2',
	        '__params' => array(
	            'BAND' => 'Rebar band item index number over which the event was triggered.',
			),
		),
		'change' => array(
		    '__desc' => 'When the rebar controls height changes because of band movement.',
	        '__cmd' => 'W H',
	        '__eg' => '2',
	        '__params' => array(
	            'W' => 'New rebar control width.',
	            'H' => 'New rebar control height.',
			),
		),
	);
}
?>
