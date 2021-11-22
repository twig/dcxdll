<?php
function get_intro_image() {
	echo 'Image control that lets you display BMP, GIF, JPEG, Exif, PNG, TIFF, ICON, WMF and EMF images. However, if GDI+ is not loaded, DCX can only support native BMP format files. See [f]$dcx(IsUsingGDI)[/f].';
}

function get_styles_image(&$STYLES) {
	$STYLES = array(
        'alpha' => 'Control is alpha blended.',
		'tooltips' => 'The image will have a tooltip.',
        'hgradient' => 'Draws a horizontal gradient with the colors specified.',
		'vgradient' => 'Draws a vertical gradient with the colors specified.',
		'vcenter' => 'Draws the image centered vertically in the control. (only affects images loaded with [s]+k[/s])',
		'hcenter' => 'Draws the image centered horizontally in the control. (only affects images loaded with [s]+k[/s])',
	);
}

function get_xdid_image(&$XDID) {
	$XDID = array(
	    'i' => array(
	        '__desc' => 'This command lets you set the displayed image.',
	        '__cmd' => '[+FLAGS] [FILENAME]',
	        '__eg' => '+ $mircdir/image/test.bmp',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Load image flags.",
                    '__values' => array(
                        'a' => "Anti-aliased mode.",
                        'b' => "Enable support for PNG per-pixel-alpha channels.",
                        'h' => "High quality mode (Slower).",
                        't' => "Tiled image.",
                        'A' => "No animation even if the format supports it.",
                        'k' => "Keep images aspect ratio.",
                    ),
                ),
                'FILENAME' => 'Image filename.',
            ),
	        '__notes' => array(
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
	        '__eg' => '+g 2 48 shell32.dll',
	        '__params' => array(
	        	// +FLAGS
	            'INDEX' => 'Icon index in icon archive.',
	            'SIZE' => 'Size of the icon.',
				'FILENAME' => 'Icon archive filename.',
			),
	        '__notes' => array(
				"Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
				"When the icon is loaded, the Image control is resized to [p]SIZE[/p] for width and height.",
			),
		),
		'o' => array(
			'__desc' => 'This command lets you offset an image in the control.',
			'__cmd' => '[XOFFSET] [YOFFSET]',
			'__eg' => '20 20',
			'__params' => array(
				'XOFFSET' => 'The horizontal offset for the image.',
				'YOFFSET' => 'The vertical offset for the image.',
			),
		'__notes' => 'Negative offsets are valid.',
		),
		'S' => array(
			'__desc' => 'This command lets you enable or disable automatic resizing of the image.',
			'__cmd' => '[1|0]',
			'__eg' => '1',
			'__notes' => "Automatic resizing is enabled by default.",
		),
		'F' => array(
			'__desc' => 'This command lets you enable or disable automatic resizing of the image.',
			'__cmd' => '[+FLAG] [ARGS]',
			'__eg' => '+a 1',
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Animation flags.',
					'__values' => array(
						'a' => 'Start/Pause an animation.<br/>[ARGS] is [1|0].',
						'd' => 'Set the delay between frames.<br/>[ARGS] is [DELAY].<br/>A delay of zero causes the images built in delay to be used.',
						'f' => 'Set the current frame.<br/>[ARGS] is [FRAME].<br/>[FRAME] must be >=0 & <= $xdid(dname,id).frames',
					),
				),
				'ARGS' => 'This is dependent on which flag is used.',
			),
			'__notes' => ".",
		),
	);
	
	writeDcxLoadIcon($XDID, 'w', '+FLAGS', 1);
}

function get_xdidprops_image(&$XDIDPROPS) {
	$XDIDPROPS = array(
        'fname' => 'This property returns loaded filename (if any).',
        'frames' => 'This property returns the number of frames in an animation (if any).',
        'isanimated' => 'This property returns if a loaded image is an animation.',
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
