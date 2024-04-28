<?php
function get_intro_statusbar() {
	echo "This control enables you to create a status bar.<br/>Root control: <a href='https://learn.microsoft.com/en-gb/windows/win32/controls/status-bar-reference'>StatusBar</a>";
}


function get_styles_statusbar(&$STYLES) {
	$STYLES = array(
		"grip" => "Status bar displays a sizing grip.",
		"tooltips" => "Status bar has tooltips (tooltips only work for cells that have no text or cells where the text does not fit in the cell width).",
		"top" => "The statusbar is auto-positioned at top of dialog window.",
		'noresize' => "Status isn't auto-resized when dialog's size changes",
		'noparentalign' => "Status isn't auto-positioned when dialog's size changes.",
                'noauto' => "Prevents the statusbar from being auto-positioned and resized by the parent window. The user will be responsible for the positioning and resizing of the statusbar.",
	);
}


function get_xdid_statusbar(&$XDID) {
	$XDID = array(
        'k' => array(
            '__desc' => 'This command lets you set the status bar background color.',
            '__cmd' => '[RGB]',
            '__eg' => '$rgb(255,0,0)',
            '__notes' => array(
                "You can use [v]-1[/v] for [p]RGB[/p] to revert to default color.",
                'This command will only work if the computer has no visual styles applied (XP themes). To use this on machines with visual styles enabled, apply the [s]notheme[/s] style.',
            ),
		),
		'l' => array(
            '__desc' => 'This command lets you set the width of status bar cells. A value of [v]-1[/v] will cause the cell to automatically stretch, filling the remainding space. Values can also be a percentage, such as [v]20%[/v].',
            '__cmd' => '[POS] (POS POS ...)',
            '__eg' => '10% 200 400 -1'
		),
		't' => array(
            '__desc' => 'This command lets you set the icon, text and tooltip of a statusbar cell. Child controls could also be created in place of a statusbar cell.',
            '__cmd' => '[N] [+FLAGS] [#ICON] (Cell Text) $chr(9) (Tooltip)',
            '__eg' => '2 +p 0 Cell Text $chr(9) Tooltip Text',
            '__params' => array(
                'N' => 'Status bar cell number to modify.',
                '+FLAGS' => array(
                    '__desc' => 'Status bar cell flags.',
                    '__values' => array(
                        'c' => 'Allows you to add controls to the statusbar (requires different syntax). Use [p]H[/p] of [v][/v] for default size.',
                        'f' => 'Supports mIRC formatted text.',
                        'p' => 'Popup style border around cell.',
                        'n' => 'No borders around cell.',
                    ),
                ),
                '#ICON' => 'Icon number for the statusbar cell (0 for no icon)',
                '__args' => array(
                    'c' => array(
						'__cmd' => '[N] [+c] [#ICON] [CID] [CONTROL] [X] [Y] [W] [H] (STYLES)',
						'__params' => array(
							'CONTROL' => 'The type of DCX Control to create. Values can be:<br />
[link page="button"]button[/link], [link page="calendar"]calendar[/link], [link page="comboex"]comboex[/link], [link page="multicombo"]multicombo[/link],
[link page="datetime"]datetime[/link], [link page="directshow"]directshow[/link], [link page="ipaddress"]ipaddress[/link], [link page="listview"]listview[/link],
[link page="pbar"]pbar[/link], [link page="richedit"]richedit[/link], [link page="statusbar"]statusbar[/link], [link page="toolbar"]toolbar[/link],
[link page="trackbar"]trackbar[/link], [link page="treeview"]treeview[/link], [link page="updown"]updown[/link], [link page="webctrl"]webctrl[/link],
[link page="web2ctrl"]web2ctrl[/link]<br />
[link page="box"]box[/link], [link page="check"]check[/link], [link page="edit"]edit[/link], [link page="image"]image[/link], [link page="line"]line[/link],
[link page="link"]link[/link], [link page="list"]list[/link], [link page="radio"]radio[/link], [link page="scroll"]scroll[/link], [link page="text"]text[/link],
[link page="divider"]divider[/link], [link page="pager"]pager[/link], [link page="panel"]panel[/link], [link page="rebar"]rebar[/link], [link page="stacker"]stacker[/link], or
[link page="tab"]tab[/link].',
                            'X' => 'The X-offset of the control is ignored but required.',
                            'Y' => 'The Y-offset of the control is ignored but required.',
							'W' => 'The width of the control is ignored but required.',
                            'H' => 'The height of the control. The cell height is used if a value of [v]0[/v] is supplied.',
						),
					),
				),
            ),
            '__notes' => 'Tooltip is only displayed if cell text is absent or cell text is too long to fit in cell.',
		),
		'v' => array(
	        '__desc' => 'This command lets you change the text of a statusbar cell.',
	        '__cmd' => '[N] (TEXT)',
	        '__eg' => "2 New Text",
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
		'y' => 'This command lets you clear the statusbar image list.',
	);
	
	writeDcxLoadIcon($XDID, 'w', '+FLAGS', 1);
}

function get_xdidprops_statusbar(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the Nth status bar cell text.",
		    '__cmd' => 'N',
		    '__eg' => '2',
		),
		"parts" => array(
		    '__desc' => 'This property lets you retreive the status bar cell division parts.',
		),
		"tooltip" => array(
		    '__desc' => 'This property lets you retreive the status bar tooltip of the Nth cell.',
		    '__cmd' => 'N',
		    '__eg' => '3',
		),
	);
}

function get_events_statusbar(&$EVENTS) {
	$EVENTS = array(
        "sclick" => array(
			'__desc' => "Single left click over a status bar cell.",
			'__cmd' => 'CELL',
            '__eg' => '3',
			'__params' => array(
				'CELL' => 'Cell number where the event was triggered.'
			),
		),
        "dclick" => array(
			'__desc' => "Double-click over a status bar cell.",
			'__cmd' => 'CELL',
            '__eg' => '2',
			'__params' => array(
				'CELL' => 'Cell number where the event was triggered.'
			),
		),
		"rclick" => array(
			'__desc' => "Right-click over a status bar cell.",
			'__cmd' => 'CELL',
            '__eg' => '5',
			'__params' => array(
				'CELL' => 'Cell number where the event was triggered.'
			),
		),
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
	);
}
?>
