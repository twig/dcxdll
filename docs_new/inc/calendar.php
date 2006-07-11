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
	    'daystate' => 'You can specify whether specific dates are bold or not. Refer to [e]daystate[/e] event.',
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
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
		'selchange' => 'When the date selection is changed.',
		'select' => array(
			'__desc' => 'When a new date is selected.',
			'__cmd' => 'START (END)',
			'__eg' => '1152622371 1152722371',
			'__params' => array(
				'START' => 'The first date selected.',
				'END' => 'The last date selected. This is not sent unless [s]multi[/s] style is specified.',
			),
		),
		'daystate' => array(
			'__desc' => "When the month is JUST about to be shown. Return dates which you wish to be bolded.",
			'__cmd' => 'OFFSET_DATE',
			'__eg' => '1152622371',
			'__params' => array(
				'OFFSET_DATE' => 'The date which is being presented to you. Each time a month is shown (eg. July), the [e]daystate[/e] event will trigger for June, July, and August.',
			),
			'__return' => "[r]N,N,N...N[/r] Return a comma seperated string indicating the days to bold.",
			'__notes' => array(
		 		'This may seem difficult to figure out at first. Use $asctime() to determine which month is being requested by the Calendar control.',
		 		'The values returned are bolded. Returning [v]1,3,10[/v] will bold the 1st, 3rd and 10th.',
			),
		),
	);
}
?>
