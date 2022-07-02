<?php
function get_intro_edit() {
	echo "Normal edit control.";
}


function get_styles_edit(&$STYLES) {
	$STYLES = array(
		"multi" => "Enables a multiline edit.",
		"center" => "Text is centered.",
		"right" => "Text is right justified.",
		'autohs' => "Text scrolling is automatically scrolled horizontally without need of a horizontal scrollbar.",
		'autovs' => "Text scrolling is automatically scrolled vertically without need of a vertical scrollbar.",
		'hsbar' => "Displays a horizontal scrollbar.",
		'vsbar' => "Displays a vertical scrollbar.",
		'lowercase' => "Converts all characters to lowercase as they are typed into the edit control.",
		'uppercase' => "Converts all characters to uppercase as they are typed into the edit control.",
		'number' => 'Allows only digits to be entered into the edit control.',
		'password' => "Displays an asterisk (*) for each character typed into the edit control.",
		'return' => "Specifies that a carriage return be inserted when the user presses the ENTER key while entering text into a multiline edit control in a dialog box.",
		'readonly' => "Prevents the user from typing or editing text in the edit control.",
		'tooltips' => 'The edit will have a tooltip.',
		'showsel' => 'When the control loses focus, the selection is still displayed.',
		'alpha' => 'Control is alpha blended.',
		'showlinenumbers' => 'Show line numbers in a multi line control',
		'unlockgutter' => 'Allow resizing the gutter area.',
		'__notes' => array(
			'Even with [s]number[/s] style, it is still possible to paste non-digits into the edit control.',
			'[s]password[/s] does not work with [s]multi[/s] style.',
		),
	);
}


function get_xdid_edit(&$XDID) {
	$XDID = array(
		'a' => array(
		        '__desc' => 'This command lets you add text to the end of the edit.',
		        '__cmd' => '[TEXT]',
		        '__eg' => 'New Text',
		),
		'c' => "This command will copy the text in the edit to the clipboard.",
		//'C' => array(
		//	'__desc' => 'This command sets the caret position to the Nth character.',
		//	'__cmd' => '[N]',
		//	'__eg' => '2',
		//	'__notes' => "If [s]N[/s] is a negative, the position is moved to current position + [s]N[/s].",
		//),
		'd' => array(
			'__desc' => 'This command lets you delete the Nth line of text in the edit.',
			'__cmd' => '[N,N2,N3-N4]',
			'__eg' => '2,40-55',
			'__notes' => "Only works with [s]multi[/s].",
		),
		'l' => array(
			'__desc' => 'This command lets you change the [s]readonly[/s] style.',
			'__cmd' => '[1|0]',
			'__eg' => '1',
		),
		'i' => array(
			'__desc' => 'This command lets you insert a line at the Nth position in the edit.',
		        '__cmd' => '[N] (TEXT)',
		        '__eg' => "5 New Text",
		        '__notes' => "If the [s]multi[/s] style is not used, will overwrite the whole text."
		),
		'I' => array(
			'__desc' => 'This command lets you insert a line at the Nth position in the edit.',
		        '__cmd' => '[N] [TEXT]',
		        '__eg' => "5 Inserted Text",
		        '__notes' => "The text is inserted at the [s]Nth[/s] character no matter what mode the control is in. If the control has less that N characters the text is added to the end."
		),
		'j' => array(
		        '__desc' => 'This command lets you trigger the password style of the edit control on the fly.',
		        '__cmd' => '[1|0] (CHAR)',
		        '__eg' => "1 *",
		        '__notes' => 'Sets the password obscuring char to (CHAR) if supplied, or the default for your system.'
		),
		'o' => array(
			'__desc' => 'This command lets you overwrite the Nth line in the edit.',
			'__cmd' => '[N] [TEXT]',
			'__eg' => '5 New Text',
			'__notes' => "If the [s]multi[/s] style is not used, will overwrite the whole text."
		),
		'P' => array(
			'__desc' => 'This command lets you paste the contents of the clipboard into the edit control.',
		        '__notes' => 'The text will be pasted directly into the current caret position. See [f]/xdid -S[/f].',
		),
		'q' => array(
		        '__desc' => 'This command lets you set a char length limit on the edit.',
		        '__cmd' => '[LIMIT]',
		        '__eg' => '8',
		),
		'r' => "This command lets you clear the contents of the edit.",
		't' => array(
		        '__desc' => 'This command lets you load the contents of a file directly in the edit.',
		        '__cmd' => '[FILENAME]',
		        '__eg' => 'C:/mIRC/blah.txt',
		),
		'u' => array(
		        '__desc' => 'This command lets you save the contents of the edit directly in a file.',
		        '__cmd' => '[FILENAME]',
		        '__eg' => 'C:/mIRC/blah.txt',
		),
		'E' => array(
		        '__desc' => 'This command lets you set the cue text. It behaves like a ghostly value when the edit control is empty.',
		        '__cmd' => '[TEXT]',
		        '__eg' => 'Username',
		),
		'S' => array(
		        '__desc' => 'This command lets you set the selection range (and/or caret position) in the edit control.',
		        '__cmd' => '[START] (END)',
		        '__eg' => array(
			        '1 3',
				'5',
			),
		        '__notes' => 'When the [p]END[/p] parameter is not specified, then this command will set the caret position to [p]START[/p].',
		),
		'y' => array(
			'__desc' => 'This command lets you ignore callbacks for repeated keydown events when the user holds a button. This command also allows you to enable/disable line numbers',
			'__cmd' => '[1|0|-] (0|1)',
			'__eg' => '1',
			'__notes' => array(
				'Repeated keys are ignored by default.',
				'A value of [v]1[/v] will prevent callbacks from being generated for repeated keydown events.',
			),
		),
		'g' => array(
			'__desc' => 'This command lets you set the colours to use for line numbers',
			'__cmd' => '[Selected line Background Colour|-] (Background Colour|-) (Selected Line Text Colour|-) (Text Colour|-) (Border Colour|-) (Unlock Gutter 0|1|-) (Gutter Size|-) (Gutter Border Size|-)',
			'__eg' => '$rgb(255,0,0) $rgb(16,24,5) - $rgb(12,34,0)',
			'__notes' => array(
				'A value of [v]-[/v] will cause an option to be ignored.',
				'atm the Gutter Border Size option isnt implemented.',
			),
		),
	);
}

