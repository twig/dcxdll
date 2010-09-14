<?php
function get_intro_updown() {
	echo "Updown control (without an edit control buddy.)";
}


function get_styles_updown(&$STYLES) {
	$STYLES = array(
	    "left" => "Positions the up-down control next to the left edge of the buddy window. The buddy window is moved to the right, and its width is decreased to accommodate the width of the up-down control. (default is right)",
	    "arrowkeys" => "Causes the up-down control to increment and decrement the position when the UP ARROW and DOWN ARROW keys are pressed.",
	    "horz" => "Causes the up-down control's arrows to point left and right instead of up and down.",
	    'hottrack' => 'Causes the control to exhibit "hot tracking" behavior. That is, it highlights the UP ARROW and DOWN ARROW on the control as the pointer passes over them.',
	    'nothousands' => 'Does not insert a thousands separator between every three decimal digits.',
 		'buddyint' => 'Causes the up-down control to set the text of the buddy window when the position changes. The text consists of the position formatted as a decimal or hexadecimal string.',
 		'wrap' => 'Causes the position to "wrap" if it is incremented or decremented beyond the ending or beginning of the range.',
 		'tooltips' => 'The up-down control will have a tooltip.',
	);
}


function get_xdid_updown(&$XDID) {
	$XDID = array(
	    'c' => array(
	        '__desc' => 'This command lets you set the updown buddy control.',
	        '__cmd' => '[BUDDY ID]',
	        '__eg' => '7',
	        '__notes' => array(
	            "Setting a buddy control is optional, and limited to <a>edit</a> and <a>text</a> type controls only."
			),
		),
		'r' => array(
	        '__desc' => 'This command lets you set the updown range.',
	        '__cmd' => '[MIN] [MAX]',
	        '__eg' => '0 100',
		),
		't' => array(
	        '__desc' => 'This command lets you set the updown display numerical base.',
	        '__cmd' => '[BASE]',
	        '__eg' => "16",
            '__params' => array(
                'BASE' => array(
                    '__desc' => "Numerical base for the value.",
                    '__values' => array(
                        '10' => "Decimal",
                        '16' => "Hexadecimal",
					),
				),
			),
		),
		'v' => array(
	        '__desc' => 'This command lets you set the updown value.',
	        '__cmd' => '[VALUE]',
	        '__eg' => "25",
		),
	);
}

function get_xdidprops_updown(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"value" => array(
		    '__desc' => "This property lets you retreive updown value.",
		),
		"range" => array(
		    '__desc' => 'This property lets you retreive updown range.',
		),
	);
}

function get_events_updown(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When the text is clicked.",
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
