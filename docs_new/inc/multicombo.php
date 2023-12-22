<?php
function get_intro_multicombo() {
	echo "This control enables you to create a multicombo control, which is a custom combo box that supports user defined dropdowns.";
}


function get_styles_multicombo(&$STYLES) {
	$STYLES = array(
		"listbox" => "Combo that is a list with an edit box",
		"colourgrid" => "dropdown colour grid.",
		
		'__notes' => array(
			"When creating a MultiCombo control, it is important to remember that the size of the dropdown portion is set by a measureitem callback.",
			"When no style is supplied then the dropdown portion is assumed to be supplied by you via the /xdid -C command.",
			"NB: Styles can't be combined.",
		),
	);
}


function get_xdid_multicombo(&$XDID) {
	$XDID = array(
	    'a' => array(
	        '__desc' => 'This command lets you add an item to the listbox/colourgrid portion of the control.',
	        '__cmd' => '[#RGB] (TEXT)',
	        '__eg' => '$rgb(255,0,0) red',
            '__params' => array(
                '#RGB' => "The RGB colour to display in colourgrid mode.",
                'TEXT' => 'the text to display for this item.',
			),
			'__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		'c' => array(
	        '__desc' => 'This command lets you select the Nth item.',
	        '__cmd' => '[N]',
	        '__eg' => '5',
			'__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		'C' => array(
	        '__desc' => 'This command lets you add a custom control as a dropdown.',
	        '__cmd' => '[ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)',
	        '__eg' => '200 listview 0 0 100 100 report showgrid',
			'__notes' => array(
				"This removes any pre-existing drop controls.",
			),
		),
		'd' => array(
	        '__desc' => 'This command lets you delete the Nth item.',
	        '__cmd' => '[N]',
	        '__eg' => "6",
			'__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		'D' => array(
	        '__desc' => 'This command lets you delete the any pre-existing drop controls.',
			'__notes' => array(
				"This also deletes the internally created listbox or colourgrid controls.",
			),
		),
		's' => array(
	        '__desc' => 'This command shows or hides the dropdown.',
	        '__cmd' => '[1 or 0]',
	        '__eg' => "1",
		),
		't' => array(
	        '__desc' => 'This command sets the text shown in the editbox portion of the control.',
	        '__cmd' => '(TEXT)',
	        '__eg' => "woohoo",
			'__notes' => array(
				"Supplying an empty text clears the editbox.",
			),
		),
		'm' => array(
	        '__desc' => "This command loads the colourgrid with mIRC's current colours.",
			'__notes' => array(
				"This only affects the control when in colourgrid mode.",
			),
		),
		'o' => array(
	        '__desc' => 'This command lets you overwrite an item.',
	        '__cmd' => '[N] [RGB] (TEXT)',
	        '__eg' => '2 $rgb(0,255,0) green',
			'__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		'i' => array(
	        '__desc' => 'This command lets you insert an item.',
	        '__cmd' => '[N] [RGB] (TEXT)',
	        '__eg' => '2 $rgb(0,255,0) green',
			'__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		'u' => array(
	        '__desc' => 'This command makes the currently selected item unselected.',
			'__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		'r' => array(
			'__desc' => 'This command lets you clear all the items.',
			'__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
	);
}

function get_xdidprops_multicombo(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the Nth items text.",
	        '__cmd' => '[N]',
	        '__eg' => '1',
	        '__notes' => array(
	            'You can use a value of [v]0[/v] for [p]N[/p] to retrieve the text of the editbox.',
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		"colour" => array(
		    '__desc' => "This property lets you retreive the Nth items colour.",
	        '__cmd' => '[N]',
	        '__eg' => '1',
	        '__notes' => array(
	            'You can use a value of [v]0[/v] for [p]N[/p] to retrieve the colour of the editbox.',
				"This only affects the control when in colourgrid mode.",
			),
		),
		"sel" => array(
		    '__desc' => 'This property lets you retreive the selected item number.',
	        '__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		"seltext" => array(
		    '__desc' => 'This property lets you retreive selected item text.',
	        '__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		"num" => array(
		    '__desc' => "This property lets you retreive the total number of items.",
	        '__notes' => array(
				"This only affects the control when in listbox or colourgrid mode.",
			),
		),
		"style" => array(
		    '__desc' => "This property lets you retreive the controls current style.",
	        '__return' => array(
				'0' => 'Custom',
				'1' => 'Colour grid',
				'2' => 'Listbox',
			),
		),
		"dropstate" => array(
		    '__desc' => "This property lets you retreive the current drop state.",
	        '__return' => array(
				'-1' => "- error.",
				'0' => "- not dropped.",
				'1' => "- dropped.",
			),
		),
	);
}

function get_events_multicombo(&$EVENTS) {
	$EVENTS = array(
		"sclick" => array(
			'__desc' => "When an item is selected in the listbox/colourgrid.",
			'__cmd' => 'ITEM',
			'__eg' => '6',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
			),
		),
		"dclick" => array(
			'__desc' => "When an item is double-clicked in the listbox/colourgrid.",
			'__cmd' => 'ITEM',
			'__eg' => '6',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
			),
		),
		"measureitem" => array(
			'__desc' => "When the size of the dropdown needs renewed.",
			'__return' => array(
				'WIDTH HEIGHT' => 'The desired width and height of the dropdown.',
			),
		),
	    "edit" => "When the text is edited in the editbox.",
	    "return" => "When the <b>Enter</b> key is pressed in the editbox.",
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
	);
}
?>
