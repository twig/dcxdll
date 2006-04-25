<?php
function get_intro_radio() {
	echo "Radio control.";
}


function get_styles_radio(&$STYLES) {
	$STYLES = array(
	    "right" => 'Check control box is at the right of text.',
	    'center' => 'Control text is centered.',
	    'rjustify' => 'Control text is right justified.',
	    'pushlike' => 'Makes the check act and look like a push button.',
	);
}


function get_xdid_radio(&$XDID) {
	$XDID = array(
	    'c' => array(
	        '__desc' => 'This command lets you check the radio control.',
		),
		't' => array(
	        '__desc' => 'This command lets you set the radio text.',
	        '__cmd' => '(Text)',
	        '__eg' => 'Radio Text',
		),
		'u' => array(
	        '__desc' => 'This command lets you uncheck the radio control.',
		),
	);
}


function get_xdidprops_radio(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the radio text.",
		),
		"state" => array(
		    '__desc' => "This property lets you retreive radio selection state.",
		),
	);
}


function get_events_radio(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When the text is clicked.",
	    "rclick" => "When you right-click on the text.",
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
