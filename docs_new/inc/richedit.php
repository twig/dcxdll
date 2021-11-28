<?php
function get_intro_richedit() {
	echo "This control enables you to create a richedit. The richedit interprets the mIRC control codes to display the colors and bold/underline effects. The richedit control loads it's internal <a>Color Palette</a> as the actual mIRC colors as default behavior. You can modify the colors using the /xdid commands.<br />
<br />
The <a>Color Palette</a> is the richedit's internal color list of 16 values used to process mIRC color codes and convert them to disaplyed colors.";
}


function get_styles_richedit(&$STYLES) {
	$STYLES = array(
//        'alpha' => 'Control is alpha blended.',
		"multi" => "Enables a multiline richedit.",
		"center" => "Text is centered.",
		"readonly" => "Text cannot be edited manually.",
		'right' => 'Text is right justified.',
		'autohs' => 'Text scrolling is automatically scrolled horizontally without need of a horizontal scrollbar.',
		'autovs' => 'Text scrolling is automatically scrolled vertically without need of a vertical scrollbar.',
		'hsbar' => 'Displays a horizontal scrollbar.',
		'vsbar' => 'Displays a vertical scrollbar.',
		'tooltips' => 'The richedit will have a tooltip.',
		'showlinenumbers' => 'Show line numbers in a multi line control',
	);
}


function get_xdid_richedit(&$XDID) {
	$XDID = array(
	    'a' => array(
	        '__desc' => 'This command lets you add text to the end of the richedit.',
	        '__cmd' => '[TEXT]',
	        '__eg' => 'New Text',
		),
		'c' => array(
	        '__desc' => 'This command will copy the text in the edit to the clipboard.',
		),
		//'C' => array(
		//	'__desc' => 'This command sets the caret position to the Nth character.',
		//	'__cmd' => '[N]',
		//	'__eg' => '2',
		//	'__notes' => "If [s]N[/s] is a negative, the position is moved to current position + [s]N[/s].",
		//),
		'd' => array(
			'__desc' => 'This command lets you delete the Nth line of text in the richedit.',
			'__cmd' => '[N,N2,N3-N4]',
			'__eg' => '2,40-55',
			'__notes' => "Only works with [s]multi[/s].",
		),
		'i' => array(
	        '__desc' => 'This command lets you insert a line at the Nth position in the richedit.',
	        '__cmd' => '[N] [TEXT]',
	        '__eg' => "2 New Text",
	        '__notes' => array(
	            "If the [s]multi[/s] style is not used, overwrites all text."
			),
		),
		'I' => array(
			'__desc' => 'This command lets you insert a line at the Nth position in the richedit. (cmd is uppercase i)',
		        '__cmd' => '[N] [TEXT]',
		        '__eg' => "5 Inserted Text",
			    '__notes' => array(
					"The text is inserted at the [s]Nth[/s] character no matter what mode the control is in.",
					" If the control has less that N characters the text is added to the end."
				),
		),
		'k' => array(
	        '__desc' => 'This command lets you change the richedit background color.',
	        '__cmd' => '[RGB]',
	        '__eg' => '$rgb(255,0,0)',
	        '__notes' => array(
	            "Use -1 to revert to default.",
			),
		),
		'l' => array(
	        '__desc' => 'This command lets you change a color in the richedit palette. (cmd is lowercase L)',
	        '__cmd' => '[N] [RGB]',
	        '__eg' => '5 $rgb(255,0,0)',
	        '__params' => array(
	            "N" => "An index between 1-16",
			),
		),
		'm' => array(
	        '__desc' => 'This command lets you load the mIRC color palette into the richedit.',
		),
		'n' => array(
			'__desc' => 'This command lets you toggle the automatic hyperlinking of urls to enable the [e]link[/e] event.',
			'__cmd' => '[1|0]',
			'__eg' => '1',
		),
		'o' => array(
	        '__desc' => 'This command lets you overwrite the Nth line in the richedit.',
	        '__cmd' => '[N] [TEXT]',
	        '__eg' => '5 New Text',
	        '__notes' => array(
	            "If the [s]multi[/s] style is not used, overwrites all text."
			),
		),
		'P' => array(
		  '__desc' => 'This command lets you paste the contents of the clipboard into the richedit control.',
		  '__notes' => array(
			'Formatting in the text is kept when using this command.',
			'The text will be pasted directly into the current caret position. See [f]/xdid -S[/f].',
			),
		),
		'q' => array(
	        '__desc' => 'This command lets you load a custom palette into the richedit control.',
	        '__cmd' => '[COLOR1] [COLOR2] ... [COLOR16]',
	        '__eg' => '2 $rgb(255,0,0)',
	        '__notes' => array(
	            "The number of colors is limited to 16, everything past the 16th color is ignored.",
	            "You can give the command a number of colors inferior to 16, it will then only add thoses colors in order to the palette starting at 1.",
			),
		),
		'r' => array(
	        '__desc' => 'This command lets you clear the contents of the richedit.',
		),
		't' => array(
			'__desc' => 'This command lets you load the contents of a file directly in the richedit.',
			'__cmd' => '[FILENAME]',
			'__eg' => 'C:/mIRC/blah.txt',
		),
		'u' => array(
			'__desc' => 'This command lets you save the contents of the richedit directly in a file.',
			'__cmd' => '[FILENAME]',
			'__eg' => 'C:/mIRC/blah.txt',
		),
        'S' => array(
            '__desc' => 'This command lets you set the selection range (and/or caret position) in the richedit control.',
            '__cmd' => '[START] (END)',
            '__eg' => array(
                '1 3',
                '5',
            ),
            '__notes' => 'When the [p]END[/p] parameter is not specified, then this command will set the caret position to [p]START[/p].',
        ),
		'y' => array(
			'__desc' => 'This command lets you ignore callbacks for repeated keydown events when the user holds a button.',
			'__cmd' => '[1|0]',
			'__eg' => '1',
			'__notes' => array(
				'Repeated keys are ignored by default.',
				'A value of [v]1[/v] will prevent callbacks from being generated for repeated keydown events.',
			),
		),
		'Z' => array(
			'__desc' => 'This command lets you zoom the text of the richedit.',
			'__cmd' => '[NUMERATOR] [DENOMINATOR]',
			'__eg' => '200 100',
			'__notes' => 'The percentage of zooming is calculated by dividing the numerator by denominator ([p]NUMERATOR[/p] / [p]DENOMINATOR[/p] * 100)',
		),
		'g' => array(
			'__desc' => 'This command lets you set the colours to use for line numbers',
			'__cmd' => '[Selected line Background Colour|-] (Background Colour|-) (Selected Line Text Colour|-) (Text Colour|-) (Border Colour|-)',
			'__eg' => '$rgb(255,0,0) $rgb(16,24,5) - $rgb(12,34,0)',
			'__notes' => array(
				'A value of [v]-[/v] will cause an option to be ignored.',
			),
		),
	);
}

