<?php
function get_intro_panel() {
	echo "The panel control is like an empty borderless dialog which is used to host other controls.";
}

function get_styles_panel(&$STYLES) {
    $STYLES = array(
        'alpha' => 'Control is alpha blended.',
        'hgradient' => 'Draws a horizontal gradient with the colors specified.',
	'vgradient' => 'Draws a vertical gradient with the colors specified.',
    );
}

function get_xdid_panel(&$XDID) {
	global $CLA;

	$XDID = array(
		"c" => array(
			'__desc' => "This command lets you add a child control to a panel control.",
			'__cmd' => "[CID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)",
			'__eg' => "12 richedit 10 10 400 25 multi",
			'__params' => array(
			    'CID' => "Unique control ID for the DCX Control. <b>Must be unique for all the controls of the dialog!</b>",
				'CONTROL' => 'The type of DCX Control to create. Values can be:<br />
[v]button[/v], [v]calendar[/v], [v]colorcombo[/v], [v]comboex[/v], [v]datetime[/v], [v]directshow[/v], [v]ipaddress[/v], [v]listview[/v], [v]pbar[/v], [v]richedit[/v], [v]statusbar[/v], [v]toolbar[/v], [v]trackbar[/v], [v]treeview[/v], [v]updown[/v], [v]webctrl[/v]<br />
[v]box[/v], [v]check[/v], [v]edit[/v], [v]image[/v], [v]line[/v], [v]link[/v], [v]list[/v], [v]radio[/v], [v]scroll[/v], [v]text[/v]<br />
[v]divider[/v], [v]pager[/v], [v]panel[/v], [v]rebar[/v], [v]stacker[/v], [v]tab[/v]<br />
[v]dialog[/v] or [v]window[/v].',
				'X' => "X position of control.",
				'Y' => "Y position of control.",
				'W' => "Width of control.",
				'H' => "Height of control.",
				'OPTIONS' => "Optional styles and options available on each individual controls.",
			),
                        '__notes' => 'If docking a mIRC @Window or mIRC Dialog, the first parameter in [p]OPTIONS[/p] must be the @window/dialog name.',
		),
		'd' => array(
	        '__desc' => "This command lets you delete a control added on the panel.",
	        '__cmd' => '[CID]',
	        '__eg' => '6',
		),
		'l' => $CLA,
		't' => array(
		    '__desc' => "This command lets you set the text for a panel control.",
		    '__cmd' => '[TEXT]',
		    '__eg' => 'New Window Text',
		    '__notes' => 'The panel text (and border) is only visible when used with /xdid -x DIALOG ID +bd',
		),
	);
}


function get_events_panel(&$EVENTS) {
	$EVENTS = array(
	    "help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
		'sclick' => 'When the left mouse button is clicked.',
		'dclick' => 'When the left mouse button is double-clicked.',
		'lbdown' => 'When the left mouse button is pressed.',
		'lbup' => 'When the left mouse button is released.',
		'lbdblclk' => 'When the left mouse button is double-clicked.',
	);
}
?>
