<?php
function get_intro_datetime() {
	echo 'The DateTime Picker control allows users to select a date or time in a neat way, automatically enclosing the calendar control and also providing UpDown spinners on date/time values.<br />';
	echo '<br />';
	echo 'It is highly recommended that you familiarise yourself with how the mIRC functions $ctime and $asctime() functions work before using this control.<br />';
}


function get_styles_datetime(&$STYLES) {
	$STYLES = array(
	    "longdateformat" => '***The month calendar will allow the user to select a range of dates within the control. By default, the maximum range is one week.',
	    'shortdateformat' => '***The month calendar control will not display the "today" date at the bottom of the control.',
	    'shortdatecenturyformat' => '***The month calendar control will not circle the "today" date.',
	    'timeformat' => '***The month calendar control will display week numbers (1-52) to the left of each row of days. Week 1 is defined as the first week that contains at least four days.',
	    'right' => '***You can specify whether specific dates are bold or not. Refer to [e]daystate[/e] event.',
	    'shownone' => '***You can specify whether specific dates are bold or not. Refer to [e]daystate[/e] event.',
	    'updown' => '***You can specify whether specific dates are bold or not. Refer to [e]daystate[/e] event.',
	);
}


function get_xdid_datetime(&$XDID) {
	$XDID = array(
		'f' => array(
			'__desc' => 'This command lets you set format of the DateTime Picker text.',
			'__cmd' => '[FORMAT]',
			'__eg' => "'Right now it is: 'hh':'m':'s ddddMMMdd', 'yyy",
            '__params' => array(
                'FORMAT' => array(
                    '__desc' => "Date time format options.",
                    '__values' => array(
                        "d" => "The one- or two-digit day.",
                        "dd" => "The two-digit day. Single-digit day values are preceded by a zero.",
                        "ddd" => "The three-character weekday abbreviation.",
                        "dddd" => "The full weekday name.",
                        "h" => "The one- or two-digit hour in 12-hour format.",
                        "hh" => "The two-digit hour in 12-hour format. Single-digit values are preceded by a zero.",
                        "H" => "The one- or two-digit hour in 24-hour format.",
                        "HH" => "The two-digit hour in 24-hour format. Single-digit values are preceded by a zero.",
                        "m" => "The one- or two-digit minute.",
                        "mm" => "The two-digit minute. Single-digit values are preceded by a zero.",
                        "M" => "The one- or two-digit month number.",
                        "MM" => "The two-digit month number. Single-digit values are preceded by a zero.",
                        "MMM" => "The three-character month abbreviation.",
                        "MMMM" => "The full month name.",
                        "t" => 'The one-letter AM/PM abbreviation (that is, AM is displayed as "A").',
                        "tt" => 'The two-letter AM/PM abbreviation (that is, AM is displayed as "AM").',
                        "yy" => 'The last two digits of the year (that is, 1996 would be displayed as "96").',
                        "yyyy" => 'The full year (that is, 1996 would be displayed as "1996").',
                    ),
                ),
            ),
			'__notes' => array(
                'Any non-format characters must be enclosed within single quotes.',
                'A DTP control tracks locale changes when it is using the default format string.',
            ),
		),
//		'-' => array(
//			'__desc' => 'This command lets you change the calendar style.',
//			'__cmd' => '[+FLAGS] [COLOR]',
//			'__eg' => '+bi $rgb(255,0,255)',
//			'__params' => array(
//				'+FLAGS' => array(
//					'__desc' => "Calendar flags.",
//					'__values' => array(
//						'b' => 'Calendar background.',
//						'g' => 'Month background.',
//						't' => 'Day text.',
//						'i' => 'Title background and selected day background color.',
//						'a' => 'Title text and selected day text color.',
//						'r' => 'Trailing text (days not included in this month).',
//					),
//				),
//				'COLOR' => 'The color to set.',
//			),
//		),
//		'-' => array(
//			'__desc' => 'This command lets you set maximum number of days you can select at any one time.',
//			'__cmd' => '[MAX]',
//			'__eg' => '5',
//			'__notes' => 'The default value for [p]MAX[/p] is [v]7[/v].',
//		),
		'r' => array(
			'__desc' => 'This command lets you set the range of the first and last selectable dates.',
			'__cmd' => '[MIN_TIMESTAMP] [MAX_TIMESTAMP]',
			'__eg' => array(
				'$ctime(3rd July 2006) $ctime',
				'nolimit $ctime',
			),
			'__notes' => 'You can use [v]nolimit[/v] for [p]MIN_TIMESTAMP[/p] or [p]MAX_TIMESTAMP[/p] if you wish to specify no minimum or maximum date range respectively.',
		),
//		'-' => array(
//			'__desc' => 'This command lets you select a date, or range of dates.',
//			'__cmd' => '[MIN_TIMESTAMP] (MAX_TIMESTAMP)',
//			'__eg' => '$ctime(3rd July 2006) $ctime',
//			'__notes' => array(
//				'[p]MAX_TIMESTAMP[/p] optional for the [s]multi[/s] style. If no [p]MAX_TIMESTAMP[/p] is specified, the selected date will only be [p]MIN_TIMESTAMP[/p].',
//				'[p]MAX_TIMESTAMP[/p] is ignored for a single select calendar.',
//			),
//		),
		't' => array(
			'__desc' => 'This command lets you set the selected date/time.',
			'__cmd' => '[TIMESTAMP]',
			'__eg' => '$ctime(3rd July 3006)',
            '__notes' => 'You can use [v]reset[/v] for [p]TIMESTEMP[/p] to set it back to the current date/time, but this only applies to the [s]shownone[/s] style.',
		),
	);
}


function get_xdidprops_datetime(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"range" => array(
		    '__desc' => "This property lets you retreive the date/time range.",
			'__notes' => '[v]nolimit[/v] is returned when the calendar range is not bounded.',
		),
		"value" => "This property lets you retreive the selected date/time.",
	);
}


function get_events_datetime(&$EVENTS) {
	$EVENTS = array(
//	    "sclick" => "When a date is clicked.",
//		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
//		'selchange' => 'When the date selection is changed.',
//		'select' => array(
//			'__desc' => 'When a new date is selected.',
//			'__cmd' => 'START (END)',
//			'__eg' => '1152622371 1152722371',
//			'__params' => array(
//				'START' => 'The first date selected.',
//				'END' => 'The last date selected. This is not sent unless [s]multi[/s] style is specified.',
//			),
//		),
//		'daystate' => array(
//			'__desc' => "When the month is JUST about to be shown. Return dates which you wish to be bolded.",
//			'__cmd' => 'OFFSET_DATE',
//			'__eg' => '1152622371',
//			'__params' => array(
//				'OFFSET_DATE' => 'The date which is being presented to you. Each time a month is shown (eg. July), the [e]daystate[/e] event will trigger for June, July, and August.',
//			),
//			'__return' => "[r]N,N,N...N[/r] Return a comma seperated string indicating the days to bold.",
//			'__notes' => array(
//		 		'This may seem difficult to figure out at first. Use $asctime() to determine which month is being requested by the Calendar control.',
//		 		'The values returned are bolded. Returning [v]1,3,10[/v] will bold the 1st, 3rd and 10th.',
//			),
//		),
	);
}
?>
