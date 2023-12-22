<?php
function get_intro_scroll() {
	echo "This control enables you to create a scroll bar.";
}


function get_styles_scroll(&$STYLES) {
	$STYLES = array(
        'alpha' => 'Control is alpha blended.',
		"vertical" => "Vertical scroll bar.",
	);
}


function get_xdid_scroll(&$XDID) {
	$XDID = array(
	    'l' => array(
	        '__desc' => 'This command lets you set the line step value.',
	        '__cmd' => '[VALUE]',
	        '__eg' => '5',
	        '__notes' => "See [e]Lineup[/e] and [e]Linedown[/e] events.",
		),
		'm' => array(
	        '__desc' => "This command lets you set the page step value.",
	        '__cmd' => '[VALUE]',
	        '__eg' => '10',
	        '__notes' => 'See [e]Pageup[/e] and [e]Pagedown[/e] events'
		),
		'r' => array(
	        '__desc' => 'This command lets you set the scrollbar range.',
	        '__cmd' => '[MIN] [MAX]',
	        '__eg' => '0 100',
		),
		'v' => array(
	        '__desc' => 'This command lets you set the tracker position.',
	        '__cmd' => '[POS]',
	        '__eg' => '50',
		),
	);
}

function get_xdidprops_scroll(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"value" => "This property lets you retreive the scroll bar position value.",
		"range" => 'This property lets you retreive the scroll bar range.',
		"line" => 'This property lets you retreive the scroll bar line step value.',
		'page' => 'This property lets you retreive the scroll bar page step value.'
	);
}

function get_events_scroll(&$EVENTS) {
	$EVENTS = array(
		"trackend" => array(
			'__desc' => "When the user releases the left mouse button after dragging the thumb.",
			'__cmd' => 'POS',
			'__eg' => '73',
			'__params' => array(
				'POS' => 'Scroll bar position.'
			),
		),
        "top" => array(
			'__desc' => "When the user presses the home key.",
			'__cmd' => 'POS',
            '__eg' => '0',
			'__params' => array(
				'POS' => 'Scroll bar position.'
			),
		),
        "bottom" => array(
			'__desc' => "When the user presses the end key.",
			'__cmd' => 'POS',
            '__eg' => '100',
			'__params' => array(
				'POS' => 'Scroll bar position.'
			),
		),
		"lineup" => array(
			'__desc' => "When the user increases the value by using the arrow keys.",
			'__cmd' => 'POS',
            '__eg' => '89',
			'__params' => array(
				'POS' => 'Scroll bar position.'
			),
		),
		"linedown" => array(
			'__desc' => "When the user decreases the value by using the arrow keys.",
			'__cmd' => 'POS',
			'__eg' => '71',
			'__params' => array(
				'POS' => 'Scroll bar position.'
			),
		),
		"pageup" => array(
            '__desc' => "When the user increases the value in large increments by clicking on the scrollbar thumb track.",
			'__cmd' => 'POS',
			'__eg' => '90',
			'__params' => array(
				'POS' => 'Scroll bar position.'
			),
		),
		"pagedown" => array(
            '__desc' => "When the user decreases the value in large increments by clicking on the scrollbar thumb track.",
			'__cmd' => 'POS',
            '__eg' => '80',
			'__params' => array(
				'POS' => 'Scroll bar position.'
			),
		),
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
	);
}
?>
