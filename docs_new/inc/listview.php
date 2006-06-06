<?php
function get_intro_listview() {
	echo "This control enables you to create a listview in 4 different view styles: icon, small icon, list and report.";
}


function get_styles_listview(&$STYLES) {
	$STYLES = array(
		' icon' => 'Creates a listview in icon view.',
		' list' => 'Creates a listview in item list view.',
		' report' => 'Creates a listview in report view (the view with columns and grid).',
		' smallicon' => 'Creates a listview in smallicon view.<br /><br />',

		'singlesel' => 'Allows only one item at a time to be selected. By default, multiple items can be selected.',
		'flatsb' => 'Enables flat scroll bars on the listview.',
		'borderselect' => 'When an item is selected a special border is drawn arround it instead of it being highlighted.',
		'grid' => 'Displays grid lines between item cells. ([s]report[/s] view)',
		'fullrow' => 'Highlights the full row when an item is selected. ([s]report[/s] view)',
		'noheader' => 'Does not display the header on the listview. ([s]report[/s] view)',
		'alignleft' => 'Items are left-aligned in the listview control. ([s]icon[/s] & [s]smallicon[/s] views)',
		'aligntop' => 'Items are aligned with the top of the listview control. ([s]icon[/s] & [s]smallicon[/s] views)',
		'autoarrange' => 'Items are rearranged automaticly when inserted. ([s]icon[/s] & [s]smallicon[/s] views)',
		'showsel' => 'When the control loses focus, the selection is still displayed.',
		'nolabelwrap' => 'The text is displayed on a single line. ([s]icon[/s] & [s]smallicon[/s] views)',
		'checkbox' => 'Enables checkbox support in the list view. ([s]report[/s] view)',
		'editlabel' => 'Enables the possibility to edit listview item labels.',
		'headerdrag' => 'Enables the possibility to drag listview headers to another position. ([s]report[/s] view)',
		'sortasc' => 'Items are sorted based on item text in ascending order.',
		'sortdesc' => 'Items are sorted based on item text in descending order.',
		'subitemimage' => 'Subitems have images. ([s]report[/s] view)',
		'noscroll' => 'Scrolling is disabled. All items must be within the client area. ([s]icon[/s] & [s]smallicon[/s] views)',
		'hottrack' => 'Enables hot-track selection in a listview control. Hot track selection means that an item is automatically selected when the cursor remains over the item for a certain period of time.',
		'tooltip' => 'If a partially hidden label in any view lacks tooltip text, the control will unfold the label. If this style is not set, the control will unfold partly hidden labels only for the [s]icon[/s] view.',
		
		'__notes' => array(
		     'Gridlines dont work with groups. See <a>/xdid -q</a>.',
		     '[s]sortasc[/s] and [s]sortdesc[/s] only work with [s]report[/s] or [s]list[/s] views.'
		),
	);
}


