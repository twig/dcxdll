<?php
function get_intro_box() {
	echo "The box control is a container control like the panel. It hosts other controls inside and also has the Cell Layout Algorithm available to manage the layout of child controls automatically.";
}


function get_styles_box(&$STYLES) {
	$STYLES = array(
		'center' => 'Control text is centered.',
		'right' => 'Control text is right justified.',
		'bottom' => 'Control text is at the bottom of the box.',
		'squared' => 'Box border is a squared rectangle instead of the default round rectangle.',
		'__notes' => 'Applying the border [s]regular[/s]+[s]dialog frame[/s] border styles on the box will give it a titlebar. (/xdid -x DNAME BOX_ID +bd)'
	);
}


function get_xdid_box(&$XDID) {
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
	        '__desc' => "This command lets you delete a control added on the box.",
	        '__cmd' => '[CID]',
	        '__eg' => '6',
		),
		'l' => $CLA,
		't' => array(
	        '__desc' => 'This command lets you set the group box text.',
	        '__cmd' => '(TEXT)',
	        '__eg' => 'Group Box Text',
		),
	);
}

function get_xdidprops_box(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => "This property lets you retreive the group box text.",
		"inbox" => array(
            '__desc' => 'This property lets you retreive interior frame box size that clears the box border and text.',
            '__notes' => 'The return value is X Y W H.'
		),
	);
}


function get_events_box(&$EVENTS) {
	$EVENTS = array(
	    "help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
