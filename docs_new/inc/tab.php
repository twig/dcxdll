<?php
function get_intro_tab() {
	echo "The tab control is a collection of controls which allows the user to select a visible page at a time.";
}


function get_styles_tab(&$STYLES) {
	$STYLES = array(
		'alpha' => 'The control is alpha blended.',
		'closable' => 'The tabs have a close button on the right of the tab.',
	    'vertical' => 'The tabs are aligned verticaly left. (See note below).',
		'bottom' => 'The tabs are aligned at the bottom. (See note below)',
		'right' => 'The tabs are aligned verticaly right (used with vertical). (See note below).',
		'fixedwidth' => 'All tabs are the same width.',
		'buttons' => 'The tabs appear as buttons like a toolbar.',
		'flat' => 'The tab buttons appear flat. (Only with buttons)',
		'hottrack' => 'The tab item under the mouse is highlighted (may not work on some systems).',
		'multiline' => 'The tabs aline themselves on multiple rows (default is single row).',
		'rightjustify' => 'The width of each tab is increased so the tab fills the entire width of the row (Only with multi).',
		'scrollopposite' => 'Unneeded tabs scroll to the opposite side of the control when a tab is selected.',
		'flatseps' => 'The tab separators are drawn as flat (Only with buttons and flat).',
		'gradient' => 'A gradient background is applied. (Only works with [s]closable[/s])',

		'__notes' => array(
			'Vertical and bottom tabs are not supported correctly in XP Themes. The control works, but the tabs are not drawn properly. There is no easy fix for this at the moment. It is a bug in the windows theme system, not in the API.',
			'When using the [s]gradient[/s] style, the gradient color can be changed by applying the text background color with <a class="value" href="xdid.htm#xdid.big.C">xdid -C</a>',
		),
	);
}


