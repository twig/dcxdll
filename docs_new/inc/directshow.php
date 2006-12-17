<?php
function get_intro_directshow() {
	echo 'DIRECTSHOW CONTROL';
}


function get_styles_directshow(&$STYLES) {
	$STYLES = array(
		'transparent' => 'This allows the background of the directshow control to be transparent and the video border not rendered.',
		'fixratio' => 'Causes the content to maintain its aspect ratio, without this style the content fills the control.',
	);
}


function get_xdid_directshow(&$XDID) {
	$XDID = array(
		'c' => array(
			'__desc' => "This command lets you load and unload any previous file.",
			'__cmd' => '[COMMAND]',
			'__eg' => 'play',
			'__params' => array(
				'COMMAND' => array(
					'__desc' => "Play commands.",
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
		'a' => array(
			'__desc' => "This command lets you load and unload any previous file.",
			'__cmd' => '[+FLAGS] [FILENAME]',
			'__eg' => '+ File.mpeg',
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
	);
}

function get_xdidprops_directshow(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"size" => array(
			'__desc' => 'TODO: This property returns the media size width height ARwidth ARheight.',
			'__params' => array(
				'width' => "width",
				'height' => "height",
				'ARwidth' => "arwidth",
				'ARheight' => "arheight",
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
