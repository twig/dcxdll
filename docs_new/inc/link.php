<?php
function get_intro_link() {
	echo "Regular link control with icon support.";
}




function get_xdid_link(&$XDID) {
	$XDID = array(
	    'l' => array(
	        '__desc' => 'This command lets you change a color in the link palette.',
	        '__cmd' => '[N] [COLOR]',
	        '__eg' => '2 $rgb(255,0,255)',
	        '__params' => array(
                "N" => "Palette color number 1-4 (see <a>-q</a> for color order).",
 			)
		),
		'q' => array(
	        '__desc' => 'This command lets you load a custom palette into the link control.',
	        '__cmd' => '[NORMAL] [HIGHLIGHT] [VISITED] [DISABLED]',
	        '__eg' => '$rgb(255,0,0) $rgb(0,0,0)',
	        '__notes' => array(
	            "The number of colors is limited to 4, everything past the 4th color is ignored.",
	            "You can give the command a number of colors inferior to 4, it will then only add thoses colors in order to the palette starting at 1.",
			),
		),
		't' => array(
	        '__desc' => 'This command lets you set the link text.',
	        '__cmd' => '(Text)',
	        '__eg' => 'Text Label'
		),
		'w' => array(
	        '__desc' => 'This command lets you set an icon for the link control.',
	        '__cmd' => '[INDEX] [FILENAME]',
	        '__eg' => '113 C:/mIRC/shell.dll',
	        '__params' => array(
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => "Use 0 if the file is a single icon file.",
		),
	);
}

function get_xdidprops_link(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the text.",
		),
	);
}

function get_events_link(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When the text is clicked.",
	    "dclick" => "When the text is double-clicked.",
	    "rclick" => "When you right-click on the text.",
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
