<?php
function get_intro_treeview() {
	echo "This control enables you to create a treeview.";
}


function get_styles_treeview(&$STYLES) {
	$STYLES = array(
		'haslines' => 'Items are linked with lines.',
		'linesatroot' => 'Enables lines to be connected to the root item (needs the [s]haslines[/s] style to work).',
		'hasbuttons' => 'Enables &plusmn; buttons on the side of items containing child items for expand/collapse operations.',
		'checkbox' => 'Enables Checkbox support in the treeview.',
		'editlabel' => 'Makes the treeview items editable.',
		'nohscroll' => 'Disables horizontal scrolling in the treeview.',
		'noscroll' => 'Disables scrolling in the treeview.',
		'notooltips' => 'Disables tooltip popup on items.',
		'showsel' => 'Selection is always visible, even when control loses focus.',
		'fullrow' => 'Selection highlight is the full row of the item. Cannot be used with [s]haslines[/s].',
		'singleexpand' => 'Causes the item being selected to expand and the item being unselected to collapse upon selection in the tree view. If the user holds down the CTRL key while selecting an item, the item being unselected will not be collapsed.',
	);
}


function get_xdid_treeview(&$XDID) {
	$XDID = array(
	    'a' => array(
	        '__desc' => 'This command lets you add an item to the treeview.',
	        '__cmd' => '[PATH] [TAB] [+FLAGS] [#ICON] [#SICON] [#OVERLAY] [STATE] [INTEGRAL] [COLOR] (Item Text) [TAB] (Tooltip Text)',
	        '__eg' => '$+(-1 2 4,$chr(9),+bc 1 1 0 0 0 $rgb(0,0,255) Item Text,$chr(9),Tooltip)',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => 'Item flags.',
                    '__values' => array(
	                    'b' => 'Item text is bold.',
						'c' => 'Item text is colored ([p]COLOR[/p] is the text color).',
						'e' => 'Item is explanded.',
						's' => 'Item appears selected.',
						'u' => 'Item text is underlined.',
					),
				),
                '#ICON' => "Icon index number from the normal icon list.",
                '#SICON' => "Selected Icon index number from the normal icon list.",
                '#OVERLAY' => 'Icon index number from the normal icon list that was marked as an overlay icon. ([n]Not functional yet[/n])',
				'STATE' => array(
                    '__desc' => 'Icon index number from the state icon list.',
					'__values' => array(
						'0' => 'No check.',
						'1' => 'Unchecked.',
						'2' => 'Checked',
					),
				),
				'INTEGRAL' => 'Multiple of the regular treeview item height.',
				'COLOR' => 'Item text color of the treeview item in $rgb() format. (Needs to add flag [f]+c[/f] to work)',
			),
			'__notes' => array(
				"You can use [v]0[/v] for the [p]#ICON[/p], [p]#SICON[/p], and [p]#OVERLAY[/p] values if you wish to use no icon.",
				"You need to use [s]checkbox[/s] style in order to use checkboxes in the treeview.",
				"[p]INTEGRAL[/p] is calculated in this manner.<br />
				[v]0[/v] = normal height,<br />
				[v]1[/v] = twice the height,<br />
				[v]2[/v] = 3X height, etc.",
			),
		),
		'c' => array(
	        '__desc' => 'This command lets you select a treeview item.',
	        '__cmd' => '[PATH]',
	        '__eg' => '1 3 5',
		),
		'd' => array(
	        '__desc' => "This command lets you delete a treeview item (and it's sub-items if any).",
	        '__cmd' => '[PATH]',
	        '__eg' => '1 3 5',
		),
		'g' => array(
	        '__desc' => 'This command lets you set the treeview item height (in pixels).',
	        '__cmd' => '[HEIGHT]',
	        '__eg' => '15',
	        '__notes' => 'You can use the value -1 to reset to the default height.'
		),
		'i' => array(
	        '__desc' => 'This command lets you change treeview colors.',
	        '__cmd' => '[+FLAGS] [COLOR]',
	        '__eg' => '+b $rgb(0,255,0)',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Color flags.",
                    '__values' => array(
						'b' => 'Background color.',
						'l' => 'Line color.',
						't' => 'Text color.',
					),
				),
                'COLOR' => 'Color in $rgb() format.',
			),
	        '__notes' => "Use [v]-1[/v] to revert to default color.",
		),
		'j' => array(
	        '__desc' => 'This command lets you change the icon and selected icon of a treeview item.',
	        '__cmd' => '[PATH] [TAB] [ICON] [SICON]',
	        '__eg' => '1 2 $chr(9) 3 3',
		),
		'k' => array(
	        '__desc' => 'This command lets you change treeview colors.',
	        '__cmd' => '[STATE] [PATH]',
	        '__eg' => '2 1 3 5',
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
	        '__desc' => 'This command lets you change the size of the treeview icons.',
	        '__cmd' => '[SIZE]',
	        '__eg' => '24',
            '__notes' => array(
                'You need to use this command before adding icons to a treeview.',
                'Values can be [v]16[/v] (default), [v]24[/v], or [v]32[/v].',
			),
		),
		'm' => array(
	        '__desc' => 'This command lets you move a treeview item and its sub-items. The first path is the item to be moved to the second path.',
	        '__cmd' => '[PATH1] {TAB} [PATH2]',
	        '__eg' => '1 2 $chr(9) 1',
		),
		'n' => array(
	        '__desc' => 'This command lets you copy a treeview item and its sub-items. The first path is the item to be copied to the second path.',
	        '__cmd' => '[PATH1] {TAB} [PATH2]',
	        '__eg' => '1 2 $chr(9) 1',
		),
		'o' => array(
	        '__desc' => 'This command lets you change the tooltip text of a treeview item.',
	        '__cmd' => '[PATH] [TAB] (TipText)',
	        '__eg' => '1 2 $chr(9) Blah',
		),
		'q' => array(
	        '__desc' => 'This command lets you change the font styles and color on a treeview item.',
	        '__cmd' => '[+FLAGS] [COLOR] [PATH]',
	        '__eg' => '+bcu $rgb(128,0,255) 1 2',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Check State",
                    '__values' => array(
						'b' => 'Item text is bold.',
						'u' => 'Item text is underlined..',
						'c' => 'Item text is colored. ([p]COLOR[/p] is the text color).',
					),
				),
                'COLOR' => 'Item text color of the treeview item in $rgb() format. (Needs to add flag [f]+c[/f] to work)',
			),
	        '__notes' => "Use [v]-1[/v] as [p]COLOR[/p] value if you omit the [f]+c[/f] switch.",
		),
		'r' => 'This command lets you clear all the treeview items.',
		't' => array(
	        '__desc' => 'This command lets you toggle/expand/collapse treeview items. You can use root as the path with the +a and +z switches.',
	        '__cmd' => '[+FLAGS] [PATH]',
	        '__eg' => '+a root',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Toggle flags.",
                    '__values' => array(
						'a' => 'Expands the branch and all sub-branches.',
						'c' => 'Collapses the branch.',
						'e' => 'Expands the branch.',
						'p' => 'Expands partially the branch.',
						'r' => 'Collapses and deletes the branches children.',
						't' => 'Toggles opposite action on branch.',
						'z' => 'Collapses the branch and all sub-branches.',
					),
				),
                'COLOR' => 'Item text color of the treeview item in $rgb() format. (Needs to add flag [f]+c[/f] to work)',
			),
	        '__notes' => "Use [v]-1[/v] as [p]COLOR[/p] value if you omit the [f]+c[/f] switch.",
		),
		'u' => 'This command makes the currently selected treeview item unselected.',
		'v' => array(
	        '__desc' => 'This command lets you change the text of a treeview item.',
	        '__cmd' => '[PATH] [TAB] (ItemText)',
	        '__eg' => '1 2 $chr(9) Blah',
		),
		'w' => array(
	        '__desc' => 'This command lets you add an icon to the treeview image lists.',
	        '__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
	        '__eg' => '+n 113 C:/mIRC/shell.dll',
	        '__params' => array(
	            '+FLAGS' => array(
	                '__desc' => "Image list flags.",
	                '__values' => array(
						'd' => 'Disabled icon list.',
						'n' => 'Normal icon list.',
						'h' => 'Hottrack icon list.',
					),
				),
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => array(
	            "Use 0 if the file is a single icon file.",
			),
		),
		'y' => array(
	        '__desc' => 'This command lets you clear the treeview image lists.',
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
	        '__desc' => 'This command lets you sort the data in the treeview control.',
	        '__cmd' => '[+FLAGS] [PATH] [TAB] (ALIAS)',
	        '__eg' => '+tab root tvsort',
	        '__params' => array(
	            '+FLAGS' => array(
	                '__desc' => "Sort flags.",
	                '__values' => array(
						'a' => 'Ascendant order sorting. ([v]Default[/v])',
						'b' => 'Sorts only the given branch.',
						'c' => 'Custom sorting provided by [p]ALIAS[/p].',
						'd' => 'Descendant order sorting.',
						'n' => 'Numerical sorting.',
						'r' => 'Sorts recursively all sub-branches.',
						's' => 'Case-sensitive sorting. (With Alpha-numeric)',
						't' => 'Alpha-numeric sorting. ([v]Default[/v])',
					),
				),
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

function get_xdidprops_treeview(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive a treeview item text.",
	        '__cmd' => '{TAB}[Matchtext]{TAB}, [T], [N], (SUBPATH)',
	        '__eg' => '1 2',
		),
		"seltext" => 'This property lets you retreive the selected treeview item text.',
		"selpath" => "This property lets you retreive the <a href='#path'>Item Path</a> that leads to the selected item in the treeview.",
		"find" => array(
		    '__desc' => "This property lets you retreive a treeview <a href='#path'>Item Path</a> that leads to the Nth found treeview item matching the input search patern. If [p]N[/p] is [v]0[/v], returns the total number of matching items. [p]SUBPATH[/p] is an optional parameter to start the search at a desired branch.",
		    '__cmd' => 'PATH',
	        '__eg' => '1 2',
	        '__params' => array(
	            'T' => array(
	                '__desc' => 'Value indicating if the search is using a regex patern or wildcard string.',
	                '__values' => array(
						'W' => 'Matchtext is a wildcard string.',
						'R' => 'Matchtext is a regex patern.',
					),
				),
                'N' => "Parameter indicating to return the Nth match. If [p]N[/p] is [v]0[/v], returns the total matches found.",
                'SUBPATH' => 'Optional parameter to start the search at a specific branch.'
			),
			'__notes' => array(
				'The {TAB} characters surrounding the matchtext are mandatory or else it wont work.',
				'If [p]SUBPATH[/p] is blank, searches at root of treeview.',
			)
		),
		"num" => array(
		    '__desc' => "This property lets you retreive the number of child items of a treeview item. You can use the value [v]root[/v] as [p]ITEMPATH[/p] to get the number of childs of the root item.",
			'__cmd' => '[ITEMPATH]',
	        '__eg' => '1 2',
		),
		"state" => array(
		    '__desc' => "This property lets you retreive the check item state of an item.",
			'__cmd' => '[ITEMPATH]',
	        '__eg' => '1 2',
	        '__notes' => 'If you are not using the [s]checkbox[/s] style, the value returned is the state icon value. Otherwise, returns [v]2[/v] (checked), [v]1[/v] (unchecked), or [v]0[/v] (no check).'
		),
		"expand" => array(
		    '__desc' => "This property lets you retreive whether a treeview item is expanded or not.",
			'__cmd' => '[ITEMPATH]',
	        '__eg' => '1 2',
		),
		"icon" => array(
		    '__desc' => "This property lets you retreive the treeview item icon.",
			'__cmd' => '[ITEMPATH]',
	        '__eg' => '1 2',
		),
		"tooltip" => array(
		    '__desc' => "This property lets you retreive the treeview item tooltip.",
			'__cmd' => '[ITEMPATH]',
	        '__eg' => '1 2',
		),
		"tooltip" => array(
			'__desc' => "This property lets you retreive the treeview Item Path over which the mouse pointer is hovering over.",
			'__notes' => 'Returns [v]0[/v] if mouse is not over any item.',
		),
	);
}

function paths_treeview() {
	ob_start();
?><a name="path"></a>
The treeview control has an engine that parses the given item "path" data to know where to insert/delete/modify a given item in the control. The parameters you supply are a space separated numerical token list of the treeview branches that lead to the item position:<br />
<br />
<table>
	<tr>
		<td class="syntax">Format</td><td>N N N ... N</td>
	</tr>
	<tr>
		<td class="example">Example</td><td>1 2 3</td>
	</tr>
</table>
<br />
This position represents the 3rd child branch of the 2nd child branch of the first branch as shown by the selected item in the picture below:<br />
<br />
<div style="text-align: center;">
	<img src="images/treeview2.png" /><br />
</div>
<br />
You can also use the value [v]-1[/v] for any of the values to indicate the parser that you want to insert the item at the end of the branch (if used as the last path number) or to select the last branch of a given sub-path (if not used as last path number).<br />
<br />
<table>
	<tr>
		<td class="example">Example</td><td>-1 -1</td>
	</tr>
</table>
This would add the item at the end of the last branch in the first column of branches.<br />
<br />
[n]If you specify a path that does not exist the engin will reverse to the last valid path encoutered during parsing.[/n]
<?php
	$str = ob_get_clean();
	dcxdoc_print_description("TreeView Item Path", $str);
}

function get_events_treeview(&$EVENTS) {
	$EVENTS = array(
	    "sclick" => array(
			'__desc' => "When an item is single clicked.",
			'__cmd' => 'PATH',
			'__params' => array(
				'PATH' => 'Path leading to the item that triggered the event.',
			),
			'__notes' => 'You cant use $xdid().tvseltext to get item text, you need to use $xdid().tvtext with item path given.'
		),
		"dclick" => array(
			'__desc' => "When an item is double clicked.",
			'__cmd' => 'PATH',
			'__params' => array(
				'PATH' => 'Path leading to the item that triggered the event.',
			),
			'__notes' => 'You cant use $xdid().tvseltext to get item text, you need to use $xdid().tvtext with item path given.'
		),
		"rclick" => array(
			'__desc' => "When the treeview is right clicked.",
			'__cmd' => 'PATH',
			'__params' => array(
				'PATH' => 'Path leading to the item that triggered the event.',
			),
			'__notes' => array(
				'You cant use $xdid().tvseltext to get item text, you need to use $xdid().tvtext with item path given.',
				'This event does not require an item to be clicked on to be triggered. If no item was right clicked, no path is returned.'
			),
		),
		"stateclick" => array(
			'__desc' => "When a checkbox is clicked.",
			'__cmd' => 'STATE PATH',
			'__params' => array(
				'STATE' => 'New item check state ([s]checkbox[/s] style) or state icon value.',
				'PATH' => 'Path leading to the item that triggered the event.',
			),
		),
		"buttonclick" => array(
			'__desc' => "When a &plusmn; button is clicked.",
			'__cmd' => 'PATH',
			'__params' => array(
				'PATH' => 'Path leading to the item that had his &plusmn; button clicked.',
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
		"expand" => array(
			'__desc' => "When a branch is expanded.",
			'__cmd' => 'PATH',
			'__params' => array(
				'PATH' => 'Path leading to the item that was expanded.',
			),
		),
		"collapse" => array(
			'__desc' => "When a branch is collapsed.",
			'__cmd' => 'PATH',
			'__params' => array(
				'PATH' => 'Path leading to the item that was collapsed.',
			),
		),
		
		
		
		
	    "help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
