<?php
function get_intro_edit() {
	echo "Normal edit control.<br/>Root control: <a href='https://learn.microsoft.com/en-gb/windows/win32/controls/edit-controls'>Edit</a>";
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
		'zoomable' => 'Control is zoomable. [o]win10+[/o]',
		'eollf' => 'Allow line feeds to end lines. [o]win10+[/o]',
		'eolcr' => 'Allow carriage returns to end lines. [o]win10+[/o]',
		'__notes' => array(
			'Even with [s]number[/s] style, it is still possible to paste non-digits into the edit control.',
			'[s]password[/s] does not work with [s]multi[/s] style.',
			'By default the edit control defines the end of line as crlf',
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
		        '__notes' => "The text is inserted at the [v]Nth[/v] character no matter what mode the control is in. If the control has less that N characters the text is added to the end."
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
			'__eg' => array(
				'5 New multi line Text',
				'0 New single line Text',
			),
			'__notes' => array(
				"If the [s]multi[/s] style is not used, will overwrite the whole text.",
				"[N] must be supplied even if the control is single line",
				"For a multi line control [N] must be > zero",
			),
		),
		'P' => array(
			'__desc' => 'This command lets you paste the contents of the clipboard into the edit control.',
		    '__notes' => 'The text will be pasted directly into the current caret position. See [cmd]/xdid -S[/cmd].',
		),
		'q' => array(
		    '__desc' => 'This command lets you set a char length limit on the edit.',
		    '__cmd' => '[LIMIT]',
		    '__eg' => '8',
		),
		'r' => "This command lets you clear the contents of the edit.",
		'V' => "This command lets you scroll the contents to make the caret visible.",
		't' => array(
		    '__desc' => 'This command lets you load the contents of a file directly in the edit.',
		    '__cmd' => '[FILENAME]',
		    '__eg' => 'C:/mIRC/blah.txt',
		),
		'u' => array(
		    '__desc' => 'This command lets you save the contents of the edit directly to a file.',
		    '__cmd' => '[FILENAME]',
		    '__eg' => 'C:/mIRC/blah.txt',
		),
		'E' => array(
		    '__desc' => 'This command lets you set the cue text. It behaves like a ghostly value when the edit control is empty.',
		    '__cmd' => '[TEXT] or [+FLAGS] [TEXT]',
		    '__eg' => array(
				'Username',
				'+f Username',
			),
			'__params' => array(
                '+FLAGS' => array(
                    '__desc' => 'Cue flags.',
                    '__values' => array(
                        'f' => 'Cue text stays visible when control has focus.',
                    ),
                ),
				'TEXT' => 'Cue text to use.',
			),
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
            '__params' => array(
				'N' => 'The line index to get.',
            ),
		    '__notes' => 'If the [s]multi[/s] style is not used, you can ignore [p]N[/p] when retrieving the edit text.'
		),
		"num" => 'This property lets you retreive the number of lines in the edit.',
		"isreadonly" => array(
			'__desc' => 'Is the edit control using the [s]readonly[/s] style?',
			'__return' => array(
				'0' => '- Control is NOT readonly.',
				'1' => '- Control IS readonly.',
			),
		),
		"ispass" => array(
			'__desc' => 'Is the edit control using the [s]password[/s] style?',
			'__return' => array(
				'0' => '- Password mode disabled.',
				'1' => '- Password mode enabled.',
			),
		),
		"ismodified" => array(
			'__desc' => 'Have the contents have been modified?',
			'__return' => array(
				'0' => '- Contents have not been modified.',
				'1' => '- Contents have been modified.',
			),
		),
		"sel" => 'This returns the index of the first and last selected character position in the control.',
		"selstart" => 'This returns the index of the first selected character position in the control.',
		"selend" => 'This returns the index of the last selected character position in the control.',
		"seltext" => 'This returns the selected text in the control.',
		"cue" => 'This returns the cue banner text in the control.',
		"caretpos" => array(
		    '__desc' => "This property lets you retreive the position of the cursor.",
			'__return' => '[r]LINE[/r] [r]COLUMN[/r] [r]CHAR[/r]',
		),
		"linenumbers" => array(
			'__desc' => 'This returns if line numbers are enabled.',
			'__return' => array(
				'0' => '- disabled.',
				'1' => '- enabled.',
			),
		),
		"guttercolours" => array(
		    '__desc' => 'This returns the colours used to draw line numbers.',
			'__return' => '[[r]Selected line Background Colour[/r]] [[r]Background Colour[/r]] [[r]Selected Line Text Colour[/r]] [[r]Text Colour[/r]] [[r]Border Colour[/r]]',
		),
		"find" => array(
            '__desc' => "This property lets you retreive the character offset of the Nth found match to the input search patern.",
            '__cmd' => '[TAB]Matchtext[TAB], T, N, LINE, SUBCHAR',
            '__eg' => '$chr(9) *Item* $chr(9), W, 3, 1, 30',
            '__params' => array(
                    'MATCHTEXT' => 'String composed of wildcards or regex paterns used for the search.',
                    'T' => array(
                            '__desc' => 'Value indicating if the search is using a regex patern, a wildcard string, or an exact match.',
                            '__values' => array(
                                    'W' => 'Matchtext is a wildcard string.',
                                    'R' => 'Matchtext is a regex patern.',
									'E' => 'Matchtext is an exact match',
                            ),
                    ),
                    'N' => 'Parameter indicating to return the Nth match.',
                    'LINE' => 'The line to start the search at.',
                    'SUBCHAR' => 'The character within the line to start the search at.',
            ),
			'__return' => array(
				'OFFSET' => '- The character offset within the text that matched the search.',
			),
			'__notes' => array(
				'If [p]N[/p] is [v]0[/v], returns the total number of matching items.',
				'The [TAB] characters surrounding the matchtext are mandatory or else it wont work.',
			),
		),
		"passchar" => array(
			'__desc' => 'This property returns the character used to cover passwords.',
		    '__notes' => 'If the [s]password[/s] style is not used then this will return zero.',
		),
		"len" => array(
		    '__desc' => 'This property returns the length of the controls contents or a specific line.',
		    '__cmd' => '(N)',
		    '__eg' => array(
				'',
				'2',
			),
            '__params' => array(
                    'N' => 'The line index to get the length of.',
            ),
		    '__notes' => 'If the [s]multi[/s] style is not used, then [p]N[/p] is ignored and the length of all the text is returned.',
		),
		"endofline" => array(
		    '__desc' => 'This property returns the character used to mark the end of lines. [o]win10+[/o]',
			'__return' => array(
				'0' => "- No end of line character",
				'1' => "- EC_ENDOFLINE_CRLF - end of line is a CRLF pair.",
				'2' => "- EC_ENDOFLINE_CR - end of line is a CR",
				'3' => "- EC_ENDOFLINE_LF - end of line is a LF",
		//	EC_ENDOFLINE_CRLF	one		The end - of - line character used for new linebreaks is carriage return followed by linefeed(CRLF).
		//	EC_ENDOFLINE_CR		two		The end - of - line character used for new linebreaks is carriage return (CR).
		//	EC_ENDOFLINE_LF		three	The end - of - line character used for new linebreaks is linefeed(LF).
			),
		),
		"zoom" => array(
		    '__desc' => 'This property returns the zoom level. [o]win10+[/o]',
			'__return' => array(
				"ZOOM" => "- Level of zoom",
			),
		    '__notes' => array(
				'Only works on controls that have the [s]zoomable[/s] style.',
				"The zoom ratio is always between 1/64 and 64 NOT inclusive, 1.0 = no zoom",
			),
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
			'__return' => '[r]nocopy[/r] - To block the copy operation.'
		),
		"cut" => array(
			'__desc' => "When the user cuts text from the edit control.",
			'__return' => '[r]nocut[/r] - To block the cut operation.'
		),
		"paste" => array(
			'__desc' => "When the user pastes text into the edit control.",
			'__return' => '[r]nopaste[/r] - To block the paste operation.'
		),
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
		'dragbegin' => array(
			'__desc' => "Triggers when one or more files are dropped onto the control.",
			'__cmd' => 'COUNT',
			'__eg' => '3',
			'__params' => array(
				'COUNT' => "Total number of files dragged into the control.",
			),
			'__return' => '[r]cancel[/r] - To stop the drag drop events from occuring.',
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