function get_xdidprops_richedit(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the Nth line in the richedit or the whole richedit text.",
		    '__cmd' => 'N',
		    '__eg' => '1',
		    '__notes' => array(
		        'If the [s]multi[/s] style is not used, you can ignore the [p]N[/p] parameter to retrieve the richedit text.',
			),
		),
		"num" => 'This property lets you retreive the number of lines in the richedit.',
		"sel" => array(
		    '__desc' => 'This returns the index of the first and last selected character position in the control.',
		),
		"selstart" => array(
		    '__desc' => "This returns the index of the first selected character position in the control.",
		),
		"selend" => array(
		    '__desc' => 'This returns the index of the last selected character position in the control.',
		),
		"seltext" => array(
		    '__desc' => 'This returns the selected text in the control.',
		),
		"caretpos" => array(
		    '__desc' => "This property lets you retreive the position of the cursor.",
		    '__cmd' => 'LINE COLUMN',
		    '__eg' => '14 3',
		),
		"linenumbers" => 'This returns if line numbers are enabled.',
		"guttercolours" => 'This returns the colours used to draw line numbers.',
	);
}

function get_events_richedit(&$EVENTS) {
	$EVENTS = array(
	    "return" => "When the <b>Enter</b> key is pressed in the richedit control.",
	    "rclick" => "When the right mouse button is clicked on the richedit control.",
	    "keydown" => array(
	        '__desc' => "When a key is pressed in the richedit control.",
	        '__cmd' => 'KEYCODE',
	        '__eg' => '64',
		),
	    "keyup" => array(
	        '__desc' => "When a key is unpressed in the richedit control.",
	        '__cmd' => 'KEYCODE',
	        '__eg' => '64',
		),
		'link' => array(
		    '__desc' => "When a link is clicked.",
	        '__cmd' => 'EVENT LINK',
	        '__eg' => 'sclick http://dcx.scriptsdb.org',
	        '__params' => array(
            	'EVENT' => array(
					'__desc' => "Mouse event which triggered the callback",
					'__values' => array(
					    'sclick' => 'Single left click.',
					    'dclick' => 'Double left click.',
					    'rclick' => 'Single right click.',
					),
				),
				'LINK' => 'The URL which was clicked.',
			),
		),
		'selchange' => array(
			'__desc' => "When the selection range is changed.",
			'__cmd' => 'START END TEXT',
			'__eg' => '2 6 text has been sel',
			'__params' => array(
				'START' => 'The starting position of the selection range.',
				'END' => 'The ending position of the selection range.',
				'TEXT' => 'The text selected.',
			),
		),
		'sclick' => 'When the left mouse button is clicked.',
		'lbup' => 'When the left mouse button is released.',
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
