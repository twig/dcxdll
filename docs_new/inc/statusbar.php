<?php
function get_intro_statusbar() {
	echo "This control enables you to create a status bar.";
}


function get_styles_statusbar(&$STYLES) {
	$STYLES = array(
		"grip" => "Status bar displays a sizing grip.",
		"tooltips" => "Status bar has tooltips (tooltips only work for cells that have no text or cells where the text does not fit in the cell width).",
		"top" => " The statusbar is auto-positioned at top of dialog window.",
		'noresize' => "Status isn't auto-resized when dialog's size changes",
		'noparentalign' => "Status isn't auto-positioned when dialog's size changes.",
	);
}


function get_xdid_statusbar(&$XDID) {
	$XDID = array(
	    'k' => array(
	        '__desc' => 'This command lets you set the status bar background color.',
	        '__cmd' => '[RGB]',
	        '__eg' => '$rgb(255,0,0)',
	        '__notes' => "You can use [v]-1[/v] for [p]RGB[/p] to revert to default color.",
		),
		'l' => array(
	        '__desc' => "This command lets you set the status bar cell divisions. Every value givin is the position of the cell's right border. For example, if you have a 600 pixel width statusbar, you could use the values 200 400 -1 to get 3 200 pixel wide cells. If a cell has the value -1 for right border position, the cell will extend to the right edge of the statusbar control.",
	        '__cmd' => '[POS] (POS POS ...)',
	        '__eg' => '200 400 -1'
		),
		't' => array(
	        '__desc' => 'This command lets you set the icon, text and tooltip of a statusbar cell.',
	        '__cmd' => '[N] [+FLAGS] [#ICON] (Cell Text) $chr(9) (Tooltip)',
	        '__eg' => '2 +p 0 Cell Text $chr(9) Cell Text',
	        '__params' => array(
	            'N' => 'Status bar cell number to modify.',
	            '+FLAGS' => array(
	                '__desc' => 'Status bar cell flags.',
	                '__values' => array(
						'p' => 'Popup style border around cell.',
						'n' => 'No borders around cell.',
					),
				),
				'#ICON' => 'Icon number for the statusbar cell (0 for no icon)',
			),
	        '__notes' => array(
	            "Cell text is optional.",
	            'Tooltip is only displayed if cell text is absent or cell text is too long to fit in cell.'
			),
		),
		'v' => array(
	        '__desc' => 'This command lets you change the text of a statusbar cell.',
	        '__cmd' => '[N] (TEXT)',
	        '__eg' => "2 New Text",
	        '__notes' => array(
	            "Text is optional."
			),
		),
		'w' => array(
	        '__desc' => 'This command lets you add an icon to the statusbar image list.',
	        '__cmd' => '[INDEX] [FILENAME]',
	        '__eg' => '113 C:/mIRC/shell.dll',
	        '__params' => array(
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => array(
	            "Use 0 if the file is a single icon file.",
			),
		),
		'y' => array(
	        '__desc' => 'This command lets you clear the statusbar image list.',
		),
	);
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
		"selstart" => array(
		    '__desc' => "This returns the index of the first selected character position in the control.",
		),
	);
}

function get_events_statusbar(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => array(
			'__desc' => "Single left click over a status bar cell.",
			'__cmd' => 'CELL',
			'__params' => array(
				'CELL' => 'Cell number where the event was triggered.'
			),
		),
	    "dclick" => array(
			'__desc' => "Double-click over a status bar cell.",
			'__cmd' => 'CELL',
			'__params' => array(
				'CELL' => 'Cell number where the event was triggered.'
			),
		),
		"rclick" => array(
			'__desc' => "Right-click over a status bar cell.",
			'__cmd' => 'CELL',
			'__params' => array(
				'CELL' => 'Cell number where the event was triggered.'
			),
		),
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
