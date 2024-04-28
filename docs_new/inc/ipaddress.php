<?php
function get_intro_ipaddress() {
	echo "IP Address control.<br/>Root control: <a href='https://learn.microsoft.com/en-gb/windows/win32/controls/ip-address-control-reference'>IP Address</a>";
}

function get_styles_ipaddress(&$STYLES) {
	$STYLES = array(
	    'tooltips' => 'The ipaddress will have a tooltip.',
	);
}

function get_xdid_ipaddress(&$XDID) {
	$XDID = array(
	    'a' => array(
	        '__desc' => 'This command lets you set the IP Address value.',
	        '__cmd' => '[IP]',
	        '__eg' => '255.255.0.0',
		),
		'g' => array(
	        '__desc' => 'This command lets you set the range of one of the four IP Address fields.',
	        '__cmd' => '[N] [MIN] [MAX]',
	        '__eg' => '2 10 20',
		),
		'j' => array(
	        '__desc' => 'This command lets you set the focus on one of the four IP Address fields.',
	        '__cmd' => '[N]',
	        '__eg' => "3",
		),
		'r' => array(
	        '__desc' => 'This command lets you clear the IP Address control.',
		),
	);
}

function get_xdidprops_ipaddress(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"ip" => array(
		    '__desc' => "This property lets you retreive the ip.",
		),
	);
}

function get_events_ipaddress(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When you left-click on the IpAddress control.",
	    "edit" => "When a field in the control changes.",
	    "rclick" => "When you right-click on the IP Address control.",
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
	);
}
?>
