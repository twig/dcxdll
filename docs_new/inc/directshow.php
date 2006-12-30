<?php
function get_intro_directshow() {
	echo 'DIRECTSHOW CONTROL';
}


function get_styles_directshow(&$STYLES) {
	$STYLES = array(
		'transparent' => 'This allows the background of the directshow control to be transparent and the video border not rendered.',
		'fixratio' => 'The content maintains its aspect ratio. By default, the content fills the control.',
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
                        'l' => "Play loop away.",
                    ),
                ),
                'FILENAME' => 'Previous filename.',
            ),
        ),
		'c' => array(
			'__desc' => "This lets you control playback of the file.",
			'__cmd' => '[COMMAND]',
			'__eg' => 'play',
			'__params' => array(
				'COMMAND' => array(
					'__desc' => "DirectShow commands.",
					'__values' => array(
						'play' => "Plays the loaded file.",
						'pause' => "Pauses the playback.",
						'stop' => "Stops the playback.",
						'close' => "Close the playback and unload it.",
					),
				),
				'FILENAME' => 'Previous filename.',
			),
		),
	);
}

function get_xdidprops_directshow(&$XDIDPROPS) {
	$XDIDPROPS = array(
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
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
