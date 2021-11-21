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
		
		'alpha' => 'The control is alpha blended.',
		'singlesel' => 'Allows only one item at a time to be selected. By default, multiple items can be selected.',
		'flatsb' => 'Enables flat scroll bars on the listview.',
		'borderselect' => 'When an item is selected a special border is drawn arround it instead of it being highlighted.',
		'grid' => 'Displays grid lines between item cells. ([s]report[/s] view)',
		'fullrow' => 'Highlights the full row when an item is selected. ([s]report[/s] view)',
		'noheader' => 'Does not display the header on the listview. ([s]report[/s] view unless [s]headeralways[/s] is used)',
		'alignleft' => 'Items are left-aligned in the listview control. ([s]icon[/s] & [s]smallicon[/s] views)',
		'aligntop' => 'Items are aligned with the top of the listview control. ([s]icon[/s] & [s]smallicon[/s] views)',
		'autoarrange' => 'Items are rearranged automaticly when inserted. ([s]icon[/s] & [s]smallicon[/s] views)',
		'showsel' => 'When the control loses focus, the selection is still displayed.',
		'nolabelwrap' => 'The text is displayed on a single line. ([s]icon[/s] & [s]smallicon[/s] views)',
		'checkbox' => 'Enables checkbox support in the list view. ([s]report[/s] view)',
		'editlabel' => 'Enables the possibility to edit listview item labels. (To make a a column editable, see [f]/xdid -o[/f])',
		'headerdrag' => 'Enables the possibility to drag listview headers to another position. ([s]report[/s] view)',
		'sortasc' => 'Items are sorted based on item text in ascending order.',
		'sortdesc' => 'Items are sorted based on item text in descending order.',
		'subitemimage' => 'Subitems have images. ([s]report[/s] view)',
		'noheadersort' => 'This will prevent the headers from being clickable.',
		'noscroll' => 'Scrolling is disabled. All items must be within the client area. ([s]icon[/s] & [s]smallicon[/s] views)',
		'hottrack' => 'Enables hot-track selection in a listview control. Hot track selection means that an item is automatically selected when the cursor remains over the item for a certain period of time.',
		'tooltip' => 'If a partially hidden label in any view lacks tooltip text, the control will unfold the label. If this style is not set, the control will unfold partly hidden labels only for the [s]icon[/s] view.',
//		'tooltips' => 'The listview will have a tooltip.',
		'balloon' => 'The listview and listview items tooltip will use the balloon style.',

		'oneclick' => 'Activates an item on single click. This style also enables hot tracking in the list-view control. Hot tracking means that when the cursor moves over an item, it is highlighted but not selected. (Not to be used with [s]twoclick[/s])',
		'twoclick' => 'Activates an item on double click. This style also enables hot tracking in the list-view control. Hot tracking means that when the cursor moves over an item, it is highlighted but not selected. (Not to be used with [s]oneclick[/s])',
		'underlinehot' => 'Causes those hot items that may be activated to be displayed with underlined text. ([s]oneclick[/s] or [s]twoclick[/s] needed)',
		'underlinecold' => 'Causes those non-hot items that may be activated to be displayed with underlined text. ([s]twoclick[/s] needed)',
		'transparentbkg' => 'Background is painted by the parent.',
		'shadowtext' => 'Enable shadow text on transparent backgrounds only. ([s]transparentbkg[/s] needed)',
		'hidelabels' => 'Hides the labels in icon and small icon view. ([s]icon[/s] & [s]smallicon[/s] view)',
		'headeralways' => 'Show column headers in all view modes.',
		'autocheck' => 'Automatically select check boxes on single click. ([s]checkbox[/s] needed)',
		'columnsnap' => 'Snap to minimum column width when the user resizes a column.',
		'columnjustify' => 'Icons are lined up in columns that use up the whole view. ([s]icon[/s] & [s]smallicon[/s] view)',
		'columnoverflow' => 'Indicates that an overflow button should be displayed in icon/tile view if there is not enough client width to display the complete set of header items. ([s]headeralways[/s] needed)',
		'autosize' => 'Automatically size listview columns. ([s]report[/s] view)',
		'snaptogrid' => 'In icon view, icons automatically snap into a grid. ([s]icon[/s] & [s]smallicon[/s] view)',
		'autoautoarrange' => 'Automatically arrange icons if no icon positions have been set (Similar to [s]autoarrange[/s]) ([s]icon[/s] & [s]smallicon[/s] view).',
		
		'__notes' => array(
		     '[s]grid[/s] does not work with groups. See <a>/xdid -q</a>.',
		     '[s]sortasc[/s] and [s]sortdesc[/s] only work with [s]report[/s] or [s]list[/s] views.'
		),
	);
}


