<?php
function get_intro_list() {
	echo "Normal listbox control.";
}


function get_styles_list(&$STYLES) {
	$STYLES = array(
        'alpha' => 'Control is alpha blended.',
		"noscroll" => "Shows a disabled vertical scroll bar for the list box when the box does not contain enough items to scroll. If you do not specify this style, the scroll bar is hidden when the list box does not contain enough items.",
		"extsel" => "Allows multiple items to be selected.",
		"multi" => "Turns string selection on or off each time the user clicks or double-clicks a string in the list box. The user can select any number of strings.",
                'multicol' => 'Allows a multi column list.',
		'noformat' => 'Disables support for mIRC codes in control (faster).',
                'nointegral' => "Specifies that the size of the list box is exactly the size specified by the application when it created the list box. Normally, the system sizes a list box so that the list box does not display partial items.",
		'nosel' => "Specifies that the list box contains items that can be viewed but not selected.",
		'vsbar' => 'Specifies that the list box should display a vertical scrollbar when it overflows.',
                'hsbar' => 'Enables the horizontal scrollbar',
		'draglist' => 'Allows items in the list to be dragged using the mouse. Cannot be used with [s]multi[/s].',
		'dragline' => 'Used with [s]draglist[/s], this replaces the standard arrow and draws a line in the list to indicate the drop position.',
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
                        '__notes' => array(
                                'In a single line listbox, you only provide the line number to select.',
                                'You can use [v]0[/v] for [p]N[/p] to select the last item.',
                        ),
		),
		'd' => array(
                        '__desc' => 'This command lets you delete an item in the listbox.',
                        '__cmd' => '[N]',
                        '__eg' => '1',
                        '__notes' => 'You can use [v]0[/v] for [p]N[/p] to delete the last item.',
		),
                'm' => array(
                        '__desc' => 'This command lets you to modify the [s]multicol[/s] properties.',
                        '__cmd' => '[+FLAG] [N](,N,N,...)',
                        '__eg' => '+w 150',
                        '__params' => array(
                                'w' => 'Sets the width of the colums. [p]N[/p] is the width of the column.',
                                't' => array(
                                        '__desc' => 'Sets the tabstops within the list.',
                                        '__values' => array(
                                                '-1' => 'Resets the tabs to the control default of [v]2[/v].',
                                                'N' => 'Sets the tabs to a fixed value for each tab.',
                                                'N1,N2,...' => 'Sets the tabs to specific values for each tab.',
                                        ),
                                ),
                        ),
                        '__notes' => 'Currently only one tabstop can be set.',
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
                        '__notes' => 'You can use [v]0[/v] for [p]N[/p] to overwrite the last item.',
		),
                'A' => array(
                        '__desc' => 'This command lets you add new items to the listbox.',
                        '__cmd' => '[N] [+FLAG] [ARGS]',
                        '__eg' => '0 +t settings',
                        '__params' => array(
                                'N' => 'The position to insert the new item(s)',
                                '+FLAGS' => array(
                                        '__desc' => "New item flags.",
                                        '__values' => array(
                                                'f' => 'Loads the entire contents of a file into the list.',
                                                'H' => 'Allows you to load an item from a hash table by name.',
                                                'n' => 'Allows you to load an item from a hash table by index.',
                                                't' => 'Loads entire hashtable into list.',
                                        ),
                                ),
                                '__args' => array(
                                        'f' => array(
                                                '__cmd' => '[START] [END] [FILENAME]',
                                                '__params' => array(
                                                    'START' => 'The starting index for items to add. Use [v]0[/v] to ignore. Negative values are treated as an offset from the end of the file.',
                                                    'END' => 'The ending index for items to add. Use [v]0[/v] to ignore. Negative values are treated as an offset from the end of the file.',
                                                ),
                                        ),
                                        'H' => '[HASHTABLE] [ITEM]',
                                        'n' => '[HASHTABLE] [INDEX]',
                                        't' => array(
                                                '__cmd' => '[HASHTABLE] [START] [END]',
                                                '__params' => array(
                                                    'START' => 'The starting index for items to add. Use [v]0[/v] to ignore. Negative values are treated as an offset from the end of the table.',
                                                    'END' => 'The ending index for items to add. Use [v]0[/v] to ignore. Negative values are treated as an offset from the end of the table.',
                                                ),
                                        ),
                                        'No flags' => '[TEXT]',
                                ),
                        ),
                        '__notes' => array(
                                'You can use [v]0[/v] for [p]N[/p] to insert at the end of the listbox.',
                                'If no [p]FLAG[/p] is specified, this behaves exactly like [f]/xdid -a[/f].',
                        ),
		),
	);
}

