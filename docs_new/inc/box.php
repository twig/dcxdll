<?php
function get_intro_box() {
	echo "The box control is a container control like the panel. It hosts other controls inside and also has the Cell Layout Algorithm available to manage the layout of child controls automatically.";
}


function get_styles_box(&$STYLES) {
	$STYLES = array(
        'alpha' => 'Control is alpha blended.',
		'center' => 'Control text is centered.',
		'right' => 'Control text is right justified.',
		'bottom' => 'Control text is at the bottom of the box.',
		'none' => 'Control will not display a border or label.',
		'rounded' => 'The border drawn for the box control will be a rounded rectangle.',
//		'check' => 'The box frame will have a checkbox which can enable/disable all children controls (regardless of previous state).',
		'shadow' => 'Display text with a shadow.',
		'noformat' => 'Disables support for mIRC codes in control (faster).',
                'hgradient' => 'Draws a horizontal gradient with the colors specified.',
		'vgradient' => 'Draws a vertical gradient with the colors specified.',
		'__notes' => array(
			'Applying the border [s]regular[/s]+[s]dialog frame[/s] border styles on the box will give it a titlebar. (/xdid -x DNAME BOX_ID +bd)',
			'[s]shadow[/s] will only work if the text color is not black or the same as the default text color on the current windows theme (ie. $rgb(0, 0, 0) or $dcx(GetSystemColor, COLOR_WINDOWTEXT) respectively). Change it by using [f]xdid -C[/f]',
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
	    'lbdown' => 'When the left mouse button is clicked down.',
	    'sclick' => 'When the left mouse button is clicked and released.',
	    'lbup' => 'When the left mouse button is released.',
	    'dclick' => 'When the left mouse button is double clicked.',
	    'rclick' => 'When the right mouse button is clicked.',
//                'checkchange' => array(
//			'__desc' => "When the checkbox has been clicked.",
//			'__cmd' => 'STATE',
//			'__eg' => '1',
//			'__params' => array(
//				'STATE' => 'The state of the checkbox. [v]1[/v] for checked or [v]0[/v] for unchecked.',
//			),
//                        '__return' => '[r]nochange[/r] to cancel automatic enabling/disabling of the child controls in the box.'
//		),
	);
}
?>