function get_xdid_listview(&$XDID) {
	$XDID = array(
		'a' => array(
	        '__desc' => 'This command lets you add an item to the listview.',
			'__cmd' => '[N] [INDENT] [+FLAGS] [ICON] [STATE] [OVERLAY] [GROUPID] [COLOR] [BGCOLOR] (TEXT) [TAB] [+FLAGS] [ICON] [OVERLAY] [COLOR] [BGCOLOR] (TEXT) [TAB] ...',
			'__eg' => '1 0 +cbku 1 0 0 0 $rgb(170,213,255) $rgb(0,255,255) text $chr(9) + 0 -1 $rgb(10,210,250) $rgb(255,0,255) text2 $chr(9) + 0 -1 $rgb(120,255,210) $rgb(255,255,0) text3',
			'__params' => array(
				'N' => 'Position where the listview item is inserted. (Use [v]0[/v] to insert at the end)',
				'INDENT' => 'Space in icon width at the beginning of a line in the listview ([v]0[/v] for no space)',
				'+FLAGS' => array(
					'__desc' => 'Item flags.',
					'__values' => array(
						'a' => 'Autosize.',
						'b' => 'The item is bold.',
						'c' => 'The item has a text color defined by the [p]COLOR[/p] parameter (use [v]-1[/v] for no color).',
						'C' => 'The item has no text so the icon should be centered',
						'd' => 'The item appears selected like a drop target.',
						'f' => 'The item has focus thus having a dotted box around it.',
						//'F' => "The item's column is a fixed width.",
						'h' => 'Autosize according to header text width.',
						'H' => 'Allows you to load an item from a hash table by name. Cannot be used with [v]+n[/v]',
						'i' => 'The item is italic.',
						'k' => 'The item has a background color defined by the [p]BGCOLOR[/p] parameter (use [v]-1[/v] for no color).',
						'm' => 'Autosize the column to fit header or content.',
						'n' => 'Allows you to load an item from a hash table by index. Cannot be used with [v]+H[/v]',
						's' => 'The item is selected.',
						//'S' => "The item's column is auto sized to max width allowed.",
						'p' => 'The item is a DCX ProgressBar control. (This flag is changing its args etc... expect errors)',
						//'P' => "The item's column is auto sized to a percentage of the total width.",
						't' => 'The item icon appears as 50% opaque, looks like it has a "ghost" effect.',
						'u' => 'The item is underlined.',
						'w' => '(TEXT) is [+IFLAGS] [TABLE NAME] [N|N1-N2|name]',
						'y' => '(TEXT) is [+IFLAGS] [@windowname] [N|N1-N2]',
						'z' => '(TEXT) is [+IFLAGS] [DIALOG] [ID] (N|N1-N2)',
					),
				),
				'ICON' => "Icon index number from the normal icon list.",
				'STATE' => array(
					'__desc' => 'Icon index number from the state icon list.',
					'__values' => array(
						'0' => 'No check.',
						'1' => 'Unchecked.',
						'2' => 'Checked',
					),
				),
				'OVERLAY' => 'Icon index number from the overlay icon list.',
				'GROUPID' => 'Group ID. (Use [v]0[/v] for no value) [o]XP+[/o]',
				'COLOR' => 'Item text color.',
				'BGCOLOR' => 'Item background color.',
				'TEXT' => 'The text to display in the cell.<br />If used with [v]+p[/v] [p]+FLAGS[/p], then this is the DCX ProgressBar [s]style[/s].<br />If used with [v]+H[/v] or [v]+n[/v] [p]+FLAGS[/p], then this is either [v]HASHTABLE[/v] [v]ITEM[/v] or [v]HASHTABLE[/v] [v]INDEX[/v] respectively.',
			),
			'__notes' => array(
				'Icons must be added prior to be used in the listview using [f]/xdid -w[/f]',
				'The [p]INDENT[/p] parameter only works if you are using normal icons in the listview.',
				'Even if there are icons in the icon lists and [v]0[/v] is used for no icon, there will be the an icon indent space in front of the item text. (normal and state icon lists)',
				"You can use [v]0[/v] for the [p]#ICON[/p], and [p]#OVERLAY[/p] values if you wish to use no icon.",
				"You need to use [s]checkbox[/s] style in order to use checkboxes in the listview.",
				'[+IFLAGS] is a combination of',
				' +   No Flags specified. [N] provided, add a single item as text only.',
				' +a  item(s) contain all the flags & icon info etc.. for the item(s) to be added from [INDENT] onwards.',
				' +A  add all items starting at N',
				' +n  [N1-N2] numeric range supplied. Add all items in range. (can\'t be used with +A)',
				'- N1 must be > 0',
				'- N2 can\'t be 0, but can be a negative.',
				' +i  [name] single named item to be added. (can\'t be used with +n or +A)',
			),
		),
		'A' => array(
			'__desc' => 'This command lets you set a property on a specific (sub)item.',
			'__cmd' => '[ROW] [COLUMN] [+FLAGS] [INFO]',
			'__eg' => '4 2 +M Marked text here',
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Property flags.',
					'__values' => array(
						'M' => 'Marked text property. Much like [link page="xdid" section="xdid" flag="M"]/xdid -M[/link], this can be used to define custom data but on a per (sub)item basis.',
					),
				),
				'INFO' => 'The information to set. The usage of this parameter depends on the flags specified in [p]FLAGS[/p].',
			),
		),
		'B' => array(
	        '__desc' => 'This command lets you force label editing on a listview item.',
	        '__cmd' => '[N]',
	        '__eg' => '3',
		),
		'c' => array(
	        '__desc' => 'This command lets you select listview items.',
	        '__cmd' => '[N(,N,N-N,...)]',
	        '__eg' => '1,3,5,10-30',
	        '__notes' => 'In a [s]singlesel[/s] listview, there is only one value for [p]N[/p].'
		),
		'd' => array(
	        '__desc' => "This command lets you delete the Nth listview item.",
	        '__cmd' => '[N(,N,N-N,...)] ([+FLAGS] [SUBITEM] [MATCH])',
	        '__eg' => '1,3,5,10-30',
            '__params' => array(
				'N(,N,N-N,...)' => 'If [v]+FLAGS[/v] is not specified, this is the item(s) to delete. If [v]+FLAGS[/v] is specified this is the item to start searching from.',
                '+FLAGS' => array(
                    '__desc' => "Search flags.",
                    '__values' => array(
						'w' => 'Wildcard search.',
						'r' => 'Regex search.',
						't' => 'Exact match plain text search. (default if no flag specified)',
					),
				),
                'SUBITEM' => 'The subitem to search.',
                'MATCH' => 'The search pattern.',
			),
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
		'G' => array(
	        '__desc' => 'This command lets you change a groups state.',
	        '__cmd' => '[GID] [+MASK] [+STATES]',
	        '__eg' => '1 +O +O',
            '__params' => array(
                'GID' => 'ID of the group to change',
                '+MASK' => array(
                    '__desc' => "The mask of group states to affect",
                    '__values' => array(
						'C' => 'Collapsible',
						'H' => 'Hidden',
						'N' => 'No header',
						'O' => 'Collapsed',
						'S' => 'Selected',
					),
				),
                '+STATES' => array(
                    '__desc' => "The group states to enable",
                    '__values' => array(
						'C' => 'Collapsible',
						'H' => 'Hidden',
						'N' => 'No header',
						'O' => 'Collapsed',
						'S' => 'Selected',
					),
				),
			),
	        '__notes' => 'Only states included in +MASK will be changed. If a state is included in +MASK but not in +STATES then it will be removed.'
		),
		'H' => array(
	        '__desc' => 'This command lets you set the header settings.',
	        '__cmd' => '[COL|COL1-COL2|COL1,COL2|COL1,COL2-COL3 etc..] [+FLAGS] [ARGS]',
	        '__eg' => '1 +s sortdown unchecked',
            '__params' => array(
                'COL|COL1-COL2|COL1,COL2|COL1,COL2-COL3' => 'The column number(s) to change the header for.',
                '+FLAGS' => array(
                    '__desc' => "The action to take.",
                    '__values' => array(
						's' => 'Change header style',
					),
				),
                'ARGS' => array(
                    '__desc' => "The arguments for the given +FLAGS",
                    '__values' => array(
						'sortdown' => 'Change the header to show a sort down arrow. (can\'t be used with sortup or nosort)',
						'sortup' => 'Change the header to show a sort up arrow. (can\'t be used with sortdown or nosort)',
						'nosort' => 'Change the header to not show any sort arrow. (can\'t be used with sortup or sortdown)',
						'checkbox' => 'Change the header to show a checkbox. (can\'t be used with nocheckbox)',
						'nocheckbox' => 'Change the header to not show a checkbox. (can\'t be used with checkbox)',
						'checked' => 'Change the header to show a selected checkbox. (can\'t be used with nocheckbox or unchecked)',
						'unchecked' => 'Change the header to show a unselected checkbox. (can\'t be used with nocheckbox or checked)',
					),
				),
			),
	        '__notes' => 'Only +s flag is supported atm, so only styles can be changed.'
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
			'__cmd' => '[N] [NSUB] [STATE] (COLOR BGCOLOR)',
			'__eg' => array(
				'2 5 +bt',
				'7 1 +u $rgb(255, 0, 0) $rgb(0, 255, 0)',
			),
			'__params' => array(
				'N' => 'The row index for the item.',
				'NSUB' => 'The column index for the item.',
				'STATE' => array(
					'__desc' => "Item flags",
					'__values' => array(
						'b' => 'The item is bold.',
						'd' => 'The item appears selected like a drop target.',
						'f' => 'The item has focus thus having a dotted box around it.',
						'i' => 'The item is italic.',
						's' => 'The item is selected.',
						't' => 'The item appears as 50% opaque, looks like it has a "ghost" effect.',
						'u' => 'The item is underlined.',
					),
				),
			),
			'__notes' => 'Changing the color of column [p]NSUB[/p] will subsequently change the color of following columns.',
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
                'N' => 'The state to set the item to.',
			),
	        '__notes' => "If you are not using the [s]checkbox[/s] style, this function can also change the state icon of an item. Be sure to insert the state icons before you use them.",
		),
		'l' => array(
			'__desc' => 'This command lets you change the listview item icon.',
			'__cmd' => '[N] [NSUB] [ICON] (OVERLAY)',
			'__eg' => '1 2 5 3',
			'__params' => array(
				'N' => 'Item index.',
				'NSUB' => 'Column index.',
				'ICON' => 'Icon index',
				'OVERLAY' => 'Overlay icon index',
			),
			'__notes' => array(
				'Use [v]0[/v] for [p]ICON[/p] and [p]OVERLAY[/p] to display no icon.',
				'If you only want to change the overlay, use [v]-1[/v] for [p]ICON[/p].',
			),
		),
		'm' => array(
			'__desc' => 'This command lets you trigger the group view in the listview, which is off by default. [o]XP+[/o]',
			'__cmd' => '[1|0]',
			'__eg' => '1',
		),
		'n' => array(
			'__desc' => 'This command lets you change the Nth listview column width ([s]report[/s] view only).',
			'__cmd' => '[COL] [+FLAGS] (WIDTH ...)',
			'__eg' => array(
				'2 + 100',
				'0 +m 150 100 200',
			),
			'__params' => array(
				'COL' => 'Column index.',
				'+FLAGS' => array(
					'__desc' => 'Flags for column width.',
					'__values' => array(
						'a' => 'Autosize.',
						'h' => 'Autosize according to header text width.',
						'F' => 'Autosize the column to a fixed width.',
						'S' => 'Autosize the column to fit header or content. Cannot be used with [f]+a[/f].',
						'P' => 'Autosize the column to a percentage of the total width.',
						'm' => 'Allows you to set the widths manually. [p]COL[/p] is ignored.',
					),
				),
				'WIDTH' => 'Header width (Ignored if you use flags [f]+a[/f], [f]+h[/f] or [f]+s[/f]).',
			),
			'__notes' => array(
				'If the flag [f]+h[/f] is used for the last column, the column will be resized to the width of the control.',
				'[f]+m[/f] flag cannot be used with the [f]+a[/f], [f]+h[/f] or [f]+S[/f] flags.',
			),
		),
		'o' => array(
			'__desc' => 'This command lets you change the column order within the listview. ([s]report[/s] view only)',
			'__cmd' => '[COL_ID ...]',
			'__eg' => array(
				'3 2 1',
				'1 3 2 4',
			),
			'__notes' => 'This command can be used to set the move the first column, which is editable. There can only be one editable column at any time.',
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
		'Q' => array(
	        '__desc' => 'This command lets you add, move, & delete groups.',
	        '__cmd' => 'Add [N] [+FLAGS] [GID] [GROUPTEXT]|Move [GID] [N]|Del [GID]',
	        '__eg' => array(
                'Add 1 +l 101 Group 101!',
                'Move 101 4',
                'Del 101'
            ),
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
                'GID' => 'ID that identifies the group when adding items in <a>/xdid -a</a>.',
                'GROUPTEXT' => 'Label for the group.'
			),
		),
		'r' => 'This command lets you clear all the listview items.',
		't' => array(
			'__desc' => 'This command lets you set the different column header text, width and alignment.',
			'__cmd' => '[+FLAGS] [#ICON] [WIDTH] (TEXT) [TAB] [+FLAGS] [#ICON] [WIDTH] (TEXT) [TAB] ...',
			'__eg' => '+l 2 130 column 1 $chr(9) +c 1 130 column 2 $chr(9) +rb 2 130 column 3',
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Item flags.",
					'__values' => array(
						'b' => 'Header icon appears right of text.',
						'c' => 'Header text is centered.',
						'l' => 'Header text is left justified.',
						'r' => 'Header text is right justified.',
						'f' => 'Header is a fixed width. (Column can\'t be resized)',
						'q' => 'Header is a fixed ratio.',
						'd' => 'Header is a split button.',
						'e' => 'Header has a sort down arrow.',
						'g' => 'Header has a sort up arrow.',
					),
				),
				'ICON' => 'Icon index number from the icon list ([v]0[/v] for no icon).',
				'WIDTH' => 'Column width in pixels.',
			),
			'__notes' => array(
				'The leftmost column is always left aligned. If you want a leftmost column aligned other than left, create a zero width left dummy column.',
				'If you re-issue this command while the listview is populated, all the data except the first column will disappear, so you will have to refill the list.',
			),
		),
		'T' => array(
			'__desc' => 'This command lets you set a tooltip for a specific item.',
			'__cmd' => '[N] [NSUB] (TOOLTIP)',
			'__eg' => '2 1 This tooltip applies to Item 2',
			'__notes' => 'This currently only works for the first column.',
		),
		'u' => 'This command makes the currently selected listview item unselected.',
		'v' => array(
			'__desc' => 'This command lets you change the listview item text. If the target item is a DCX ProgressBar, then this allows you to send /xdid commands to the embedded ProgressBar.',
			'__cmd' => '[N] [NSUB] (TEXT)',
			'__eg' => '1 2 5',
			'__params' => array(
				'N' => 'Item row.',
				'NSUB' => 'Column index.',
				'TEXT' => 'Item Text, or command to send to ProgressBar.',
			),
			'__notes' => array(
				'Use [p]TEXT[/p] as [v]-v 80[/v] to set the value of a progress bar.',
				'If N == -1 then sets the empty listview text which is displayed when the listview contains no items.',
				'If NSUB == 1 when N == -1 then redraw after setting empty text.'
			),
		),
		'V' => array(
	        '__desc' => 'This command lets you scroll to the item specified.',
	        '__cmd' => '[N]',
	        '__eg' => '5',
	        '__params' => array(
                'N' => 'Item row to make visible.',
			),
		),
		'w' => array(
			'__desc' => 'This command lets you add an icon to the listview image lists.',
			'__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
			'__eg' => array(
				'+n 113 shell32.dll',
				'+no2 29 shell32.dll',
			),
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Image list flags.",
					'__values' => array(
						// +dcxLoadIcons() flags
						'n' => 'Normal icon list.',
						's' => 'State icon list.',
						'o' => 'Overlay icon list. The overlay index is declared immediately after the [v]+o[/v] flag. Must be used with [v]+n[/v].',
					),
				),
				'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
			'__notes' => array(
			    'Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.',
			    'For readability and ease of indexing, it is suggested you declare your overlay icons AFTER the normal icons. Overlay icons are also added to the normal icon list, so it will mess up your existing icon indexes.',
			    'There is a limit of 15 overlay icons you may use, indexes from [v]1[/v] to [v]15[/v].',
			),
		),
		'W' => array(
			'__desc' => 'This command lets you change the viewing mode.',
			'__cmd' => '[MODE]',
			'__eg' => 'icon',
			'__params' => array(
				'MODE' => array(
					'__desc' => 'The mode to change to.',
					'__values' => array(
						'icon' => 'Changes the listview to icon view.',
						'list' => 'Changes the listview to item list view.',
						'report' => 'Changes the listview to report view.',
						'smallicon' => 'Changes the listview to smallicon view.',
					),
				),
			),
		),
		'S' => array(
			'__desc' => 'This command lets you save the items in a listview to file/@window/hashtable/xml',
			'__cmd' => '[+FLAGS] [N1] [N2] [ARGS]',
			'__eg' => '+f 1 20 listview.txt',
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => "Type of save to do...",
					'__values' => array(
						'c' => 'Save to a custom window.',
						'f' => 'Save to a file. (Data is appended to an existing file or a new file is created)',
						'h' => 'Save to a hashtable. (Table must exist)',
						'x' => 'Save to an xml file. (Data is appended to the bottom of the window, window must exist)',
					),
				),
				'N1' => 'Start of the item range to save.',
				'N2' => 'End of the item range to save.',
				'ARGS' => array(
					'__desc' => 'Arguments dependant on +FLAGS',
					'__values' => array(
						'c' => '@window',
						'f' => 'filename',
						'h' => 'hashtable',
						'x' => 'dataset_name filename',
					),
				),
			),
			'__notes' => array(
				'This command is incomplete. Only +c & +h work at this time.',
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
	
	writeDcxLoadIcon($XDID, 'w', '+FLAGS', 1);
}

