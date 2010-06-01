<?php
function get_intro_list() {
	echo "Normal listbox control.";
}


function get_styles_list(&$STYLES) {
	$STYLES = array(
		"noscroll" => "Shows a disabled vertical scroll bar for the list box when the box does not contain enough items to scroll. If you do not specify this style, the scroll bar is hidden when the list box does not contain enough items.",
		"extsel" => "Allows multiple items to be selected.",
		"multi" => "Turns string selection on or off each time the user clicks or double-clicks a string in the list box. The user can select any number of strings.",
		'nointegral' => "Specifies that the size of the list box is exactly the size specified by the application when it created the list box. Normally, the system sizes a list box so that the list box does not display partial items.",
		'nosel' => "Specifies that the list box contains items that can be viewed but not selected.",
		'vsbar' => 'Specifies that the list box should display a vertical scrollbar when it overflows.'
	);
}


function get_xdid_list(&$XDID) {
	$XDID = array(
	    'a' => array(
	        '__desc' => 'This command lets you add a new item to the listbox.',
	        '__cmd' => '[N] [Item Text]',
	        '__eg' => '0 New Item',
	        '__notes' => "You can use [v]0[/v] for [p]N[/p] to insert at the end of the listbox.",
		),
		'c' => array(
	        '__desc' => "This command lets you select items in the listbox.",
	        '__cmd' => '[N(,N,...)]',
	        '__eg' => '1,2,3',
	        '__notes' => 'In a single line listbox, you only provide the line number to select.'
		),
		'd' => array(
	        '__desc' => 'This command lets you delete an item in the listbox.',
	        '__cmd' => '[N]',
	        '__eg' => '1',
		),
		'r' => array(
	        '__desc' => 'This command lets you clear the listbox contents.',
		),
		'u' => array(
	        '__desc' => 'This command lets you unselect items the listbox.',
		),
		'o' => array(
	        '__desc' => 'This command lets you overwrite an item in the listbox.',
	        '__cmd' => '[N] [Item Text]',
	        '__eg' => '1 New Text',
		),
	);
}

function get_xdidprops_list(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the Nth listbox item text.",
		    '__cmd' => 'N',
		    '__eg' => '2',
		),
		"num" => array(
		    '__desc' => 'This property lets you retreive the number of listbox items.',
		),
		"sel" => array(
		    '__desc' => 'This property lets you retreive the selected item(s).',
		    '__cmd' => 'N',
		    '__eg' => '3',
		    '__notes' => 'In a [s]multisel[/s] listbox, it returns a comma separated list of selected indexes.'
		),
	);
}

function get_events_list(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => array(
			'__desc' => "When an item is selected in the listbox.",
			'__cmd' => 'ITEM',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.'
			),
		),
	    "dclick" => array(
			'__desc' => "When an item is double-clicked in the listbox.",
			'__cmd' => 'ITEM',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.'
			),
		),
		"rclick" => array(
			'__desc' => "When you right-click on the list.",
		),
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
		'dragbegin' => array(
		    '__desc' => "Triggers when one or more files are dropped onto the control.",
		    '__cmd' => 'COUNT',
		    '__eg' => '3',
            '__params' => array(
            	'COUNT' => "Total number of files dragged into the control.",
			),
			'__return' => '[r]cancel[/r] to stop the drag drop events from occuring.',
		),
		'dragfile' => array(
			'__desc' => "The filename of the file that has",
			'__cmd' => 'FILENAME',
			'__eg' => 'C:\mIRC\blah.txt',
		),
		'dragfinish' => "Event triggered when processing of drag drop is complete.",
	);
}
?>
