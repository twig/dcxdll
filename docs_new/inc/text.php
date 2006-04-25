<?php
function get_intro_text() {
	echo "Regular text control. Used for labels or captions.";
}

function get_styles_text(&$STYLES) {
	$STYLES = array(
		"nowrap" => "Single line of text (default is multiline) (not compatible with [s]center[/s] or [s]right[/s]).",
		"right" => "Check control box is at the right of text.",
		"center" => "Control text is centered.",
		"noprefix" => "Prevents interpretation of any ampersand (&amp;) characters in the control's text as accelerator prefix characters. These are displayed with the ampersand removed and the next character in the string underlined.",
		"endellipsis" => "If the end of a string does not fit in the rectangle, it is truncated and ellipses (...) are added.",
		"pathellipsis" => "Replaces characters in the middle of the string with ellipses so that the result fits in the specified rectangle. If the string contains backslash (\) characters, pathellipsis preserves as much as possible of the text after the last backslash.",
	);
}


function get_xdid_text(&$XDID) {
	$XDID = array(
	    't' => array(
	        '__desc' => 'This command lets you set the text.',
	        '__cmd' => '(Text)',
	        '__eg' => 'Text Label'
		),
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