function get_xdid_listview(&$XDID) {
	$XDID = array(
	    'a' => array(
	        '__desc' => 'This command lets you add an item to the listview.',
	        '__cmd' => '[N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] (Item Text) [TAB] [+FLAGS] [#ICON] (Item Text) [TAB] ...',
	        '__eg' => '1 0 +cbku 1 0 0 0 -1 $rgb(170,213,255) text $chr(9) + 0 text2 $chr(9) + 0 text3',
            '__params' => array(
                'N' => 'Position where the listview item is inserted. (Use [v]0[/v] to insert at the end)',
				'INDENT' => 'Space in icon width at the beginning of a line in the listview ([v]0[/v] for no space)',
                '+FLAGS' => array(
                    '__desc' => 'Item flags.',
                    '__values' => array(
						'b' => 'The item is bold.',
						'c' => 'The item has a text color defined by the [p]COLOR[/p] parameter (use [v]-1[/v] for no color).',
						'd' => 'The item appears selected like a drop target.',
						'f' => 'The item has focus thus having a dotted box around it.',
						'k' => 'The item has a background color defined by the [p]BGCOLOR[/p] parameter (use [v]-1[/v] for no color).',
						's' => 'The item is selected.',
						'p' => 'The item is a DCX ProgressBar control.',
						't' => 'The item icon appears as 50% opaque, looks like it has a "ghost" effect.',
						'u' => 'The item is underlined.',
					),
				),
                '#ICON' => "Icon index number from the normal icon list.",
				'STATE' => array(
                    '__desc' => 'Icon index number from the state icon list.',
					'__values' => array(
						'0' => 'No check.',
						'1' => 'Unchecked.',
						'2' => 'Checked',
					),
				),
                '#OVERLAY' => 'Icon index number from the normal icon list that was marked as an overlay icon. ([n]Not functional yet[/n])',
                '#GROUPID' => 'Group ID. (Use [v]0[/v] for no value) [o]XP+[/o]',
				'COLOR' => 'Item text color.',
				'BGCOLOR' => 'Item background color.',
				'Item Text' => 'The text to display in the cell. If used with [v]+p[/v] [p]+FLAGS[/p], then this is the DCX ProgressBar [s]style[/s].',
			),
			'__notes' => array(
			    'Icons must be added prior to be used in the listview.',
			    'The [p]INDENT[/p] parameter only works if you are using normal icons in the listview.',
			    'If you add icons to the icon lists and use 0 for no icon, there will be the an icon indent space in front of the item text. (normal and state lists)',
				"You can use [v]0[/v] for the [p]#ICON[/p], and [p]#OVERLAY[/p] values if you wish to use no icon.",
				"You need to use [s]checkbox[/s] style in order to use checkboxes in the listview.",
			),
		),
		'B' => array(
	        '__desc' => 'This command lets you force label editing on a listview item.',
	        '__cmd' => '[N]',
	        '__eg' => '3',
	        '__notes' => 'You must give the listview focus before forcing the item editing mode.'
		),
		'c' => array(
	        '__desc' => 'This command lets you select listview items.',
	        '__cmd' => '[N(,N,...)]',
	        '__eg' => '1,3,5',
	        '__notes' => 'In a [s]singlesel[/s] listview, there is only one value for [p]N[/p].'
		),
		'd' => array(
	        '__desc' => "This command lets you delete the Nth listview item.",
	        '__cmd' => '[N]',
	        '__eg' => '6',
		),
		'g' => array(
	        '__desc' => 'This command lets you set a background image to the listview control.',
	        '__cmd' => '[+FLAGS] [X] [Y] (FILENAME)',
	        '__eg' => '+n 50 50 $mircdir/images/blah.png',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Image flags.",
                    '__values' => array(
						'n' => 'Normal image.',
						'r' => 'Remove image.',
						't' => 'Tiled Image.',
					),
				),
                'X' => 'Normal horizontal offset.',
                'Y' => 'Normal vertical offset.',
                'FILENAME' => 'Filename of the image including the path.',
			),
	        '__notes' => 'For [p]X[/p] and [p]Y[/p], 0 = left aligned, 50 = centred, 100 = right aligned.'
		),
		'i' => array(
	        '__desc' => 'This command lets you change the listview control colors.',
	        '__cmd' => '[+FLAGS] [COLOR]',
	        '__eg' => '+bk none',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Color flags.",
                    '__values' => array(
						'b' => 'Background color.',
                        'k' => 'Text background color.',
						'o' => 'Outline color. (see [s]borderselect[/s] style)',
						't' => 'Text color.',
					),
				),
                'COLOR' => 'Color in $rgb() format.',
			),
	        '__notes' => array(
				'[p]COLOR[/p] can be [v]none[/v] for no colors, for both background and text background flags.',
				'If [p]COLOR[/p] is [v]none[/v] for text background, the text background parameter of <a>/xdid -a</a> wont work anymore.',
			),
		),
		'j' => array(
	        '__desc' => 'This command lets you change the appearance of a listview item.',
	        '__cmd' => '[N] [NSUB] [+FLAGS]',
	        '__eg' => '2 5 +bt',
            '__params' => array(
				'N' => 'The row index for the item.',
				'NSUB' => 'The column index for the item.',
                'STATE' => array(
                    '__desc' => "Item flags",
                    '__values' => array(
						'b' => 'The item is bold.',
						'd' => 'The item appears selected like a drop target.',
						'f' => 'The item has focus thus having a dotted box around it.',
						's' => 'The item is selected.',
						't' => 'The item appears as 50% opaque, looks like it has a "ghost" effect.',
						'u' => 'The item is underlined.',
					),
				),
			),
		),
		'k' => array(
	        '__desc' => 'This command lets you change the check state on the Nth listview item.',
	        '__cmd' => '[STATE] [N]',
	        '__eg' => '2 5',
            '__params' => array(
                'STATE' => array(
                    '__desc' => "Check State",
                    '__values' => array(
						'0' => 'No check.',
						'1' => 'Unchecked.',
						'2' => 'Checked',
					),
				),
                'COLOR' => 'Color in $rgb() format.',
			),
	        '__notes' => "If you are not using the [s]checkbox[/s] style, this function can also change the state icon of an item. Be sure to insert the state icons before you use them.",
		),
		'l' => array(
	        '__desc' => 'This command lets you change the listview item icon.',
	        '__cmd' => '[N] [NSUB] [ICON]',
	        '__eg' => '1 2 5',
            '__params' => array(
                'N' => 'Item index.',
                'NSUB' => 'Column index ([v]0[/v] for first column)',
                'ICON' => 'Icon index ([v]0[/v] for first column)',
			),
		),
		'm' => array(
	        '__desc' => 'This command lets you trigger the group view in the listview, which is off by default. [o]XP+[/o]',
	        '__cmd' => '[ENABLED]',
	        '__eg' => '1',
		),
		'n' => array(
	        '__desc' => 'This command lets you change the Nth listview column width ([s]report[/s] view only).',
	        '__cmd' => '[N] [+FLAGS] [WIDTH]',
	        '__eg' => '2 + 100',
			'__params' => array(
                '+FLAGS' => array(
                    '__desc' => 'Flags for column width.',
                    '__values' => array(
						'a' => 'Autosize.',
						'h'=> 'Autosize according to header text width.',
					),
				),
                'WIDTH' => 'Header width (Ignored if you use flags +a or +h).',
                'ICON' => 'Icon index ([v]0[/v] for first column)',
			),
			'__notes' => 'If the flag +h is used for the last column, the column will be resized to the width of the control.',
		),
		'q' => array(
	        '__desc' => 'This command lets you add a group to the listview. [o]XP+[/o]',
	        '__cmd' => '[N] [+FLAGS] [GROUPID] [GROUPTEXT]',
	        '__eg' => '1 +l 101 Group 101!',
	        '__params' => array(
	            'N' => 'Group index order.',
                '+FLAGS' => array(
                    '__desc' => "Group flags.",
                    '__values' => array(
						'c' => 'Group text is centered.',
						'l' => 'Group text is left aligned.',
						'r' => 'Group text is right aligned.',
					),
				),
                'GROUPID' => 'ID that identifies the group when adding items in <a>/xdid -a</a>.',
                'GROUPTEXT' => 'Label for the group.'
			),
		),
		'r' => 'This command lets you clear all the listview items.',
		't' => array(
	        '__desc' => 'This command lets you set the different column header text, width and alignment. ([s]report[/s] style only)',
	        '__cmd' => '[+FLAGS] [#ICON] [WIDTH] (TEXT) [TAB] [+FLAGS] [#ICON] [WIDTH] (TEXT) [TAB] ...',
	        '__eg' => '+l 2 130 column 1 $chr(9) +c 1 130 column 2 $chr(9) +rb 2 130 column 3',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Item flags.",
                    '__values' => array(
						'a' => 'Autosize.',
						'b' => 'Header icon appears right of text.',
						'c' => 'Header text is centered.',
						'h' => 'Autosize according to header text width.',
						'l' => 'Header text is left justified.',
						'r' => 'Header text is right justified.',
					),
				),
                'ICON' => 'Icon index number from the icon list ([v]0[/v] for no icon).',
                'WIDTH' => 'Column width in pixels.',
			),
	        '__notes' => array(
				'The leftmost column is always left aligned. If you want a leftmost column aligned other than left, create a zero width left dummy column.',
				'If you re-issue this command while the listview is populated, all the data except the first column will disappear, so you will have to refill the list.',
				'If the flag +h is used for the last column, the column will be resized to the width of the control.',
			),
		),
		'u' => 'This command makes the currently selected listview item unselected.',
		'v' => array(
	        '__desc' => 'This command lets you change the listview item text. If the target item is a DCX ProgressBar, then this allows you to send /xdid commands to the embedded ProgressBar.',
	        '__cmd' => '[N] [NSUB] (TEXT)',
	        '__eg' => '1 2 5',
	        '__params' => array(
                'N' => 'Item row.',
                'NSUB' => 'Column index. ([v]0[/v] for first column)',
                'TEXT' => 'Item Text, or command to send to ProgressBar.',
			),
			'__notes' => 'Use [p]TEXT[/p] as [v]-v 80[/v] to set the value of a progress bar.'
		),
		'w' => array(
	        '__desc' => 'This command lets you add an icon to the listview image lists.',
	        '__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
	        '__eg' => '+n 113 C:/mIRC/shell.dll',
	        '__params' => array(
	            '+FLAGS' => array(
	                '__desc' => "Image list flags.",
	                '__values' => array(
						'n' => 'Normal icon list.',
						's' => 'State icon list.',
					),
				),
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => array(
	            "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
			),
		),
		'y' => array(
	        '__desc' => 'This command lets you clear the listview image lists.',
	        '__cmd' => '[+FLAGS]',
	        '__eg' => '+ns',
	        '__params' => array(
	            '+FLAGS' => array(
	                '__desc' => "Image list flags",
	                '__values' => array(
						'n' => 'Normal icon list.',
						's' => 'State icon list.',
					),
				),
			),
		),
		'z' => array(
	        '__desc' => 'This command lets you sort the data in the listview control.',
	        '__cmd' => '[+FLAGS] [COLUMN] (ALIAS)',
	        '__eg' => '+ca 2 lvsort',
	        '__params' => array(
	            '+FLAGS' => array(
	                '__desc' => "Sort flags.",
	                '__values' => array(
						'a' => 'Ascendant order sorting. ([v]Default[/v])',
						'c' => 'Custom sorting provided by [p]ALIAS[/p].',
						'd' => 'Descendant order sorting.',
						'n' => 'Numerical sorting.',
						's' => 'Case-sensitive sorting. (With Alpha-numeric)',
						't' => 'Alpha-numeric sorting. ([v]Default[/v])',
					),
				),
				'COLUMN' => 'Column to be used for sorting.',
	            'ALIAS' => 'Alias name to be called in the case of custom sorting.',
			),
	        '__notes' => array(
	            "[p]ALIAS[/p] is not needed if not using a custom sorting algorithm.",
				"When using [f]+c[/f], sorted values given by $1 and $2 into [p]ALIAS[/p].",
				'The custom alias must return the following values for the comparison of custom values. The ascendant or descendant order is managed by the DLL with the given values returned.<br />
[v]$1 < $2[/v] return [v]-1[/v]<br />
[v]$1 > $2[/v] return [v]1[/v]<br />
[v]$1 == $2[/v] return [v]0[/v]',
			),
		),
	);
}

