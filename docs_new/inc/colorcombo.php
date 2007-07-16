<?php
function get_intro_colorcombo() {
	echo "This control enables you to create a colorcombo.";
}





function get_xdid_colorcombo(&$XDID) {
	$XDID = array(
	    'a' => array(
	        '__desc' => 'This command lets you add a color to the colorcombo at the specified position.',
	        '__cmd' => '[N] [RGB]',
	        '__eg' => '2 $rgb(0,0,255)',
	        '__notes' => array(
	            "If N is 0, inserts color at the end of the list."
			),
		),
		'c' => array(
	        '__desc' => 'This command lets you select the Nth item in the colorcombo.',
	        '__cmd' => '[N]',
	        '__eg' => '2',
	        '__notes' => 'If [p]N[/p] is [v]0[/v], it unselects the current color.',
		),
		'd' => array(
	        '__desc' => 'This command lets you delete the Nth item in the colorcombo.',
	        '__cmd' => '[N]',
	        '__eg' => "2",
		),
		'm' => array(
	        '__desc' => 'This command lets you load all the mIRC colors into the colorcombo.',
		),
		'o' => array(
	        '__desc' => 'This command lets you overwrite the Nth item in the colorcombo.',
	        '__cmd' => '[N] [RGB]',
	        '__eg' => '2 $rgb(255,0,0)',
		),
		'r' => array(
	        '__desc' => 'This command lets you clear the contents of the colorcombo.',
		),
	);
}

function get_xdidprops_colorcombo(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"num" => array(
		    '__desc' => "This property lets you retreive the number of items in the colorcombo.",
		),
		"color" => array(
		    '__desc' => 'This property lets you retreive the Nth colorcombo color.',
		    '__cmd' => 'N',
		    '__eg' => '1',
		),
		"sel" => array(
		    '__desc' => 'This property lets you the selected item number in the colorcombo.',
		),
	);
}

function get_events_colorcombo(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => array(
			'__desc' => "When a sclick event is fired on an item.",
			'__cmd' => 'ITEM',
			'__params' => array(
			    'ITEM' => "The item number that was selected",
			),
		),
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
