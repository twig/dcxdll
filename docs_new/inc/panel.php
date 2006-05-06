<?php
function get_intro_panel() {
	echo "The panel control is like an empty borderless dialog which is used to host other controls.";
}


function get_xdid_panel(&$XDID) {
	global $CLA;

	$XDID = array(
		"c" => array(
			'__desc' => "This command lets you add a child control to a box control.",
			'__cmd' => "[ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)",
			'__eg' => "12 richedit 10 10 400 25 multi",
			'__params' => array(
			    'ID' => "Unique control ID for the DCX Control. <b>Must be unique for all the controls of the dialog!</b>",
				'CONTROL' => array(
					'__desc' => "The type of DCX Control to be created.",
					'__values' => array(
						'pbar' => "Creates a Progressbar control.",
						'treeview' => "Creates a TreeView control.",
						'toolbar' => "Creates a Toolbar control.",
						'statusbar' => "Creates a Statusbar control.",
						'comboex' => "Creates a ComboEx control.",
						'listview' => "Creates a Listview control.",
						'trackbar' => "Creates a Trackbar control.",
						'rebar' => "Creates a Rebar control.",
						'colorcombo' => "Creates a ColorCombo control.",
						'button' => "Creates a Button control.",
						'richedit' => "Creates a RichEdit control.",
						'ipaddress' => "Creates a IpAddress control.",
						'updown' => "Creates an UpDown control.",
						'webctrl' => "Creates a Web control.",
						'&nbsp;' => '&nbsp;',
						'line' => "Creates a Line control.",
						'box' => "Creates a Box control.",
						'radio' => "Creates a Radio control.",
						'check' => "Creates a Check control.",
						'edit' => "Creates a Edit control.",
						'scroll' => "Creates a Scroll control.",
						'image' => "Creates a Image control.",
						'list' => "Creates a List control.",
						'link' => "Creates a Link control.",
						'text' => "Creates a Text control.",
						'&nbsp;&nbsp;' => '&nbsp;',
						'divider' => "Creates a Divider control.",
						'panel' => "Creates a Panel control.",
						'tab' => "Creates a Tab control.",
						'&nbsp;&nbsp;' => '&nbsp;',
						'window' => "Docks a mIRC @window. The first parameter in [p]OPTIONS[/p] must be the @window name.",
						'dialog' => "Docks a mIRC dialog. The first parameter in [p]OPTIONS[/p] must be the dialog name.",
					),
				),
				'X' => "X position of control.",
				'Y' => "Y position of control.",
				'W' => "Width of control.",
				'H' => "Height of control.",
				'OPTIONS' => "Optional styles and options available on each individual controls.",
			),
		),
		'd' => array(
	        '__desc' => "This command lets you delete a control added on the panel.",
	        '__cmd' => '[CID]',
	        '__eg' => '6',
		),
		'l' => $CLA,
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
