<?php
function get_intro_xdock() {
	echo "XDOCK USED FOR DOCKING! DUH!.";
}

function get_xdock(&$XDOCK) {
	$XDOCK = array(
	    'Dock' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N HWND POS NAME',
	        '__eg' => '100',
		),
		'UnDock' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'DockRefresh' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'ShowMenubar' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'ShowSwitchbar' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'ShowToolbar' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'ShowTreebar' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
//	    'c' => array(
//			'__desc' => 'This command lets you create a single child control.',
//			'__cmd' => '[ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)',
//			'__eg' => '12 rebar 10 10 400 25 tabstop',
//			'__params' => array(
//			    'ID' => 'Unique control ID for the DCX Control.',
//				'CONTROL' => array(
//					'__desc' => 'The type of DCX Control to be created.',
//					'__values' => array(
//						'toolbar' => 'Creates a toolbar control.',
//						'rebar' => 'Creates a rebar control.',
//						'box' => 'Creates a box control.',
//						'image' => 'Creates a image control.',
//						'pager' => 'Creates a pager control.',
//						'panel' => 'Creates a panel control.',
//						'tab' => 'Creates a tab control.',
//						'window' => 'Docks a mIRC @window. The first parameter in [p]OPTIONS[/p] must be the @window name.',
//						'dialog' => 'Docks a mIRC dialog. The first parameter in [p]OPTIONS[/p] must be the dialog name.',
//					),
//				),
//				'X' => 'X position of control.',
//				'Y' => 'Y position of control.',
//				'W' => 'Width of control.',
//				'H' => 'Height of control.',
//				'OPTIONS' => 'Optional styles and options available on each individual controls.',
//			),
//		),
	);
}

function get_xdockprops(&$XDIDPROPS) {
	$XDIDPROPS = array(
		'DockPos' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'NAME',
	        '__eg' => '100',
		),
		'DockSide' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'NAME',
	        '__eg' => '100',
		),
		'SBSize' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'TBSize' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'SBPos' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'IsSwitchbar' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'IsToolbar' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'IsMenubar' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
		'IsTreebar' => array(
	        '__desc' => 'This command lets you sets border width.',
	        '__cmd' => 'N',
	        '__eg' => '100',
		),
	);
}

//function get_events_xdock(&$EVENTS) {
//	$EVENTS = array(
//	    "help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
//	);
//}
?>