function get_xdid_tab(&$XDID) {
	global $CLA;

	$XDID = array(
		"a" => array(
			'__desc' => "This command lets you add a child control to a tab control.",
			'__cmd' => "[N] [ICON] (TEXT) [TAB] [CID] [CONTROL] [X] [Y] [W] [H] (OPTIONS) [TAB] (TOOLTIP)",
			'__eg' => '0 1 Tab1 $chr(9) 12 richedit 10 10 400 25 multi $chr(9) Tooltip',
			'__params' => array(
			    'N' => 'Position where the tab will be inserted (Use [v]0[/v] to insert at the end).',
			    'ICON' => 'Icon index to be displayed (Use [v]0[/v] for no icon).',
			    'TEXT' => 'Tab item text.',
			    'CID' => "Unique control ID for the DCX Control. <b>Must be unique for all the controls of the dialog!</b>",
				'CONTROL' => 'The type of DCX Control to create. Values can be:<br />
[link page="listview"]listview[/link], [link page="treeview"]treeview[/link], [link page="webctrl"]webctrl[/link], [link page="web2ctrl"]web2ctrl[/link],
[link page="edit"]edit[/link], [link page="image"]image[/link], [link page="list"]list[/link], [link page="divider"]divider[/link], [link page="panel"]panel[/link],
[link page="rebar"]rebar[/link], or [link page="tab"]tab[/link].',
				'X' => "X position of control.",
				'Y' => "Y position of control.",
				'W' => "Width of control.",
				'H' => "Height of control.",
				'OPTIONS' => "Optional styles and options available on each individual controls.",
				'TOOLTIP' => 'Not functional yet.',
			),
			'__notes' => 'The control is optional. You can use the tab control without child controls as a toolbar (like bar of buttons).',
		),
		'c' => array(
		    '__desc' => 'This command lets you select the Nth tab item.',
		    '__cmd' => '[N]',
		    '__eg' => '5',
		),
		'v' => array(
	        '__desc' => "This command allows you to move the tab item to a new position.",
	        '__cmd' => '[N] [POS]',
	        '__eg' => '1 3',
			'__params' => array(
				'N' => "The tab to move.",
				'POS' => "The new position for the item.",
			),
		),
		'd' => array(
	        '__desc' => "This command lets you delete a control added on the panel.",
	        '__cmd' => '[CID]',
	        '__eg' => '6',
	        '__notes' => 'The control associated with the tab is removed when you delete a tab item.',
		),
		'l' => array(
	        '__desc' => "This command lets you change the Nth tab item icon (Use [v]0[/v] for no icon).",
	        '__cmd' => '[N] [ICON]',
	        '__eg' => '2 3',
		),
		'm' => array(
	        '__desc' => "This command lets you change the tab item width and height (Only works with the [s]fixedwidth[/s]).",
	        '__cmd' => '[CX] [CY]',
	        '__eg' => '50 20',
		),
		'r' => 'This command lets you clear all the tab items.',
		't' => array(
	        '__desc' => "This command lets you change the Nth tab item text.",
	        '__cmd' => '[N] (Itemtext)',
	        '__eg' => '2 New Text',
		),
		'w' => array(
	        '__desc' => 'This command lets you add an icon to the image list.',
	        '__cmd' => '[+FLAGS] [N,N2,N3-N4...] [FILENAME]',
	        '__eg' => '+g 113 shell32.dll',
	        '__params' => array(
	        	// +FLAGS
	            'N' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => array(
				"Use [v]0[/v] for [p]N[/p] if the file is a single icon file.",
				"Use [v]-1[/v] for [p]N[/p] to load all icons in the file.",
			),
		),
		'y' => array(
	        '__desc' => 'This command lets you clear the tab image list or delete a specific image from the list.',
	        '__cmd' => '([+FLAGS] (args))',
	        '__eg' => '+',
	        '__params' => array(
	        	'+FLAGS' => array(
                    '__desc' => 'Delete image flags.',
                    '__values' => array(
                        'd' => 'Delete a single image.',
                    ),
				),
				'__args' => array(
					'd' => array(
						'__cmd' => '+d [N]',
						'__params' => array(
                            'N' => 'The index of the image to delete.',
						),
					),
				),
			),
			'__notes' => 'If no args are supplied then all images are deleted.',
		),
	);
	
	writeDcxLoadIcon($XDID, 'w', '+FLAGS', 1);
}


function get_xdidprops_tab(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the Nth tab item text.",
	        '__cmd' => 'N',
	        '__eg' => '1',
		),
		'sel' => 'This property lets you retreive the tab selected item number.',
		'seltext' => 'This property lets you retreive selected tab item text.',
		'num' => 'This property lets you retreive the total number of tab items.',
		"icon" => array(
		    '__desc' => "This property lets you retreive Nth tab icon.",
	        '__cmd' => 'N',
	        '__eg' => '2',
		),
		"childid" => array(
			'__desc' => 'This property lets you retreive Nth tab child control ID ($null if no child control on the tab).',
			'__cmd' => 'N',
			'__eg' => '2',
		),
		'mouseitem' => array(
			'__desc' => 'This property lets you retreive the tab item which the mouse is currently over.',
			'__notes' => 'Returns [v]-1[/v] if mouse is not over any item.',
		),
		"tabrect" => array(
		    '__desc' => "This property lets you retreive Nth tabs header rect.",
	        '__cmd' => 'N',
	        '__eg' => '2',
		),
		"tabsrect" => "This property lets you retreive the rect containing all tab headers.",
	);
}

function get_events_tab(&$EVENTS) {
	$EVENTS = array(
	        "closetab" => "When the close button on a [s]closeable[/s] tab is clicked.",
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
		'sclick' => array(
			'__desc' => 'When a tab item is selected.',
			'__cmd' => 'ITEM',
		        '__eg' => '2',
			'__params' => array(
				'ITEM' => 'Tab item index number over which the event was triggered.',
			),
		),
		'rclick' => array(
			'__desc' => 'When a rclick event is fired over the selected tab band.',
			'__cmd' => 'ITEM',
			'__eg' => '2',
		        '__params' => array(
				'ITEM' => 'Tab item index number over which the event was triggered.',
			),
		),
		'closetab' => array(
			'__desc' => 'When the close button on the tab is clicked.',
			'__cmd' => 'ITEM',
			'__eg' => '2',
			'__params' => array(
				'ITEM' => 'Tab item index number over which the event was triggered.',
			),
		),
		'sizing' => array(
			'__desc' => 'This is triggered when the tab and/or any children controls are resized.',
			'__cmd' => 'CID',
			'__eg' => '8',
			'__params' => array(
				'CID' => 'The ID of the control being resized.',
			),
		),
	);
}
?>
