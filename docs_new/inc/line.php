<?php
function get_intro_line() {
	echo "The line control is a graphical line, much like the side of a box control.";
}

function get_styles_line(&$STYLES) {
	$STYLES = array(
		"vertical" => "The line is vertical.",
		"nowrap" => "Lengthy text will not be wrapped.",
		'center' => 'Text is center aligned',
		'noprefix' => 'The &amp; character is not parsed as an underline.',
		'endellipsis' => 'Appends ... if the text becomes too lengthy',
		'pathellipsis' => 'Replaces characters in the middle of the text with ellipses so that the result fits in the specified area. [os]NT+[/os]',
		'alpha' => 'Control is alpha blended.',
		'shadow' => 'Display text with a shadow.',
		'noformat' => 'Disables support for mIRC codes in control (faster).',
		'simple' => 'Display a simple line. (no text or shading)',
	);
}

function get_xdid_line(&$XDID) {
	$XDID = array(
		't' => array(
	        '__desc' => 'This command lets you set the text of a line.',
	        '__cmd' => '(Cell Text)',
	        '__eg' => 'Line text',
		),
	);
}

function get_xdidprops_line(&$XDIDPROPS) {
	$XDIDPROPS = array(
		'text' => 'This property lets you retreive the line text.',
	);
}

function get_events_line(&$EVENTS) {
	$EVENTS = array(
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
