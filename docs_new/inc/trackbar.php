<?php
function get_intro_trackbar() {
	echo "This control enables you to create a track bar.";
}


function get_styles_trackbar(&$STYLES) {
	$STYLES = array(
		'alpha' => 'The control is alpha blended.',
		"autoticks" => "Trackbar ticks are displayed at every N intervals (set by <a>/xdid -n</a>)",
		"both" => "Tic marks are displayed on both side of the bar.",
		"top" => "Tic marks appear on the top of the trackbar. ([s]horizontal[/s] only)",
		'bottom' => 'Tic marks appear on the bottom of the trackbar. ([s]horizontal[/s] only)',
		'left' => 'Tic marks appear on the left of the trackbar. ([s]vertical[/s] only)',
		'right' => 'Tic marks appear on the right of the trackbar. ([s]vertical[/s] only)',
		'vertical' => 'Vertical track bar.',
		"nothumb" => "No track bar thumb appears on the trackbar.",
		"noticks" => "No tic marks appear on track bar.",
		"downisleft" => "By default, the trackbar control uses down equal to right and up equal to left. Use this style to reverse the default, making down equal left and up equal right.",
		'tooltips' => 'Tooltip thumb movement displays a tooltip.',
		'select' => 'Enable selection points for a trackbar.',
		
		'__notes' => 'You must specify [s]notheme[/s] when using [s]select[/s]. Otherwise it will not display the selected range on themed desktops. [o]XP+[/o].',
	);
}


function get_xdid_trackbar(&$XDID) {
	$XDID = array(
		'c' => array(
			'__desc' => 'This command lets you add a tick mark at the specified position.',
			'__cmd' => '[POS]',
			'__eg' => '75',
		),
		'g' => array(
			'__desc' => 'This command lets you set images for specific parts of the trackbar.',
			'__cmd' => '[+FLAGS] [FILENAME]',
			'__eg' => '+dt C:/mIRC/trackbar.bmp',
			'__params' => array(
				"+FLAGS" => array(
					'__desc' => "Which trackbar component to apply the image to.",
					'__values' => array(
						"b" => "Trackbar background.",
						"c" => "Trackbar channel (track).",
						"d" => "Trackbar dragged thumb.",
						"t" => "Trackbar normal thumb.",
					),
				),
			),
			'__notes' => 'Currently only supports BMP files.',
		),
		'j' => array(
	        '__desc' => 'This command lets you set the trackbar selection.',
	        '__cmd' => '[MIN] [MAX]',
	        '__eg' => '0 75',
		),
		'l' => array(
	        '__desc' => 'This command lets you set the line step value.',
	        '__cmd' => '[VALUE]',
	        '__eg' => "5",
	        '__notes' => array(
	            "See [e]Lineup[/e] and [e]Linedown[/e] events.",
			),
		),
		'm' => array(
	        '__desc' => 'This command lets you set the page step value',
	        '__cmd' => '[VALUE]',
	        '__eg' => "10",
	        '__notes' => array(
	            "See [e]Pageup[/e] and [e]Pagedown[/e] events.",
			),
		),
		'n' => array(
	        '__desc' => 'This command lets you set the tick marks frequency.',
	        '__cmd' => '[VALUE]',
	        '__eg' => '10',
	        '__notes' => array(
	            "[s]autoticks[/s] style must be used.",
			),
		),
		'o' => array(
	        '__desc' => 'This command lets you set the transparency color for trackbar bitmaps.',
	        '__cmd' => '[VALUE]',
	        '__eg' => '$rgb(255,0,255)',
	        '__notes' => 'Use [v]-1[/v] for no transparency color.',
		),
		'q' => array(
	        '__desc' => 'This command lets you clear the tick marks.',
		),
		'r' => array(
	        '__desc' => 'This command lets you set the trackbar range.',
	        '__cmd' => '[MIN] [MAX]',
	        '__eg' => '0 100',
		),
		't' => array(
	        '__desc' => 'This command lets you set the trackbar tracker tooltip position.',
	        '__cmd' => '[POSITION]',
	        '__eg' => 'right',
	        '__params' => array(
	            "POSITION" => array(
	                '__desc' => "Tooltip position",
	                '__values' => array(
	                    "top" => "Tooltip appears over the tracker.",
	                    "bottom" => "Tooltip appears under the tracker.",
						"left" => "Tooltip appears left of the tracker.",
						"right" => "Tooltip appears right of the tracker.",
					),
				),
			),
		),
		'u' => array(
	        '__desc' => 'This command lets you set the trackbar thumb tracker length in pixels.',
	        '__cmd' => '[LENGTH]',
	        '__eg' => '10',
		),
		'v' => array(
	        '__desc' => 'This command lets you set the tracker position.',
	        '__cmd' => '[POS]',
	        '__eg' => '50',
		),
	);
}

function get_xdidprops_trackbar(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"value" => "This property lets you retreive the track bar position value.",
		"range" => 'This property lets you retreive the track bar range.',
		"line" => 'This property lets you retreive the track bar line step value.',
		"page" => "This property lets you retreive the track bar page step value.",
		"selrange" => 'This property lets you retreive the selected track bar range.',
	);
}

function get_events_trackbar(&$EVENTS) {
	$EVENTS = array(
		"trackend" => array(
			'__desc' => "When the user releases the left mouse button after dragging the thumb.",
			'__cmd' => 'POS',
			'__eg' => '73',
			'__params' => array(
				'POS' => "Track bar position."
			)
		),
		"top" => array(
			'__desc' => "When the user presses the home key.",
			'__cmd' => 'POS',
			'__eg' => '0',
			'__params' => array(
				'POS' => "Track bar position."
			)
		),
		"bottom" => array(
			'__desc' => "When the user presses the end key.",
			'__cmd' => 'POS',
			'__eg' => '100',
			'__params' => array(
				'POS' => "Track bar position."
			)
		),
		"lineup" => array(
			'__desc' => "When the user increases the value by using the arrow keys.",
			'__cmd' => 'POS',
			'__eg' => '89',
			'__params' => array(
				'POS' => "Track bar position."
			)
		),
		"linedown" => array(
			'__desc' => "When the user decreases the value by using the arrow keys.",
			'__cmd' => 'POS',
			'__eg' => '71',
			'__params' => array(
				'POS' => "Track bar position."
			)
		),
		"pageup" => array(
			'__desc' => "When the user increases the value in large increments by clicking on the trackbar thumb track.",
			'__cmd' => 'POS',
			'__eg' => '90',
			'__params' => array(
				'POS' => "Track bar position."
			)
		),
		"pagedown" => array(
			'__desc' => "When the user decreases the value in large increments by clicking on the trackbar thumb track.",
			'__cmd' => 'POS',
			'__eg' => '80',
			'__params' => array(
				'POS' => "Track bar position."
			)
		),
		'showtip' => array(
			'__desc' => 'When a tooltip is about to be displayed for the trackbar.',
			'__cmd' => 'POS',
			'__eg' => '83',
			'__params' => array(
				'POS' => 'Track bar position.',
			),
			'__return' => '[r]TEXT[/r] to change the tooltip text.',
		),
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
