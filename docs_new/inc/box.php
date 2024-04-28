<?php
function get_intro_box() {
	echo "The box control is a container control like the panel. It hosts other controls inside and also has the Cell Layout Algorithm available to manage the layout of child controls automatically.<br/>Root control: custom";
}


function get_styles_box(&$STYLES) {
	$STYLES = array(
        'alpha' => 'Control is alpha blended.',
		'center' => 'Control text is centered.',
		'right' => 'Control text is right justified.',
		'bottom' => 'Control text is at the bottom of the box.',
		'none' => 'Control will not display a border or label.',
		'rounded' => 'The border drawn for the box control will be a rounded rectangle.',
		'check' => 'The box frame will have a checkbox which can enable/disable all children controls (regardless of previous state).',
		'shadow' => 'Display text with a shadow.',
		'noformat' => 'Disables support for mIRC codes in control (faster).',
                'hgradient' => 'Draws a horizontal gradient with the colors specified.',
		'vgradient' => 'Draws a vertical gradient with the colors specified.',
		'__notes' => array(
			'Applying the border [v]regular[/v]+[v]dialog frame[/v] border styles on the box will give it a titlebar. (eg. [link page="xdid" section="xdid" flag="x"]/xdid -x DNAME BOX_ID +bd[/link])',
			'[s]shadow[/s] will only work if the text color is not black or the same as the default text color on the current windows theme (ie. $rgb(0, 0, 0) or [link page="dcx" section="general" flag="GetSystemColor"]$dcx(GetSystemColor, COLOR_WINDOWTEXT)[/link] respectively). Change it by using [link page="xdid" section="xdid" flag="C"]xdid -C[/link]',
		),
	);
}


function get_xdid_box(&$XDID) {
	global $CLA;

	$XDID = array(
		"c" => array(
			'__desc' => "This command lets you add a child control to a box control.",
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
        "help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
        'lbdown' => 'When the left mouse button is clicked down.',
        'sclick' => 'When the left mouse button is clicked and released.',
        'lbup' => 'When the left mouse button is released.',
        'dclick' => 'When the left mouse button is double clicked.',
        'rclick' => 'When the right mouse button is clicked.',
        'checkchange' => array(
            '__desc' => "When the checkbox has been clicked.",
            '__cmd' => 'STATE',
            '__eg' => '1',
            '__params' => array(
                    'STATE' => 'The state which the checkbox will change to. [v]1[/v] for checked or [v]0[/v] for unchecked.',
            ),
            '__return' => '[r]nochange[/r] to cancel automatic enabling/disabling of the child controls in the box.'
        ),
	);
}

function get_ctrldcxml_box(&$CTRLDCXMLS) {
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
