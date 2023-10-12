<?php
function get_intro_datetime() {
	echo 'The DateTime Picker control allows users to select a date or time in a neat way, automatically enclosing the calendar control and also providing UpDown spinners on date/time values.<br />';
	echo '<br />';
	echo 'It is highly recommended that you familiarise yourself with how the mIRC functions $ctime and $asctime() functions work before using this control.<br />';
}


function get_styles_datetime(&$STYLES) {
	$STYLES = array(
	    "longdateformat" => 'The date will be displayed in a long format, which produces a result like [v]Friday, April 19, 1996[/v].',
	    'shortdateformat' => 'The date will be displayed in a short format, which produces a result like [v]4/19/96[/v].',
	    'shortdatecenturyformat' => 'The date will be similar to the [s]shortdateformat[/s], however the result will include the century like [v]4/19/1996[/v].',
	    'timeformat' => 'Displays the time like [v]5:31:42 PM[/v].',
	    'right' => 'The text value will be aligned to the right.',
	    'shownone' => 'It is possible to have no date selected in the control. The user MUST select the checkbox, otherwise it will return no date.',
	    'updown' => 'Places an up-down control to the right of the DTP control to modify date-time values.',
	);
}


function get_xdid_datetime(&$XDID) {
	$XDID = array(
		'k' => array(
			'__desc' => 'This command lets you change the calendar colour.',
			'__cmd' => '[+FLAGS] [COLOR] or +A [COLOR] [COLOR] [COLOR] [COLOR] [COLOR] [COLOR]',
			'__eg' => array(
				'+bi $rgb(255,0,255)',
				'+A $rgb(255,0,255) $rgb(0,255,255) $rgb(0,0,255) $rgb(0,128,0) $rgb(255,128,255) $rgb(128,128,128)',
			),
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
						'A' => 'Set all the colours at once. This flag cannot be used in combination with any other.',
					),
				),
				'COLOR' => 'The color to set.',
			),
		),
		'o' => array(
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
		'D' => array(
			'__desc' => "This command lets you change controls dropdown font.",
			'__cmd' => "[+FLAGS] [CHARSET] [SIZE] [FONTNAME]",
			'__eg' => "+abs ansi 10 Tahoma",
			'__params' => array(
			    '+FLAGS' => array(
			        '__desc' => "Font style flags.",
                    '__values' => array(
                        'a' => "Font is anti-aliased.",
						'b' => "Font is bold.",
						'd' => "Default font.",
						'i' => "Font is italic.",
						's' => "Font is strikedout.",
						'u' => "Font is underlined.",
					),
				),
				'CHARSET' => "Can be any of the following: [v]ansi[/v], [v]baltic[/v], [v]chinesebig[/v], [v]default[/v], [v]easteurope[/v], [v]gb2312[/v], [v]greek[/v], [v]hangul[/v], [v]mac[/v], [v]oem[/v], [v]russian[/v], [v]shiftjis[/v], [v]symbol[/v], [v]turkish[/v], or [v]vietnamese[/v].",
				'SIZE' => "Font size in points.",
				'FONTNAME' => "Font name. Ex: Tahoma, Courier New, Arial, ...",
			),
			'__notes' => array(
				"Font names are limited to 31 chars (system definition).",
				'When using [v]+d[/v], [p]CHARSET[/p] [p]SIZE[/p] and [p]FONTNAME[/p] can be omitted.'
			),
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
		"text" => "This property lets you retreive the selected date/time as plain text.",
		"ideal" => "This property lets you retreive the ideal width and height of the control.",
		"calcolor" => array(
			'__desc' => "This property lets you retreive the colours of the control.",
	        '__cmd' => '[COLOR]',
	        '__eg' => array(
				'background',
				'monthbk',
				'text',
				'titlebk',
				'titletext',
				'trailingtext',
				'all',
			),
			'__params' => array(
				'background' => 'The background colour.',
				'monthbk' => 'The months background colour.',
				'text' => 'The text colour.',
				'titlebk' => 'The titles background.',
				'titletext' => 'The title text colour.',
				'trailingtext' => 'The trailing text colour.',
				'all' => 'All the above at once.',
			),
			'__notes' => "The [v]all[/v] arg returns all the colours at once.",
		),
		"calfont" => array(
			'__desc' => "This property lets you retreive the current fonts details.",
			'__notes' => 'The return value is in the format [v]+flags[/v] [v]charset[/v] [v]size[/v] [v]fontname[/v], where [v]+flags[/v] is specfied in /xdid -f',
		),
		"calstyle" => "This property lets you retreive the calendar controls current styles.",
	);
}


function get_events_datetime(&$EVENTS) {
	$EVENTS = array(
                'closed' => 'The dropdown calendar was closed.',
                'open' => 'The dropdown calendar was opened.',
		'change' => array(
			'__desc' => 'When the date/time selection has changed.',
			'__cmd' => 'SELECTION',
			'__eg' => array(
               '1152622371',
               'none'
            ),
			'__params' => array(
				'SELECTION' => 'The date selected. If this value is [v]none[/v], then the date has been cleared.',
			),
			'__notes' => '[p]SELECTION[/p] can only be [v]none[/v] when the [s]shownone[/s] style is used.',
		),
	);
}
?>
