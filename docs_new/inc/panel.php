<?php
function get_intro_panel() {
	echo "The panel control is like an empty borderless dialog which is used to host other controls.<br/>Root control: custom";
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
[link page="button"]button[/link], [link page="calendar"]calendar[/link], [link page="colorcombo"]colorcombo[/link], [link page="comboex"]comboex[/link], [link page="multicombo"]multicombo[/link], [link page="datetime"]datetime[/link], [link page="directshow"]directshow[/link], [link page="ipaddress"]ipaddress[/link], [link page="listview"]listview[/link], [link page="pbar"]pbar[/link], [link page="richedit"]richedit[/link], [link page="statusbar"]statusbar[/link], [link page="toolbar"]toolbar[/link], [link page="trackbar"]trackbar[/link], [link page="treeview"]treeview[/link], [link page="updown"]updown[/link], [link page="webctrl"]webctrl[/link], [link page="web2ctrl"]web2ctrl[/link]<br />
[link page="box"]box[/link], [link page="check"]check[/link], [link page="edit"]edit[/link], [link page="image"]image[/link], [link page="line"]line[/link], [link page="link"]link[/link], [link page="list"]list[/link], [link page="radio"]radio[/link], [link page="scroll"]scroll[/link], [link page="text"]text[/link], [link page="divider"]divider[/link], [link page="pager"]pager[/link], [link page="panel"]panel[/link], [link page="rebar"]rebar[/link], [link page="stacker"]stacker[/link], [link page="tab"]tab[/link], [link page="dialog"]dialog[/link], or [link page="window"]window[/link].',
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
		    '__notes' => 'The panel text (and border) is only visible when used with <a class="value" href="xdid.htm#xdid.x">/xdid -x DIALOG ID +bd</a>',
		),
	);
}


function get_events_panel(&$EVENTS) {
	$EVENTS = array(
	    "help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
		'sclick' => 'When the left mouse button is clicked.',
		'dclick' => 'When the left mouse button is double-clicked.',
		'lbdown' => 'When the left mouse button is pressed.',
		'lbup' => 'When the left mouse button is released.',
		'lbdblclk' => 'When the left mouse button is double-clicked.',
	);
}

function get_ctrldcxml_panel(&$CTRLDCXMLS) {
	$CTRLDCXMLS = array(
	    "control" => array(
			'__desc' => "The tag which defines this control.",
			'__cmd' => 'type="box"',
			'__eg' => 'type="box" caption="this is a box"',
			'__params' => array(
				'nocla' => 'Disabled cla for child controls.',
				'caption' => 'Title text of the box.',
				'cascade' => array(
					'__desc' => 'The cla direction for the root pane',
					'__values' => array(
						'v' => 'Vertical',
						'h' => 'Horizontal',
					),
				),
				'margin' => array(
					'__desc' => 'The spacing to leave round the inside of the control.',
					'__eg' => '5 5 5 5',
				),
			),
		),
		"pane" => array(
			'__desc' => "Defines a cla pane within the control.",
			'__cmd' => '',
			'__eg' => 'cascade="v" margin="5 5 5 5"',
			'__params' => array(
				'cascade' => array(
					'__desc' => 'The cla direction for the pane',
					'__values' => array(
						'v' => 'Vertical',
						'h' => 'Horizontal',
					),
				),
				'margin' => array(
					'__desc' => 'The spacing to leave round the inside of the pane.',
					'__eg' => '5 5 5 5',
				),
				'width' => array(
					'__desc' => 'create a fixed width pane of the specified size.',
					'__eg' => '50',
				),
				'height' => array(
					'__desc' => 'create a fixed height pane of the specified size.',
					'__eg' => '50',
				),
				'weight' => array(
					'__desc' => 'The weight to give this pane when calculating the layout.',
					'__eg' => '50',
				),
			),
		),

	);
}

?>
