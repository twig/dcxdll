<?php
function get_intro_stacker() {
	echo 'The stacker control is like a vertical container, placing controls on each other like steps on a ladder. Each control has a title, which can be collapsed to show/hide the child control.';
}


function get_styles_stacker(&$STYLES) {
	$STYLES = array(
        // FIXME: apply when it looks better 
//        'arrows' => 'Adds an arrow to the item.',
		'shadow' => 'Display text with a shadow. The shadow color depends on the color specified by the item, not the Stacker.',
		'tooltips' => 'The stacker will have a tooltip.',
		'gradient' => 'Show items with a gradient background.',
		'noformat' => 'Disables support for mIRC codes in control (faster).',
		'__notes' => '[s]shadow[/s] will only work if the text color is not black or the same as the default text color on the current windows theme (ie. $rgb(0, 0, 0) or $dcx(GetSystemColor, COLOR_WINDOWTEXT) respectively). Change it by using [f]xdid -C[/f]',
	);
}


function get_xdid_stacker(&$XDID) {
	$XDID = array(
                'a' => array(
                        '__desc' => 'This command lets you add a new item to the stacker.',
                        '__cmd' => '[N] [+FLAGS] [IMAGE] [SIMAGE] [COLOR] [BGCOLOR] Item Text [TAB] [CID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)',
                        '__eg' => '0 + 1 2 $rgb(255,0,0) $rgb(0,0,255) New Item $chr(9) 5 button 0 0 150 25 default',
                        '__params' => array(
                                'N' => 'The Nth position to insert the new item.',
                                '+FLAGS' => 'There are currently no flags.',
                                'IMAGE' => 'The image to use.',
                                'SIMAGE' => 'The selected image to use.',
                                'COLOR' => 'Foreground color for the item header.',
                                'BGCOLOR' => 'Background color for the item header.',
                                'CID' => "Unique control ID for the DCX Control. <b>Must be unique for all the controls of the dialog!</b>",
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
						'webctrl' => "Creates a WebCtrl.",
						'pager' => "Creates a Pager control.",
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
                        '__notes' => array(
				"You can use [v]0[/v] for [p]N[/p] to insert at the end of the stacker.",
				'Most of the time parameters [p]X[/p], [p]Y[/p], [p]W[/p], and [p]H[/p] are ignored.',
				'You can use [v]0[/v] or [v]-1[/v] for [p]IMAGE[/p] and [p]SIMAGE[/p] to use no image.',
			),
		),
		'c' => array(
	        '__desc' => "This command lets you select an item.",
	        '__cmd' => '[N]',
	        '__eg' => '1',
		),
		'w' => array(
			'__desc' => "This command lets you add a image file to the internal list.",
			'__cmd' => '[+FLAGS] [FILE]',
			'__eg' => '+ Start.bmp',
			'__params' => array(),
		),
		'y' => "This command lets you clear the stacker image list and updates the display.",
		'd' => array(
	        '__desc' => "This command lets you delete an item and any child control.",
	        '__cmd' => '[N]',
	        '__eg' => '1',
		),
		'r' => 'This command lets you delete all items and any child controls.',
		'T' => array(
	        '__desc' => "This command lets you set the tooltip text for item N.",
	        '__cmd' => '[N] Text',
	        '__eg' => '1 Text',
		),
        'u' => "This command lets you clear any selection.",
	);
	
	writeDcxLoadIcon($XDID, 'w', '+FLAGS', 1);
}

function get_xdidprops_stacker(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property retrieves the item text.",
		    '__cmd' => 'N',
		    '__eg' => '2',
		),
		"num" => 'This property retrieves the total number of items.',
		"sel" => 'This property retrieves the selected item ID.',
		"haschild" => array(
		    '__desc' => 'This property returns [v]$true[/v] if an item has a child control.',
		    '__cmd' => 'N',
		    '__eg' => '2',
		),
		"childid" => array(
		    '__desc' => "This property retrieves the ID of the child control on the item at the specified index.",
		    '__cmd' => 'N',
		    '__eg' => '2',
		),
	);
}

function get_events_stacker(&$EVENTS) {
	$EVENTS = array(
        	    "sclick" => array(
			'__desc' => "When an item is selected in the stacker.",
			'__cmd' => 'ITEM',
                        '__eg' => '6',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.'
			),
		),
                "dclick" => array(
			'__desc' => "When an item is double-clicked in the stacker.",
			'__cmd' => 'ITEM',
                        '__eg' => '2',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.'
			),
		),
		"rclick" => array(
			'__desc' => "When you right-click on the stacker.",
			'__cmd' => 'ITEM',
                        '__eg' => '3',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.'
			),
		),
		"lbdown" => array(
			'__desc' => 'When the left mouse button is clicked down.',
			'__cmd' => 'ITEM',
                        '__eg' => '6',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.'
			),
		),
		"lbup" => array(
			'__desc' => 'When the left mouse button is released.',
			'__cmd' => 'ITEM',
                        '__eg' => '4',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.'
			),
		),
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