function get_xdidprops_edit(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the Nth line in the edit or the whole edit text.",
		    '__cmd' => '[N]',
		    '__eg' => '2',
		    '__notes' => 'If the [s]multi[/s] is not used, you can ignore [p]N[/p] when retrieving the edit text.'
		),
		"num" => 'This property lets you retreive the number of lines in the edit.',
		"isreadonly" => 'Is the edit control using the [s]readonly[/s] style?',
		"ispass" => 'This property lets you retreive if the password style is set on the edit control.',
		"sel" => 'This returns the index of the first and last selected character position in the control.',
		"selstart" => 'This returns the index of the first selected character position in the control.',
		"selend" => 'This returns the index of the last selected character position in the control.',
		"seltext" => 'This returns the selected text in the control.',
		"cue" => 'This returns the cue banner text in the control.',
		"caretpos" => array(
		    '__desc' => "This property lets you retreive the position of the cursor.",
		    '__notes' => 'The return value is [p]LINE[/p] [p]COLUMN[/p] [p]CHAR[/p]',
		),
		"linenumbers" => 'This returns if line numbers are enabled.',
		"guttercolours" => array(
		    '__desc' => 'This returns the colours used to draw line numbers.',
		    '__notes' => 'The return value is [p]Selected line Background Colour[/p] [p]Background Colour[/p] [p]Selected Line Text Colour[/p] [p]Text Colour[/p] [p]Border Colour[/p]',
		),
	);
}

function get_events_edit(&$EVENTS) {
	$EVENTS = array(
		"edit" => "When the text is edited in the edit control.",
		"return" => 'When the <b>enter</b> key is pressed in the edit control.',
		"keydown" => array(
			'__desc' => "When a key is pressed in the edit control.",
			'__cmd' => 'KEYCODE',
			'__eg' => '65',
		),
		"keyup" => array(
			'__desc' => "When a key is unpressed in the edit control.",
			'__cmd' => 'KEYCODE',
			'__eg' => '65',
		),
		"copy" => array(
			'__desc' => "When the user copies text from the edit control.",
			'__return' => '[r]nocopy[/r] this value to block the copy operation.'
		),
		"cut" => array(
			'__desc' => "When the user cuts text from the edit control.",
			'__return' => '[r]nocut[/r] Return this value to block the cut operation.'
		),
		"paste" => array(
			'__desc' => "When the user pastes text into the edit control.",
			'__return' => '[r]nopaste[/r] Return this value to block the paste operation.'
		),
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
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
			'__desc' => "The filename of the file which was dropped onto the edit control.",
			'__cmd' => 'FILENAME',
			'__eg' => 'C:/mIRC/blah.txt',
		),
		'dragfinish' => "Event triggered when processing of drag drop is complete.",
	);
}
?>
