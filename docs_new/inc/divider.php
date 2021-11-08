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
			'__cmd' => "[MIN] [IDEAL] [TAB] [CID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)",
			'__eg' => '100 0 $chr(9) 12 richedit 10 10 400 25 multi',
			'__params' => array(
			    'MIN' => "Minimum Pane Width/Height.",
			    'IDEAL' => 'Not used yet. Use [v]0[/v], required but ignored.',
			    'CID' => "Unique control ID for the DCX Control. <b>Must be unique for all the controls of the dialog!</b>",
				'CONTROL' => 'The type of DCX Control to create. Values can be:<br />
[v]button[/v], [v]calendar[/v], [v]colorcombo[/v], [v]comboex[/v], [v]datetime[/v], [v]directshow[/v], [v]ipaddress[/v], [v]listview[/v], [v]pbar[/v], [v]richedit[/v], [v]statusbar[/v], [v]toolbar[/v], [v]trackbar[/v], [v]treeview[/v], [v]updown[/v], [v]webctrl[/v]<br />
[v]box[/v], [v]check[/v], [v]edit[/v], [v]image[/v], [v]line[/v], [v]link[/v], [v]list[/v], [v]radio[/v], [v]scroll[/v], [v]text[/v]<br />
[v]divider[/v], [v]pager[/v], [v]panel[/v], [v]rebar[/v], [v]stacker[/v] or [v]tab[/v].',
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
		'W' => array(
	        '__desc' => "This command lets you set the width positioning bar.",
	        '__cmd' => '[WIDTH]',
	        '__eg' => '5',
	        '__notes' => 'The value is in pixels.'
		),
		'Q' => array(
	        '__desc' => "This command lets you set the colour of the positioning bar.",
	        '__cmd' => '[COLOUR] [SELECTED COLOUR]',
	        '__eg' => '$rgb(255,0,0) $rgb(0,123,32)',
		),
	);
}

function get_xdidprops_divider(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"position" => 'This property lets you retrieve the position of the divider in pixels.',
		"isvertical" => 'This property lets you retrieve the alignment of the divider.',
		"barcolours" => 'This property lets you retrieve the positioning bars colours.',
		"barwidth" => 'This property lets you retrieve the positioning bars width.',
	);
}

function get_events_divider(&$EVENTS) {
	$EVENTS = array(
	    "help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
		"dragbegin" => array(
			'__desc' => "When the user starts dragging the divier.",
			'__cmd' => 'X Y',
			'__eg' => '220 300',
		),
		"drag" => array(
			'__desc' => "When the user is dragging the divier.",
			'__cmd' => 'X Y',
			'__eg' => '220 354',
		),
		"dragfinish" => array(
			'__desc' => "When the user has finished dragging the divier.",
			'__cmd' => 'X Y',
			'__eg' => '220 403',
		),
	);
}
?>
