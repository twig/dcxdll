<?php
function get_intro_calendar() {
	echo "Calendar control. This control is currently incomplete.";
}


function get_styles_calendar(&$STYLES) {
	$STYLES = array(
	    "multi" => 'The month calendar will allow the user to select a range of dates within the control. By default, the maximum range is one week.',
	    'notoday' => 'The month calendar control will not display the "today" date at the bottom of the control.',
	    'notodaycircle' => 'The month calendar control will not circle the "today" date.',
	    'weeknum' => 'The month calendar control will display week numbers (1-52) to the left of each row of days. Week 1 is defined as the first week that contains at least four days.',
	);
}


function get_xdid_calendar(&$XDID) {
	$XDID = array(
		't' => array(
	        '__desc' => 'This command lets you set the text.',
	        '__cmd' => '(Text)',
	        '__eg' => 'Check Text',
		),
	);
}


function get_xdidprops_calendar(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the text.",
		),
	);
}


function get_events_calendar(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When a date is clicked.",
//	    "dclick" => "When the text is double-clicked.",
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
		'selchange' => 'When the date selection is changed.',
		'select' => 'When a new date is selected.',
	);
}
?>