function get_xdidprops_listview(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive a Nth listview item text.",
	        '__cmd' => 'N, NSUB',
	        '__eg' => '1, 2',
	        '__notes' => 'Use NSUB value of [v]0[/v] for the first column item text.'
		),
		"seltext" => array(
            '__desc' => 'This property lets you retreive the selected listview item text.',
            '__cmd' => 'NSUB',
            '__eg' => '0',
            '__notes' => 'Use NSUB value of [v]0[/v] for the first column item text.'
		),
		"sel" => array(
            '__desc' => 'This property lets you retreive the item number of selected item in the listview.',
            '__notes' => 'If the listview is [s]multiline[/s], then a comma-separated list of the selected lines is returned.'
		),
		"selnum" => 'This property lets you retreive the number of selected items in the listview.',
		"find" => array(
		    '__desc' => "This property lets you retreive the listview item index and column index of the Nth found listview item matching the input search patern.",
		    '__cmd' => '[TAB]Matchtext[TAB], T, M, N',
	        '__eg' => '$chr(9) *Item* $chr(9), W, 0, 3',
	        '__params' => array(
	            'MATCHTEXT' => 'String composed of wildcards or regex paterns used for the search.',
	            'T' => array(
	                '__desc' => 'Value indicating if the search is using a regex patern or wildcard string.',
	                '__values' => array(
						'W' => 'Matchtext is a wildcard string.',
						'R' => 'Matchtext is a regex patern.',
					),
				),
                'M' => "Parameter indicating the column to search.",
                'N' => 'Parameter indicating to return the Nth match.'
			),
			'__notes' => array(
			    'If [p]M[/p] is [v]0[/v], searchs through all columns.',
				'If [p]N[/p] is [v]0[/v], returns the total number of matching items.',
				'The [TAB] characters surrounding the matchtext are mandatory or else it wont work.',
				'The global search including all columns goes left to right, line by line, starting at the top.',
			)
		),
		"num" => 'This property lets you retreive the number of items of a listview.',
		"state" => array(
		    '__desc' => "This property lets you retreive the check item state of the Nth listview item.",
			'__cmd' => 'N',
	        '__eg' => '2',
	        '__notes' => 'If you are not using the [s]checkbox[/s] style, the value returned is the state icon value. Otherwise, returns [v]2[/v] (checked), [v]1[/v] (unchecked), or [v]0[/v] (no check).'
		),
		"icon" => array(
		    '__desc' => "This property lets you retreive a Nth listview item icon number.",
			'__cmd' => 'N, NSUB',
	        '__eg' => '1, 2',
	        '__notes' => array(
	             'Use [p]NSUB[/p] value of [v]0[/v] for the first column item icon.',
			),
		),
		"tbitem" => array(
		    '__desc' => "This property lets you retreive top and bottom visible item indexes.",
            '__notes' => 'Returns [v]0[/v] for no items or error.'
		),
		"mouseitem" => array(
			'__desc' => "This property lets you retreive the item index over which the mouse pointer is.",
			'__notes' => 'Returns [v]0[/v] if mouse is not over any item.',
		),
		"hwidth" => array(
		    '__desc' => "This property lets you retreive the Nth listview header width.",
			'__cmd' => 'N',
	        '__eg' => '2',
		),
		"htext" => array(
		    '__desc' => "This property lets you retreive the Nth listview header text.",
			'__cmd' => 'N',
	        '__eg' => '2',
		),
        "hicon" => array(
		    '__desc' => "This property lets you retreive the Nth listview header icon index.",
			'__cmd' => 'N',
	        '__eg' => '2',
		),
		'genabled' => 'This property lets you retreive whether groups are enabled or not.',
		'pbar' => array(
		    '__desc' => "This property lets you retreive ProgressBar properties from a specific cell.",
			'__cmd' => 'N, NSUB, PBARPROP',
	        '__eg' => '3, 0, value',
		),
	);
}


