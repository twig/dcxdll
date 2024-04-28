<?php
function get_intro_check() {
	echo "Checkbox control.<br/>Root control: <a href='https://learn.microsoft.com/en-gb/windows/win32/controls/buttons'>Button</a>";
}


function get_styles_check(&$STYLES) {
	$STYLES = array(
		"right" => 'Check control box is at the right of text.',
		'center' => 'Control text is centered.',
		'vcenter' => 'Control text is vertically centered.',
		'rjustify' => 'Control text is right justified.',
		'pushlike' => 'Makes the check act and look like a push button.',
		'3state' => 'Creates a 3 state checkbox.',
		'tooltips' => 'The checkbox will have a tooltip.',
		'alpha' => 'Control is alpha blended.',
		'hgradient' => 'Draws a horizontal gradient with the colors specified.',
		'vgradient' => 'Draws a vertical gradient with the colors specified.',
		'custom' => 'Control is custom drawn.',
	);
}


function get_xdid_check(&$XDID) {
	$XDID = array(
	    'c' => array(
	        '__desc' => 'This command lets you check the checkbox.',
	        '__notes' => array(
                "You can set the indeterminate state when using a [s]3state[/s] check by using the <f>-cu</f> switches.",
 			)
		),
		't' => array(
	        '__desc' => 'This command lets you set the check text.',
	        '__cmd' => '(Text)',
	        '__eg' => 'Check Text',
		),
		'u' => array(
	        '__desc' => 'This command lets you uncheck the check control.',
		),
		"C" => array(
			'__desc' => "This command is an overload of the general [f]-C[/f] command for colours.",
			'__cmd' => "[+FLAGS] [COLOR]",
			'__eg' => '+hc $rgb(0,0,255)',
			'__params' => array(
				'+FLAGS' => array(
                    '__desc' => "Color Flags.",
                    '__values' => array(
                            'b' => "Background color.",
                            'g' => 'Sets the start gradient color.',
                            'G' => 'Sets the end gradient color.',
                            'k' => "Text background Color.",
                            'r' => "Border color.",
                            't' => "Text color.",
                            'c' => "Checkbox background color.",
                            'C' => "Checkbox Tick color.",
                            'f' => "Checkbox Frame color.",
                            'h' => "Hot color.<br/>Use in conjunction with other flags",
                            'd' => "Disabled color.<br/>Use in conjunction with other flags",
                    ),
                ),
            ),
			'__notes' => 'The h & d flags only work when combined with the checkbox specific flags.',
		),
	);
}


function get_xdidprops_check(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the check text.",
		),
		"state" => array(
		    '__desc' => "This property lets you retreive check selection state.",
		),
	);
}


function get_events_check(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => "When the text is clicked.",
	    "rclick" => "When you right-click on the text.",
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
	);
}
?>
