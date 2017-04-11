<?php
function get_intro_pager() {
	echo 'The pager control manages the automatic scrolling of its child controls. This can be created within a dialog, panel or box control. It can contain a toolbar, rebar, panel, tab, box, image, pager, @window or dialog control.';
}

function get_styles_pager(&$STYLES) {
	$STYLES = array(
		'horizontal' => 'Sets the pager control as horizontal. (Default is vertical)',
		'autoscroll' => 'Causes the control to scroll when the mouse is over the scroll button.',
	);
}

function get_xdid_pager(&$XDID) {
	$XDID = array(
	    'b' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => '[WIDTH]',
	        '__eg' => '100',
		),
	    'c' => array(
			'__desc' => 'This command lets you create a single child control.',
			'__cmd' => '[CID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)',
			'__eg' => '12 rebar 10 10 400 25 tabstop',
			'__params' => array(
			    'CID' => 'Unique control ID for the DCX Control.',
				'CONTROL' => 'The type of DCX Control to create. Values can be:<br />
[v]toolbar[/v]<br />
[v]box[/v], [v]image[/v]<br />
[v]pager[/v], [v]panel[/v], [v]rebar[/v], [v]tab[/v]<br />
[v]dialog[/v] or [v]window[/v].
				',
				'X' => 'X position of control.',
				'Y' => 'Y position of control.',
				'W' => 'Width of control.',
				'H' => 'Height of control.',
				'OPTIONS' => 'Optional styles and options available on each individual controls.',
			),
			'__notes' => 'If docking a mIRC @Window or mIRC Dialog, the first parameter in [p]OPTIONS[/p] must be the @window/dialog name.',
		),
		'd' => array(
	        '__desc' => 'This command lets delete the child control.',
	        '__cmd' => '[CID]',
	        '__eg' => '12',
			'__params' => array(
			    'CID' => "Control ID for the DCX Control.",
			),
		),
		's' => array(
	        '__desc' => 'This command lets you set the scroll button size.',
	        '__cmd' => '[SIZE]',
	        '__eg' => '10',
	        '__params' => array(
	            'SIZE' => 'Size of the scroll button.',
			),
		),
		't' => array(
	        '__desc' => 'This command lets sets the background and scroll button color.',
	        '__cmd' => '[COLOR]',
	        '__eg' => '$rgb(255,0,255)'
		),
		'z' => 'This command the pager to recalculate its size. This should be done after adding a button to a toolbar child control.',
	);
}

function get_xdidprops_pager(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"bsize" => "This property lets you retreive the button size.",
		"color" => 'This property lets you retreive the background color.',
		"border" => 'This property lets you retreive border size.',
	);
}

function get_events_pager(&$EVENTS) {
	$EVENTS = array(
	    "help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
