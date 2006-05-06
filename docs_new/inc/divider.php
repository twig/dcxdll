<?php
function get_intro_divider() {
	echo "This control enables you to create a divider control. This control acts as a container for other controls. The divider control is consisted of two panes that host a child control. The panes are resized using the divider bar between them. The divider control resizes automaticaly it's child windows when the bar is moved. Multiple dividers can be nested for advanced UI interfaces.";
}


function get_styles_divider(&$STYLES) {
	$STYLES = array(
		'vertical' => 'The divider has vertical panes.',
	);
}


function get_xdid_divider(&$XDID) {
	$XDID = array(
		"l|-r" => array(
			'__desc' => "This command lets you add a child control to a divider pane. The -l switch is for the left/top pane, and -r for the right/bottom pane.",
			'__cmd' => "[MIN] [IDEAL] [TAB] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)",
			'__eg' => '100 0 $chr(9) 12 richedit 10 10 400 25 multi',
			'__params' => array(
			    'MIN' => "Minimum Pane Width/Height.",
			    'IDEAL' => 'Not used yet. Use [v]0[/v], required but ignored.',
			    'ID' => "Unique control ID for the DCX Control. <b>Must be unique for all the controls of the dialog!</b>",
				'CONTROL' => array(
					'__desc' => "The type of DCX Control to be created.",
					'__values' => array(
						'treeview' => "Creates a TreeView control.",
						'listview' => "Creates a Listview control.",
						'rebar' => "Creates a Rebar control.",
						'richedit' => "Creates a RichEdit control.",
						'webctrl' => "Creates a Web control.",
						'&nbsp;' => '&nbsp;',
						'edit' => "Creates a Edit control.",
						'image' => "Creates a Image control.",
						'list' => "Creates a List control.",
						'&nbsp;&nbsp;' => '&nbsp;',
						'divider' => "Creates a Divider control.",
						'panel' => "Creates a Panel control.",
						'tab' => "Creates a Tab control.",
					),
				),
				'X' => "X position of control. Use [v]0[/v], required but ignored.",
				'Y' => "Y position of control. Use [v]0[/v], required but ignored.",
				'W' => "Width of control.",
				'H' => "Height of control.",
				'OPTIONS' => "Optional styles and options available on each individual controls.",
			),
		),
		'v' => array(
	        '__desc' => "This command lets you set the divider bar position.",
	        '__cmd' => '[POS]',
	        '__eg' => '150',
	        '__notes' => 'The value must be between the left pane minimum width/height and the width/height of the control minus the right pane minimum width/height.<br />
			[v]Left W/H[/v] <= [p]POS[/p] <= [v]Right W/H[/v].'
		),
	);
}


function get_events_divider(&$EVENTS) {
	$EVENTS = array(
	    "help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