function get_xdidprops_listview(&$XDIDPROPS) {
	$XDIDPROPS = array(
		'columns' => array(
			'__desc' => 'This property lets you retreive the number of columns for the listview.',
			'__notes' => array(
				'If a listview is not a [s]report[/s] listview, this will return [v]0[/v].',
				'If a listview has [s]noheader[/s], this will return [v]0[/v].',
			),
		),
		'columnorder' => array(
			'__desc' => 'This property lets you retreive the order of the Nth listview column.',
			'__cmd' => '(N)',
			'__eg' => '2',
			'__notes' => 'When [p]N[/p] is not specified, the order of all columns are returned.',
		),
		"text" => array(
			'__desc' => "This property lets you retreive a Nth listview item text.",
			'__cmd' => 'N, (NSUB)',
			'__eg' => '1, 2',
			'__notes' => 'If [p]NSUB[/p] is not specified, it will return the first column.',
		),
		"selected" => array(
			'__desc' => 'This property lets you determine if the specified item is selected or not.',
			'__cmd' => '[N]',
			'__eg' => '2',
		),
		"seltext" => array(
			'__desc' => 'This property lets you retreive the selected listview item text.',
			'__cmd' => '(NSUB)',
			'__eg' => '3',
			'__notes' => 'If [p]NSUB[/p] is not specified, it will return the first column.',
		),
		"sel" => array(
			'__desc' => 'This property lets you retreive the indexes of selected items in the listview.',
			'__cmd' => '(N)',
			'__eg' => '2',
			'__params' => array(
				'N' => 'Optional parameter allowing you to obtain the Nth selected index.',
			),
			'__notes' => array(
				'If the listview is [s]multiline[/s], and [p]N[/p] is not supplied, then this command will return a comma-separated list of the selected indexes.',
				'If [p]N[/p] is [v]0[/v], then this will return the number of lines selected. (Same as [f]$xdid().selnum[/f])',
				'When using this property without [p]N[/p], the result is limited to 900 characters. This is due to an implementation that mIRC has imposed on DLL communication, so it is advised to use the [p]N[/p] parameter to prevent inaccurate results.',
			),
		),
		"selnum" => 'This property lets you retreive the number of selected items in the listview.',
		"find" => array(
		    '__desc' => "This property lets you retreive the listview item index and column index of the Nth found listview item matching the input search patern.",
		    '__cmd' => '[TAB]Matchtext[TAB], T, COLUMN, N',
	        '__eg' => '$chr(9) *Item* $chr(9), W, 0, 3',
	        '__params' => array(
	            'MATCHTEXT' => 'String composed of wildcards or regex paterns used for the search.',
	            'T' => array(
	                '__desc' => 'Value indicating if the search is using a regex patern or wildcard string.',
	                '__values' => array(
						'E' => 'Matchtext is an exact match.',
						'W' => 'Matchtext is a wildcard string.',
						'R' => 'Matchtext is a regex patern.',
					),
				),
                'COLUMN' => "Parameter indicating the column to search.",
                'N' => 'Parameter indicating to return the Nth match.'
			),
			'__notes' => array(
			    'If [p]COLUMN[/p] is [v]0[/v], searchs through all columns.',
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
		"hstate" => array(
		    '__desc' => "This property lets you retreive the state of the Nth listview header.",
			'__cmd' => 'N',
	        '__eg' => '2',
	        '__notes' => 'If you are not using headers an error \'Unable to get Header Window\' is returned. Otherwise, returns a combination of [v]sortdown[/v] (sortdown arrow shown), [v]sortup[/v] (sort up arrow shown), or [v]dropdown[/v] (split button shown) or [v]checkbox[/v] (checkbox shown) or [v]checked[/v] (checkbox shown & checked).'
		),
		'icon' => array(
			'__desc' => 'This property lets you retreive a Nth listview item icon number.',
			'__cmd' => 'N, NSUB',
			'__eg' => '1, 2',
		),
		"tbitem" => array(
		    '__desc' => "This property lets you retreive top and bottom visible item indexes.",
            '__notes' => 'Returns [v]0[/v] for no items or error.'
		),
		"mouseitem" => array(
			'__desc' => "This property lets you retreive the index of the item (and subitem) which the mouse pointer is over.",
			'__notes' => 'Returns [v]0[/v] if mouse is not over any item.',
		),
		"hwidth" => array(
			'__desc' => "This property lets you retreive the Nth listview header width.",
			'__cmd' => '(N)',
			'__eg' => '2',
			'__notes' => 'When [p]N[/p] is not specified, all column widths are returned in their viewing order.',
		),
		"htext" => array(
		    '__desc' => "This property lets you retreive the Nth listview header text.",
			'__cmd' => '[N]',
	        '__eg' => '2',
		),
        "hicon" => array(
		    '__desc' => "This property lets you retreive the Nth listview header icon index.",
			'__cmd' => 'N',
	        '__eg' => '2',
		),
		'genabled' => 'This property lets you retreive whether groups are enabled or not.',
		'gnum' => 'This property retreives the number of groups.',
		'pbar' => array(
			'__desc' => "This property lets you retreive ProgressBar properties from a specific cell.",
			'__cmd' => 'N, NSUB, PBARPROP',
			'__eg' => '3, 1, value',
		),
		'markeditem' => array(
			'__desc' => 'This property lets you retreive the marked (see [link section="xdid" flag="A"]/xdid -A +M[/link]) text for the listview (sub)item specified.',
			'__cmd' => '[ROW] [COLUMN]',
	        '__eg' => '3, 1',
		),
	);
}


function get_events_listview(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => array(
			'__desc' => "When an item is single clicked.",
			'__cmd' => 'ITEM SUBITEM',
			'__eg' => '4 2',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
				'SUBITEM' => 'Column index where the event was triggered.',
			),
			'__notes' => 'This event does not require an item to be clicked on to be triggered. If no item was clicked clicked, no index is returned.'
		),
		"dclick" => array(
			'__desc' => "When an item is double clicked",
			'__cmd' => 'ITEM SUBITEM',
			'__eg' => '4 2',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
				'SUBITEM' => 'Column index where the event was triggered.',
			),
			'__notes' => 'This event does not require an item to be clicked on to be triggered. If no item was double clicked, no index is returned.'
		),
		"rclick" => array(
			'__desc' => "When the listview is right clicked.",
			'__cmd' => 'ITEM SUBITEM',
			'__eg' => '4 2',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
				'SUBITEM' => 'Column index where the event was triggered.',
			),
			'__notes' => 'This event does not require an item to be clicked on to be triggered. If no item was right clicked, no index is returned.'
		),
		"rdclick" => array(
			'__desc' => "When the listview is double right-clicked.",
			'__cmd' => 'ITEM SUBITEM',
			'__eg' => '4 2',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
				'SUBITEM' => 'Column index where the event was triggered.',
			),
			'__notes' => 'This event does not require an item to be clicked on to be triggered. If no item was double-right clicked, no index is returned.'
		),
		"hover" => array(
			'__desc' => "When the mouse is hovering over the listview.",
			'__cmd' => 'ITEM SUBITEM',
			'__eg' => '4 2',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
				'SUBITEM' => 'Column index where the event was triggered.',
			),
			'__notes' => 'This event does not require an item to be hovered over to be triggered. If the mouse is not over an item, no index is returned.'
		),
		"stateclick" => array(
			'__desc' => "When a checkbox is clicked.",
			'__cmd' => 'ITEM',
			'__eg' => '3',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
			),
		),
		"labelbegin" => array(
			'__desc' => "When item text label editing begins.",
			'__cmd' => 'ITEM SUBITEM',
			'__eg' => '3 1',
			'__return' => '[r]noedit[/r] to block label editing.',
			'__notes' => 'Currently only the first column is editable.',
		),
		"labelend" => array(
			'__desc' => "When the user clicks elsewhere or presses RETURN while editing or clicks somewhere else but edited the text.",
			'__cmd' => 'VALUE',
			'__eg' => 'New text value',
			'__params' => array(
				'VALUE' => 'Edited text label.',
			),
            '__return' => '[r]noedit[/r] to cancel label editing (for example if string is NULL or invalid to your application).'
		),
		"labelcancel" => "When the user presses ESC while editing or clicks somewhere else without editing the text.",
		"hsclick" => array(
			'__desc' => "When the user clicks on a header item in the listview.",
			'__cmd' => 'N',
			'__eg' => '2',
			'__params' => array(
				'N' => 'Header item index.',
			),
		),
		"hdclick" => array(
			'__desc' => "When the user double clicks on a header item in the listview.",
			'__cmd' => 'N',
			'__eg' => '2',
			'__params' => array(
				'N' => 'Header item index.',
			),
		),
		'keydown' => array(
			'__desc' => 'When the user presses a key and the listview has focus.',
			'__cmd' => 'KEYCODE',
			'__eg' => '65',
			'__params' => array(
				'KEYCODE' => 'The ASCII equivelant of the key pressed. This is not case-sensitive.',
			),
			'__return' => '[r]nospace[/r] to prevent spacebar (when [p]KEYCODE[/p] is [v]32[/v]) from changing item checkbox state.',
		),		
		"trackbegin" => array(
			'__desc' => 'When a header item is being resized.',
			'__cmd' => 'N',
			'__eg' => '2',
			'__return' => '[r]notrack[/r] to cancel resizing.'
		),
		'scrollend' => 'When the user has finished scrolling the vertical scrollbar in the listview.',
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
		"selected" => array(
			'__desc' => 'Triggered when you select an item.',
			'__cmd' => 'LINE COLUMN',
			'__eg' => '4 2',
			'__params' => array(
				'LINE' => 'The line selected.',
				'COLUMN' => 'The column selected.',
			),
			'__notes' => '[p]COLUMN[/p] will always return [v]1[/v] until a workaround is found.',
		),
		"deselected" => array(
			'__desc' => 'Triggered when an item is deselected.',
			'__cmd' => 'LINE COLUMN',
			'__eg' => '4 2',
			'__params' => array(
				'LINE' => 'The line selected.',
				'COLUMN' => 'The column selected.',
			),
			'__notes' => '[p]COLUMN[/p] will always return [v]1[/v] until a workaround is found.',
		),	
	);
}
?>
