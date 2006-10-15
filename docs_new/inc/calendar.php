<?php
function get_intro_calendar() {
	echo "The Calendar control allows users to select a date, allowing the range to be customisable.";
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
		'k' => array(
			'__desc' => 'This command lets you change the calendar style.',
			'__cmd' => '[+FLAGS] [COLOR]',
			'__eg' => '+bi $rgb(255,0,255)',
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Calendar flags.",
					'__values' => array(
						'b' => 'Calendar background.',
						'g' => 'Month background.',
						't' => 'Day text.',
						'i' => 'Title background and selected day background color.',
						'a' => 'Title text and selected day text color.',
						'r' => 'Trailing text (days not included in this month).',
					),
				),
				'COLOR' => 'The color to set.',
			),
		),
		'm' => array(
			'__desc' => 'This command lets you set maximum number of days you can select at any one time.',
			'__cmd' => '[MAX]',
			'__eg' => '5',
			'__notes' => 'The default value for [p]MAX[/p] is [v]7[/v].',
		),
		'r' => array(
			'__desc' => 'This command lets you set the range of the first and last selectable dates.',
			'__cmd' => '[MIN_TIMESTAMP] [MAX_TIMESTAMP]',
			'__eg' => array(
				'$ctime(3rd July 2006) $ctime',
				'nolimit $ctime',
			),
			'__notes' => 'You can use [v]nolimit[/v] for [p]MIN_TIMESTAMP[/p] or [p]MAX_TIMESTAMP[/p] if you wish to specify no minimum or maximum date range.',
		),
		's' => array(
			'__desc' => 'This command lets you select a date, or range of dates.',
			'__cmd' => '[MIN_TIMESTAMP] (MAX_TIMESTAMP)',
			'__eg' => '$ctime(3rd July 2006) $ctime',
			'__notes' => array(
				'[p]MAX_TIMESTAMP[/p] optional for the [s]multi[/s] style. If no [p]MAX_TIMESTAMP[/p] is specified, the selected date will only be [p]MIN_TIMESTAMP[/p].',
				'[p]MAX_TIMESTAMP[/p] is ignored for a single select calendar.',
			),
		),
		't' => array(
			'__desc' => 'This command lets you set the current day.',
			'__cmd' => '[TIMESTAMP]',
			'__eg' => '$ctime(3rd July 3006)',
		),
	);
}


function get_xdidprops_calendar(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"selected" => "This property lets you retreive the selected date range.",
		"range" => array(
		    '__desc' => "This property lets you retreive the calendar date range.",
			'__notes' => '[v]nolimit[/v] is returned when the calendar range is not bounded.',
		),
		"today" => "This property lets you retreive the calendar current day date.",
		"selcount" => "This property lets you retreive the maximum number of selectable days.",
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