function get_events_listview(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => array(
			'__desc' => "When an item is single clicked.",
			'__cmd' => 'ITEM',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
			),
		),
		"dclick" => array(
			'__desc' => "When an item is double clicked",
			'__cmd' => 'ITEM',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
			),
		),
		"rclick" => array(
			'__desc' => "When the listview is right clicked.",
			'__cmd' => 'ITEM',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
			),
			'__notes' => 'This event does not require an item to be clicked on to be triggered. If no item was right clicked, no index is returned.'
		),
		"stateclick" => array(
			'__desc' => "When a checkbox is clicked.",
			'__cmd' => 'ITEM',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
			),
		),
		"labelbegin" => array(
			'__desc' => "When item text label editing begins.",
/*			'__cmd' => 'PATH',
			'__params' => array(
				'PATH' => 'Path leading to the item that triggered the event.',
			),
*/
            '__return' => '[r]noedit[/r] to block label editing.'
		),
		"labelend" => array(
			'__desc' => "When the user clicks elsewhere or presses RETURN while editing or clicks somewhere else but edited the text.",
			'__cmd' => 'VALUE',
			'__params' => array(
				'VALUE' => 'Edited text label.',
			),
            '__return' => '[r]noedit[/r] to cancel label editing (for example if string is NULL or invalid to your application).'
		),
		"labelcancel" => "When the user presses ESC while editing or clicks somewhere else without editing the text.",
		"hsclick" => array(
			'__desc' => "When the user clicks on a header item in the listview.",
			'__cmd' => 'N',
			'__params' => array(
				'N' => 'Header item index.',
			),
		),
		"hdclick" => array(
			'__desc' => "When the user double clicks on a header item in the listview.",
			'__cmd' => 'N',
			'__params' => array(
				'N' => 'Header item index.',
			),
		),
		"trackbegin" => array(
			'__desc' => "When a header item is being resized.",
            '__return' => '[r]notrack[/r] to cancel label editing (for example if string is NULL or invalid to your application).'
		),
		'scrollend' => 'When the user has finished scrolling the vertical scrollbar in the listview.',
	    "help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
