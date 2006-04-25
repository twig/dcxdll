<?php
function get_intro_button() {
	echo "This control enables you to create a button control.";
}

function get_xdid_button(&$XDID) {
	$XDID = array(
		"k" => array(
			'__desc' => "This command lets you change the caption color (or transparency color for image) for the button.",
			'__cmd' => "[+FLAGS] [COLOR] (FILENAME)",
			'__eg' => "+nd \$rgb(255,0,255) C:\mIRC\start.bmp",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Image flags",
					'__values' => array(
						'd' => "Disabled state color/image.",
						'h' => "Hovering state color/image.",
						'n' => "Normal state color/image.",
						's' => "Selected state color/image.",
					),
					'COLOR' => "Caption text color. ([s]Transparency color[/s] of the bitmap if the button has style [s]bitmap[/s])",
					'FILENAME' => array(
						'__desc' => "Bitmap filename. (Only required if style [s]bitmap[/s] is used)",
						'__optional' => true,
					),
				),
			),
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
			'__desc' => "This command lets you add an icon to the button image list.",
			'__cmd' => "[INDEX] [FILENAME]",
			'__eg' => "113 C:/mIRC/shell.dll",
			'__params' => array(
				'INDEX' => "Icon index in icon archive (use 0 if the file is a single icon file)",
				'FILENAME' => "Icon archive filename",
			),
			'__notes' => array(
				"The insert order is important and is as follows [e]normal[/e], [e]hover[/e], [e]selected[/e] and [e]disabled[/e].",
			),
		),
	);
}


//function get_xdid_button(&$EVENTS) {
//}


function get_xdidprops_button(&$XDIDPROPS) {
	$XDIDPROPS = array(
	    "text" => array(
			'__desc' => "This property lets you retreive the button caption text.",
		),
	);
}


function get_styles_button(&$STYLES) {
	$STYLES = array(
		"bitmap" => "Button is drawed using supplied bitmaps.",
		"default" => "Button has default focus.",
	);
}


function get_events_button(&$EVENTS) {
	$EVENTS = array(
		"sclick" => "When you left-click on the button.",
		"rclick" => "When you right-click on the button.",
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}

?>
