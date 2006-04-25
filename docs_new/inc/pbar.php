<?php
function get_intro_pbar() {
	echo "This control enables you to create a progress bar.";
}

function get_xdid_pbar(&$XDID) {
	$XDID = array(
		"c" => array(
			'__desc' => "This command lets you set the progress bar color.
",
			'__cmd' => "[RGB]",
			'__eg' => "\$rgb(255,0,255)",
            '__notes' => array(
                "Only works with the [s]notheme[/s] style.",
			),
		),
		"i" => array(
			'__desc' => 'This command lets you set the progress bar text. Text is optional and if omited, no text will be displayed. If you put the characters [v]%d[/v] in the text, it will be replaced by the position value of the progressbar as defined by the <a>-j</a> switch. Putting two [v]%%[/v] signs together in the text produces a single [v]%[/v] symbol in the displayed text. Default text is [v]%d %%[/v].',
			'__cmd' => "(Text)",
			'__eg' => 'Completed % $+ d % $+ %',
			'__notes' => array(
			    'Remember that mIRC uses %d as a variable name, you will have to use [v]% $+ d[/v] to insert the text in the control.',
			),
		),
		"j" => array(
			'__desc' => "This command lets you set the progress bar numerical representation where [v]%d[/v] is replaced in the progressbar text.",
			'__cmd' => "[MODE]",
			'__eg' => "p",
			'__params' => array(
				'MODE' => array(
					'__desc' => "Mode letter.",
					'__values' => array(
						'a' => "Absolute position value.",
						'p' => "Percentage position value ([v]default[/v]).",
					),
				),
			),
		),
		"k" => array(
			'__desc' => "This command lets you set the progress bar background color.",
			'__cmd' => "[RGB]",
			'__eg' => "\$rgb(255,255,255)",
			'__notes' => array(
                'Only works with the notheme style.',
			),
		),
		'm[o|g]' => array(
			'__desc' => "This command lets you start/stop the progress bar marquee animation (-mo: start | -mg: stop). The [p]N[/p] value is the interval in milliseconds between each animation (default 100 ms).",
			'__cmd' => "[N]",
			'__eg' => "50",
		),
		"q" => array(
			'__desc' => "This command lets you set the progressbar text color (default is black)",
			'__cmd' => "[RGB]",
			'__eg' => "\$rgb(255,0,255)",
		),
		"r" => array(
			'__desc' => "This command lets you modify the progressbar range of values. (default value 0-100)",
			'__cmd' => "[LOWER] [HIGHER]",
			'__eg' => "0 50",
		),
		"t" => array(
			'__desc' => "This command lets you increase the progressbar by a step defined by the <a>-u</a> flag. When the position exceeds the maximum range value, the progress bar resets the current position so that the progress indicator starts over again from the beginning.",
		),
		"u" => array(
			'__desc' => "TThis command lets you set the progressbar step increase when using the <a>-t</a> switch (default value is 10).",
			'__cmd' => "[N]",
			'__eg' => "20",
		),
		"v" => array(
			'__desc' => "This command lets you set the progressbar value.",
			'__cmd' => "[N]",
			'__eg' => "50",
		),
	);
}


function get_styles_pbar(&$STYLES) {
	$STYLES = array(
		"smooth" => "Progress bar is smooth (no line breaks in the bar)",
		"vertical" => "Progress bar is vertical.",
		"marquee" => "Progress bar is an animated bar. [o]XP+[/o]"
	);
}


function get_xdidprops_pbar(&$XDIDPROPS) {
	$XDIDPROPS = array(
	    "value" => array(
			'__desc' => "This property lets you retreive the progress bar position value.
",
		),
		"range" => array(
		    '__desc' => "This property lets you retreive the progress bar range values.",
		),
		"text" => array(
		    '__desc' => "This property lets you retreive the progress bar text, with the current value.",
		),
	);
}


function get_events_pbar(&$EVENTS) {
	$EVENTS = array(
		"sclick" => array(
            '__desc' => "When you left-click on the progressbar.",
            '__cmd' => "VALUE POS MIN MAX",
            '__params' => array(
                'VALUE' => "Current value of progress bar",
                'POS' => "Position where mouse is when event is fired.",
                'MIN' => "Minimum range value.",
                'MAX' => "Maximum range value.",
			),
		),
		"rclick" => array(
            '__desc' => "When you right-click on the progressbar.",
            '__cmd' => "VALUE POS MIN MAX",
            '__params' => array(
                'VALUE' => "Current value of progress bar",
                'POS' => "Position where mouse is when event is fired.",
                'MIN' => "Minimum range value.",
                'MAX' => "Maximum range value.",
			),
		),
		"mousebar" => array(
            '__desc' => "When the mouse is moved on the progressbar while the left mouse button is held down.",
            '__cmd' => "VALUE POS MIN MAX",
            '__params' => array(
                'VALUE' => "Current value of progress bar",
                'POS' => "Position where mouse is when event is fired.",
                'MIN' => "Minimum range value.",
                'MAX' => "Maximum range value.",
			),
		),
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