function get_xdidprops_list(&$XDIDPROPS) {
	$XDIDPROPS = array(
                "find" => array(
                        '__desc' => "This property lets you retreive the listview item index and column index of the Nth found listview item matching the input search patern.",
                        '__cmd' => '[TAB]Matchtext[TAB], T, N',
                        '__eg' => '$chr(9) *Item* $chr(9), W, 3',
                        '__params' => array(
                                'MATCHTEXT' => 'String composed of wildcards or regex paterns used for the search.',
                                'T' => array(
                                        '__desc' => 'Value indicating if the search is using a regex patern or wildcard string.',
                                        '__values' => array(
                                                'W' => 'Matchtext is a wildcard string.',
                                                'R' => 'Matchtext is a regex patern.',
                                        ),
                                ),
                                'N' => 'Parameter indicating to return the Nth match.'
                        ),
			'__notes' => array(
				'If [p]N[/p] is [v]0[/v], returns the total number of matching items.',
				'The [TAB] characters surrounding the matchtext are mandatory or else it wont work.',
				'The global search iterates line by line, starting at the top.',
			),
		),
		"text" => array(
		    '__desc' => "This property lets you retreive the Nth listbox item text.",
		    '__cmd' => 'N',
		    '__eg' => '2',
		),
		"num" => array(
		    '__desc' => 'This property lets you retreive the number of listbox items.',
		),
        "sel" => array(
            '__desc' => 'This property lets you retreive the indexes of selected items in the list.',
            '__cmd' => '(N)',
            '__eg' => '2',
            '__params' => array(
                'N' => 'Optional parameter allowing you to obtain the Nth selected index.',
            ),
            '__notes' => array(
                'If the list is [s]multi[/s], and [p]N[/p] is not supplied, then this command will return a comma-separated list of the selected indexes.',
                'If [p]N[/p] is [v]0[/v], then this will return the number of lines selected.',
                'When using this property without [p]N[/p], the result is limited to 900 characters. This is due to an implementation that mIRC has imposed on DLL communication, so it is advised to use the [p]N[/p] parameter to prevent inaccurate results.',
            ),
        ),
		"tbitem" => "This property lets you retreive top and bottom visible item indexes.",
	);
}

function get_events_list(&$EVENTS) {
	$EVENTS = array(
		"sclick" => array(
			'__desc' => "When an item is selected in the listbox.",
			'__cmd' => 'ITEM',
                        '__eg' => '6',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.'
			),
		),
		"dclick" => array(
			'__desc' => "When an item is double-clicked in the listbox.",
			'__cmd' => 'ITEM',
                        '__eg' => '6',
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
			'__eg' => 'C:/mIRC/blah.txt',
		),
		'dragfinish' => "Event triggered when processing of drag drop is complete.",
		'scrollend' => "Event triggered when the list has finished scrolling.",
		
		'itemdragbegin' => array(
			'__desc' => "Triggers when an item is about to be dragged.",
			'__cmd' => 'ITEM_ID',
			'__eg' => '5',
		        '__params' => array(
				'ITEM_ID' => 'The ID of the item about to be dragged.',
			),
			'__return' => '[r]nodrag[/r] to prevent the item from being dragged.',
		),
		'itemdragcancel' => array(
			'__desc' => "Triggers when user cancels drag by either pressing ESC or right clicking the mouse button.",
			'__cmd' => 'ITEM_ID',
			'__eg' => '5',
		        '__params' => array(
				'ITEM_ID' => 'The ID of the item selected for dragging.',
			),
		),
		'itemdrag' => array(
			'__desc' => "Triggers when an item is dragged.",
			'__cmd' => 'ITEM_ID ITEM_MOUSEOVER',
			'__eg' => '5 3',
		        '__params' => array(
				'ITEM_ID' => 'The ID of the item dragged.',
				'ITEM_MOUSEOVER' => 'The ID of the item the mouse is currently over.',
			),
			'__return' => array(
				'stop' => 'to display a stop cursor.',
				'copy' => 'to display a copy cursor.',
			),
			'__notes' => 'If the mouse is not currently over the control, [p]ITEM_MOUSEOVER[/p] will be [v]0[/v].',
		),
		'itemdragfinish' => array(
			'__desc' => "Triggers when an item has finished dragging.",
			'__cmd' => 'ITEM_ID ITEM_MOUSEOVER',
			'__eg' => '5 3',
		        '__params' => array(
				'ITEM_ID' => 'The ID of the item dragged.',
				'ITEM_MOUSEOVER' => 'The ID of the item the mouse is currently over.',
			),
			'__notes' => 'If the mouse is not currently over the control, [p]ITEM_MOUSEOVER[/p] will be [v]0[/v].',
		),
	);
}
?>
