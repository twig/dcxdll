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
		'number' => 'Allows only digits to be entered into the edit control.<br /><a class="note">Note.</a> Even with [s]number[/s] set, it is still possible to paste non-digits into the edit control.',
 // TODO: arrays for styles
		//array(
//			'__desc' => "Allows only digits to be entered into the edit control.<br />[n]Even with [s]number[/s] set, it is still possible to paste non-digits into the edit control.[/n]",
//			'__notes' => "Even with [s]number[/s] set, it is still possible to paste non-digits into the edit control.",
//		),
		'password' => "Displays an asterisk (*) for each character typed into the edit control. Only works when [s]multi[/s] style is NOT set.",
		'return' => "Specifies that a carriage return be inserted when the user presses the ENTER key while entering text into a multiline edit control in a dialog box.",
		'readonly' => "Prevents the user from typing or editing text in the edit control.",
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
		'd' => array(
	        '__desc' => 'This command lets you delete the Nth line of text in the edit.',
	        '__cmd' => '[N]',
	        '__eg' => '2',
	        '__notes' => "Only works with [s]multi[/s]."
		),
		'i' => array(
	        '__desc' => 'This command lets you insert a line at the Nth position in the edit.',
	        '__cmd' => '[N] (TEXT)',
	        '__eg' => "5 New Text",
	        '__notes' => "If the [s]multi[/s] style is not used, will overwrite the whole text."
		),
		'j' => array(
	        '__desc' => 'This command lets you trigger the password style of the edit control on the fly.',
	        '__cmd' => '[ENABLED]',
	        '__eg' => "1",
	        '__notes' => array(
				"Use either [v]1[/v] or [v]0[/v] for [p]ENABLED[/p].",
			    'If anyone could find me the ascii value of the big DOT used for the default password character on XP, please leave a message.'
			),
		),
		'o' => array(
	        '__desc' => 'This command lets you overwrite the Nth line in the edit.',
	        '__cmd' => '[N] [TEXT]',
	        '__eg' => '5 New Text',
	        '__notes' => "If the [s]multi[/s] style is not used, will overwrite the whole text."
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
	        '__eg' => 'C:\mIRC\blah.txt',
		),
		'u' => array(
	        '__desc' => 'This command lets you save the contents of the edit directly in a file.',
	        '__cmd' => '[FILENAME]',
	        '__eg' => 'C:\mIRC\blah.txt',
		),
		'S' => array(
	        '__desc' => 'This command lets you set the selection range in the edit.',
	        '__cmd' => '[START] [END]',
	        '__eg' => '1 3',
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
		"num" => array(
		    '__desc' => 'This property lets you retreive the number of lines in the edit.',
		    '__notes' => 'Only works with the [s]multi[/s] style.'
		),
		"ispass" => 'This property lets you retreive if the password style is set on the edit control.',
		"sel" => 'This returns the index of the first and last selected character position in the control.',
		"selstart" => 'This returns the index of the first selected character position in the control.',
		"selend" => 'This returns the index of the last selected character position in the control.',
		"seltext" => 'This returns the selected text in the control.',
	);
}

function get_events_edit(&$EVENTS) {
	$EVENTS = array(
	    "edit" => "When the text is edited in the edit control.",
	    "return" => 'When the <b>enter</b> key is pressed in the edit control.',
		"keydown" => array(
			'__desc' => "When a key is pressed in the edit control.",
			'__cmd' => 'KEYCODE',
		),
		"keyup" => array(
			'__desc' => "When a key is unpressed in the edit control.",
			'__cmd' => 'KEYCODe',
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
			'__desc' => "The filename of the file that has",
			'__cmd' => 'FILENAME',
			'__eg' => 'C:\mIRC\blah.txt',
		),
		'dragfinish' => "Event triggered when processing of drag drop is complete.",
	);
}
?>
