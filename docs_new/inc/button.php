<?php
function get_intro_button() {
	echo "This control enables you to create a button control.";
}

function get_xdid_button(&$XDID) {
	$XDID = array(
		"c" => array(
			'__desc' => "This command lets you change the caption color for the button.",
			'__cmd' => "[+FLAGS] [COLOR]",
			'__eg' => '+nd $rgb(255,0,255)',
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "State flags",
					'__values' => array(
						'd' => "Disabled state color.",
						'h' => "Hovering state color.",
						'n' => "Normal state color.",
						's' => "Selected state color.",
					),
					'COLOR' => "Caption color.",
				),
			),
		),
		"k" => array(
			'__desc' => "This command lets you change the transparency color for the bitmap image for the button.",
			'__cmd' => "[+FLAGS] [COLOR] [FILENAME]",
			'__eg' => '+nd $rgb(255,0,255) C:/mIRC/start.bmp',
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "State flags",
					'__values' => array(
						'd' => "Disabled state transparent color/image.",
						'h' => "Hovering state transparent color/image.",
						'n' => "Normal state transparent color/image.",
						's' => "Selected state transparent color/image.",
					),
				),
				'COLOR' => "Transparency color of the [s]bitmap[/s] styled button.",
				'FILENAME' => "Bitmap filename.",
			),
			'__notes' => 'As from DCX v1.3.4, this no longer defines the caption color for the button control. This functionality was moved to [link page="button" section="xdid" flag="c"]/xdid -c[/link]',
		),
		"l" => array(
			'__desc' => "This command lets you set the icon size of the button and clear the image list. SIZE can be 16, 24 or 32. (Not used for [s]bitmap[/s] button)",
			'__cmd' => "[SIZE]",
			'__eg' => "32",
		),
		"t" => array(
			'__desc' => "This command lets you set the button caption text. (Not used for [s]bitmap[/s] button)",
			'__cmd' => "(TEXT)",
			'__eg' => "Button text",
		),
		"w" => array(
			'__desc' => "This command lets you add an icon to the image list.",
			// does NOT support ranges
	        '__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
			'__eg' => "+ng 113 shell32.dll",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Icon flags",
					'__values' => array(
						// + dcxLoadIcon flags
						'd' => "Disabled icon.",
						'h' => "Hovering icon.",
						'n' => "Normal icon.",
						's' => "Selected icon.",
					),
				),
				'INDEX' => "Icon index in icon archive",
				'FILENAME' => "Icon archive filename",
			),
			'__notes' => array(
				"The first icon added will be used for ALL button icon states, unless changed manually.",
				"Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
			),
		),
		"m" => array(
			'__desc' => "This command lets you enable text on a bitmap button.",
			'__cmd' => "[ENABLED]",
			'__eg' => "1",
			'__params' => array(
				'ENABLED' => array(
					'__desc' => "Value to enable bitmap button text or not.",
					'__values' => array(
					    '0' => 'Text is not drawn upon a bitmap button.',
					    '1' => 'Text is drawn upon a bitmap button.'
					),
				),
			),
		),
	);
	
	writeDcxLoadIcon($XDID, 'w', '+FLAGS', 1);
}


//function get_xdid_button(&$EVENTS) {
//}


function get_xdidprops_button(&$XDIDPROPS) {
	$XDIDPROPS = array(
	    "text" => array(
			'__desc' => "This property lets you retreive the button caption text.",
		),
	    "note" => array(
			'__desc' => "This property lets you retreive the button note text.",
		),
	);
}


function get_styles_button(&$STYLES) {
	$STYLES = array(
		"bitmap" => "Button is drawed using supplied bitmaps.",
		"default" => "Button has default focus.",
		'tooltips' => 'The button will have a tooltip.',
		'alpha' => 'Bitmap buttons get alpha blending.',
		'shadow' => 'Display text with a shadow.',
		'noformat' => 'Disables support for mIRC codes in control (faster).',
		'hgradient' => 'Draws a horizontal gradient with the colors specified.',
		'vgradient' => 'Draws a vertical gradient with the colors specified.',
		'__notes' => '[s]shadow[/s] will only work if the text color is not black or the same as the default text color on the current windows theme (ie. $rgb(0, 0, 0) or $dcx(GetSystemColor, COLOR_BTNTEXT) respectively). Change it by using [link page="xdid" section="xdid" flag="C"]/xdid -C[/link]',
	);
}


function get_events_button(&$EVENTS) {
	$EVENTS = array(
		"sclick" => "When you left-click on the button.",
		"lbdown" => "When the left mouse button is clicked down.",
		"lbup" => "When the left mouse button is released.",
		"rclick" => "When you right-click on the button.",
		"dclick" => "When you double-click on the button.",
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}

?>
