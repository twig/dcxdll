<?php
function get_intro_check() {
	echo "Checkbox control.";
}


function get_styles_check(&$STYLES) {
	$STYLES = array(
	    "right" => 'Check control box is at the right of text.',
	    'center' => 'Control text is centered.',
	    'rjustify' => 'Control text is right justified.',
	    'pushlike' => 'Makes the check act and look like a push button.',
	    '3state' => 'Creates a 3 state checkbox.',
	);
}


function get_xdid_check(&$XDID) {
	$XDID = array(
	    'c' => array(
	        '__desc' => 'This command lets you check the checkbox.',
	        '__notes' => array(
                "You can set the indeterminate state when using a [s]3state[/s] check by using the <a>-cu</a> switches.",
 			)
		),
		't' => array(
	        '__desc' => 'This command lets you set the check text.',
	        '__cmd' => '(Text)',
	        '__eg' => 'Check Text',
		),
		'u' => array(
	        '__desc' => 'This command lets you uncheck the check control.',
		),
	);
}


function get_xdidprops_check(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the check text.",
		),
		"state" => array(
		    '__desc' => "This property lets you retreive check selection state.",
		),
	);
}


function get_events_check(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When the text is clicked.",
	    "rclick" => "When you right-click on the text.",
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
