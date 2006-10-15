<?php
function get_intro_image() {
	echo "Image control that lets you display BMP, GIF, JPEG, Exif, PNG, TIFF, ICON, WMF and EMF images.";
}

function get_styles_image(&$STYLES) {
	$STYLES = array(
		'tooltips' => 'The image will have a tooltip.',
	);
}

function get_xdid_image(&$XDID) {
	$XDID = array(
	    'i' => array(
	        '__desc' => 'This command lets you set the displayed image.',
	        '__cmd' => '[FILENAME]',
	        '__eg' => '$mircdir/image/test.bmp',
	        '__notes' => array(
                "Only BMP files are supported.",
                "The control stretches the image to fit its width/height.",
				"Image transparency is supported by supplying the transparent color.",
 			)
		),
		'k' => array(
	        '__desc' => 'This command lets you set the image transparent color.',
	        '__cmd' => '[COLOR]',
	        '__eg' => '$rgb(255,0,255)',
	        '__notes' => array(
	            'When using a transparent color, you may need to set the background color of the control like $rgb(face) for the dialog background color.',
			),
		),
		'w' => array(
	        '__desc' => 'This command lets you set an icon for the image.',
	        '__cmd' => '[+FLAGS] [INDEX] [SIZE] [ICON]',
	        '__eg' => '+g 2 48 C:/mIRC/shell.dll',
	        '__params' => array(
	        	'+FLAGS' => array(
					'__desc' => "Icon flags.",
					'__values' => array(
						'g' => "Convert to grayscale icon.",
					),
				),
	            'INDEX' => 'Icon index in icon archive.',
	            'SIZE' => 'Size of the icon.',
				'FILENAME' => 'Icon archive filename.',
			),
	        '__notes' => array(
				"Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
				"When the icon is loaded, the Image control is resized to [p]SIZE[/p] for width and height.",
			),
		),
		'S' => array(
	        '__desc' => 'This command lets you enable or disable automatic resizing of the image.',
	        '__cmd' => '[ENABLED]',
	        '__eg' => '1',
	        '__params' => array(
	    	    'ENABLED' => array(
					'__desc' => "Value to enable or disable resizing of image",
					'__values' => array(
	            		'0' => 'Disable resizing of image.',
	            		'1' => 'Enable resizing of image.',
					),
				),
			),
	        '__notes' => array(
				"Automatic resizing is enabled by default.",
				"If you wish your image to be resized, call [f]/xdid -S[/f] <strong>BEFORE</strong> setting the image with [f]/xdid -i[/f]",
			),
		),
	);
}


function get_events_image(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When the image is clicked.",
	    "dclick" => "When the image is double-clicked.",
	    "rclick" => "When you right-click on the image.",
	    "lbup" => "When you release the left mouse button.",
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
