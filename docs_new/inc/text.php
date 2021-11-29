<?php
function get_intro_text() {
	echo "Regular text control. Used for labels or captions.";
}

function get_styles_text(&$STYLES) {
	$STYLES = array(
		"nowrap" => "Single line of text (default is multiline) (not compatible with [s]center[/s] or [s]right[/s]).",
		"right" => "Control text is right aligned.",
		"center" => "Control text is centered.",
		"noprefix" => "Prevents interpretation of any ampersand (&amp;) characters in the control's text as accelerator prefix characters. These are displayed with the ampersand removed and the next character in the string underlined.",
		"endellipsis" => "If the end of a string does not fit in the rectangle, it is truncated and ellipses (...) are added.",
		"pathellipsis" => "Replaces characters in the middle of the string with ellipses so that the result fits in the specified rectangle. If the string contains backslash (\) characters, pathellipsis preserves as much as possible of the text after the last backslash.",
		'tooltips' => 'The text control will have a tooltip.',
		'alpha' => 'Control is alpha blended.',
		'shadow' => 'Display text with a shadow.',
		'noformat' => 'Disables support for mIRC codes in control (faster).',
		'hgradient' => 'Draws a horizontal gradient with the colors specified.',
		'vgradient' => 'Draws a vertical gradient with the colors specified.',
		'doublebuffer' => 'Buffers the drawing. May help with flicker in some cases.',
		'__notes' => array(
			'[s]shadow[/s] will only work if the text color is not black or the same as the default text color on the current windows theme (ie. $rgb(0, 0, 0) or $dcx(GetSystemColor, COLOR_WINDOWTEXT) respectively). Change it by using [f]xdid -C[/f]',
			'mIRC control codes are only supported when transparent style is set.'
		),
	);
}


function get_xdid_text(&$XDID) {
	$XDID = array(
		'a' => array(
	        '__desc' => 'This command allows you to append text to the control (to bypass the mIRC character limitation).',
	        '__cmd' => '[SPACE] (Text)',
	        '__eg' => '1 Appended text',
	        '__params' => array(
			    'SPACE' => 'A space will automatically be appended between the previous text and the new appended text.',
		    ),
	        '__notes' => 'Valid values for [p]SPACE[/p] are [v]1[/v] or [v]0[/v].',
		),
	    't' => array(
	        '__desc' => 'This command lets you set the text.',
	        '__cmd' => '(Text)',
	        '__eg' => 'Text Label'
		),
	  	'r' => 'This command clears the text.',
	);
}

function get_xdidprops_text(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the text.",
		),
	);
}

function get_events_text(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When the text is clicked.",
	    "dclick" => "When the text is double-clicked.",
	    "rclick" => "When you right-click on the text.",
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
