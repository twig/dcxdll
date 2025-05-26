<?php
function get_intro_link() {
	echo "Regular link control with icon support.<br/>Root control: <a href='https://learn.microsoft.com/en-gb/windows/win32/controls/static-controls'>Static</a>";
}

function get_styles_link(&$STYLES) {
	$STYLES = array(
		'tooltips' => 'The link will have a tooltip.',
		'alpha' => 'Control is alpha blended.',
		'shadow' => 'Display text with a shadow.',
		'noformat' => 'Disables support for mIRC codes in control (faster).',
		'hgradient' => 'Draws a horizontal gradient with the colors specified.',
		'vgradient' => 'Draws a vertical gradient with the colors specified.',
		'nounderline' => 'Link text is not underlined.',
		'__notes' => '[s]shadow[/s] will only work if the text color is not black or the same as the default text color on the current windows theme (ie. $rgb(0, 0, 0) or $dcx(GetSystemColor, COLOR_WINDOWTEXT) respectively). Change it by using <a class="value" href="xdid.htm#xdid.big.C">xdid -C</a>',
	);
}


function get_xdid_link(&$XDID) {
	$XDID = array(
	    'l' => array(
	        '__desc' => 'This command lets you change a color in the link palette.',
	        '__cmd' => '[N] [COLOR]',
	        '__eg' => array(
				'2 $rgb(255,0,255)',
				'2 default',
			),
	        '__params' => array(
                "N" => "Palette color number 1-6 (see [cmd]/xdid -q[/cmd] for color order).",
 			),
            '__notes' => array(
				'You can use the word default instead of a colour value to restore the colour to its default.',
				'A colour value of -1 will result in that colour not being changed.',
			),
		),
		'q' => array(
            '__desc' => 'This command lets you load a custom palette into the link control.',
            '__cmd' => '[NORMAL] [HOVER] [VISITED] [DISABLED] [PRESSED] [HOT]',
            '__eg' => array(
				'$rgb(255,0,0) $rgb(0,0,0) $rgb(255, 0, 255) $rgb(150, 150, 150) $rgb(0, 0, 0) $rgb(0,102,204)',
				'$rgb(255,0,0) $rgb(0,0,0)',
				'default -1 default default -1 default',
            ),
            '__notes' => array(
				'You can give the command a number of colors inferior to 6, it will then only add thoses colors in order to the palette starting at 1.',
				'You can use the word default instead of a colour value to restore the colour to its default.',
				'A colour value of -1 will result in that colour not being changed.',
			),
		),
		't' => array(
	        '__desc' => 'This command lets you set the link text.',
	        '__cmd' => '(Text)',
	        '__eg' => 'Text Label'
		),
		'w' => array(
	        '__desc' => 'This command lets you set an icon for the link control.',
	        '__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
	        '__eg' => '+g 113 shell32.dll',
	        '__params' => array(
	        	// +FLAGS
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => array(
				"Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
				"This is always a small 16x16 icon.",
			),
		),
	);
	
	writeDcxLoadIcon($XDID, 'w', '+FLAGS', 1);
}

function get_xdidprops_link(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the text.",
		),
	);
}

function get_events_link(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When the text is clicked.",
	    "dclick" => "When the text is double-clicked.",
	    "rclick" => "When you right-click on the text.",
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
	);
}
?>
