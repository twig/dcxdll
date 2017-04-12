<?php
function get_intro_directshow() {
	echo 'The DirectShow control allows playback of media files. Formats which can be played will depend upon the codecs avaiable on the system.';
}


function get_styles_directshow(&$STYLES) {
	$STYLES = array(
		'fixratio' => 'The content maintains its aspect ratio. By default, the content fills the control.',
        'hgradient' => 'Draws a horizontal gradient with the colors specified.',
		'vgradient' => 'Draws a vertical gradient with the colors specified.',
	);
}


function get_xdid_directshow(&$XDID) {
	$XDID = array(
        'a' => array(
            '__desc' => "This command lets you load and unload any previous file.",
            '__cmd' => '[+FLAGS] [FILENAME]',
            '__eg' => '+ fileCopy.mpeg',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Previous flags.",
                    '__values' => array(
                        'p' => "Play right away.",
                        'l' => "Loop playback.",
						'a' => "Specifies audio only, no VMR is loaded, no video rendered.",
                    ),
                ),
                'FILENAME' => 'Previous filename.',
            ),
        ),
		'c' => array(
			'__desc' => "This lets you control playback of the file.",
			'__cmd' => '[COMMAND] [ARGS]',
			'__eg' => array(
                'play',
                'seek 3500'
            ),
			'__params' => array(
				'COMMAND' => array(
					'__desc' => "DirectShow commands.",
					'__values' => array(
						'play' => "Plays the loaded file.",
						'pause' => "Pauses the playback.",
						'stop' => "Stops the playback.",
						'close' => "Close the playback and unload it.",
                                                'seek' => 'Seeks to the millisecond of the clip specified.',
					),
				),
                '__args' => array(
                    'play' => '[FILENAME]',
                    'seek' => array(
                        '__cmd' => '[TIME]',
                        '__params' => array(
                                'TIME' => 'The time to seek to, specified in milliseconds.',
                        ),
                    ),
                ),
			),
		),
        'v' => array(
            '__desc' => "This command sets the brightness, contrast, hue, & saturation",
            '__cmd' => '[+FLAGS] [BRIGHTNESS] [CONTRAST] [HUE] [SATURATION] ',
            '__eg' => '+b 80.00 0 0 0',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Command switches.",
                    '__values' => array(
		                'b' => "Sets the brightness level as a percentage between [v]0.00[/v] & [v]100.00[/v] (fractions are valid values).",
		                'c' => "Sets the contrast level as a percentage between [v]0.00[/v] & [v]100.00[/v] (fractions are valid values).",
		                'h' => "Sets the hue level as a percentage between [v]0.00[/v] & [v]100.00[/v] (fractions are valid values).",
		                's' => "Sets the saturation level as a percentage between [v]0.00[/v] & [v]100.00[/v] (fractions are valid values).",
                    ),
                ),
            ),
        ),
        'V' => array(
            '__desc' => "This command sets the volume or speakerbalance",
            '__cmd' => '[+FLAG] [VALUE] ',
            '__eg' => '+v 80.00',
            '__params' => array(
                '+FLAG' => array(
                    '__desc' => "Command switches.",
                    '__values' => array(
		                'v' => "Sets the volume level as a percentage between [v]0.00[/v] & [v]100.00[/v] (fractions are valid values).",
// TODO: not functional                        'b' => "Sets speaker balance between [v]-10000[/v] & [v]10000[/v] (non functional atm)",
                    ),
                ),
            ),
        ),
	);
}

function get_xdidprops_directshow(&$XDIDPROPS) {
	$XDIDPROPS = array(
        'volume' => 'This property returns volume percentage as a value between [v]0.00[/v] and [v]100.00[/v].',
        'fname' => 'This property returns the filename of the loaded media (if any).',
        'isloaded' => 'This property returns if a file is loaded or not.',
	    'brange' => array(
            '__desc' => 'This property returns the range of values that the video brightness can be set to.',
            '__params' => array(
                'DEFAULT' => 'The default value.',
                'MIN' => 'The minimum value.',
                'MAX' => 'The maximum value.',
                'STEP' => 'The value to use when changing this property.',
            ),
        ),
        'crange' => array(
            '__desc' => 'This property returns the range of values that the video contrast can be set to.',
            '__params' => array(
                'DEFAULT' => 'The default value.',
                'MIN' => 'The minimum value.',
                'MAX' => 'The maximum value.',
                'STEP' => 'The value to use when changing this property.',
            ),
        ),
        'currentpos' => array(
            '__desc' => 'This property returns the current position of the content.',
            '__params' => array(
                'STATE' => 'This is [v]D_OK[/v] if the command succeeded. Otherwise [p]STATE[/p] is [v]D_ERROR[/v].',
                'RESULT' => 'The time in milliseconds. If [p]STATE[/p] was not [v]D_OK[/v], then [p]RESULT[/p] will be the appropriate error message. (ie. [v]Method Not Supported[/v])',
            ),
        ),
        'duration' => array(
            '__desc' => 'This property returns the length of the media clip.',
            '__params' => array(
                'STATE' => 'This is [v]D_OK[/v] if the command succeeded. Otherwise [p]STATE[/p] is [v]D_ERROR[/v].',
                'RESULT' => 'The time in milliseconds. If [p]STATE[/p] was not [v]D_OK[/v], then [p]RESULT[/p] will be the appropriate error message. (ie. [v]Method Not Supported[/v])',
            ),
        ),
        'hrange' => array(
            '__desc' => 'This property returns the range of values that the video hue can be set to.',
            '__params' => array(
                'DEFAULT' => 'The default value.',
                'MIN' => 'The minimum value.',
                'MAX' => 'The maximum value.',
                'STEP' => 'The value to use when changing this property.',
            ),
        ),
        'srange' => array(
            '__desc' => 'This property returns the range of values that the video saturation can be set to.',
            '__params' => array(
                'DEFAULT' => 'The default value.',
                'MIN' => 'The minimum value.',
                'MAX' => 'The maximum value.',
                'STEP' => 'The value to use when changing this property.',
            ),
        ),
		"size" => array(
			'__desc' => 'This property returns the media size.',
			'__params' => array(
				'WIDTH' => "Original width of the content.",
				'HEIGHT' => "Original height of the content.",
				'ARWIDTH' => "Aspect ratio width of the content.",
				'ARHEIGHT' => "Aspect ratio height of the content.",
			),
            '__notes' => 'This returns the media size, not the control size.',
		),
        'state' => array(
                '__desc' => 'This property returns the state of the media file, if any.',
                '__return' => array(
                        'nofile' => 'No file has been specified.',
                        'stopped' => 'Stopped state.',
                        'paused' => 'Paused state.',
                        'playing' => 'Currently playing state.',
                        'unknown' => 'Unknown media state.',
                        'D_ERROR Message' => 'Error in determining state of control.',
                ),
        ),
        'video' => array(
            '__desc' => 'This property returns properties on the video media which can be altered.',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => 'Flags can be any combination of the following.',
                    '__values' => array(
                        'b' => "The brightness of the video.",
                        'c' => "The contrast of the video.",
                        'h' => "The hue of the video.",
                        's' => "The saturation of the video.",
                    ),
                ),
            ),
        ),
	);
}

function get_events_directshow(&$EVENTS) {
	$EVENTS = array(
		'rbdown' => 'right mouse button is down',
		'rbup' => 'right mouse button is up',
		'rclick' => 'right mouse button was clicked',
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
		'rclick' => 'When the right mouse button is clicked.',
		'rbdown' => 'When the right mouse button is pressed.',
		'rbup' => 'When the right mouse button is released.',
	);
}
?>
