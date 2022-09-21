<?php
function get_intro_comboex() {
	echo "This control enables you to create a comboex control, which is a combo box extended to support icons.";
}


function get_styles_comboex(&$STYLES) {
	$STYLES = array(
		"simple" => "Combo that is a list with an edit box",
		"dropdown" => "Simple dropdown combo.",
		"dropedit" => "Dropdown combo with an editbox.",
		'hscroll' => 'Enables the horizontal scroll bar in the listbox part.',
		'sort' => 'Sort the contents of the listbox part. (untested)',

		'noeditimage' => 'No icon is show in edit control with [s]dropedit[/s] style',
		'noeditindent' => "The icon in the edit control isn`t indented.",
		'nosizelimit' => 'No limit on how small the control can be (can ofcourse lead to display oddness)',
		'endellipsis' => 'Place ellipsis at the end of text if it extends past the edge of the control.',

		'__notes' => 'When creating a ComboEx control, it is important to remember that the height of the control <strong>includes</strong> the dropdown list portion.',
	);
}


function get_xdid_comboex(&$XDID) {
	$XDID = array(
	    'a' => array(
	        '__desc' => 'This command lets you add an item to the comboex.',
	        '__cmd' => '[N] [INDENT] [#ICON] [#SELECTED] [#OVERLAY] [Item Text]',
	        '__eg' => '3 0 1 2 0 Visit scriptsdb.org',
            '__params' => array(
                'N' => "Position where the comboex item will be added.",
                'INDENT' => "Number of indent widths from the left border.",
                '#ICON' => "Icon displayed when item is not selected.",
                '#SELECTED' => "Icon displayed when item is selected.",
                '#OVERLAY' => 'Overlay icon of the item ([n]Not functional yet[/n])',
			),
			'__notes' => array(
				"You can use [v]0[/v] for the [p]N[/p] value to insert the item at the end of the comboex.",
				"Every unit of [p]INDENT[/p] is a 10 pixel indent.",
				"Use [v]0[/v] for [p]#ICON[/p] or [p]#SELECTED[/p] if you wish to use no icon.",
				"Use [v]-1[/v] for [p]N[/p] to set the text for the editable part of the comboex (only used with [s]dropedit[/s] style).",
			),
		),
		'c' => array(
	        '__desc' => 'This command lets you select the Nth comboex item.',
	        '__cmd' => '[N]',
	        '__eg' => '5',
		),
		'd' => array(
	        '__desc' => 'This command lets you delete the Nth comboex item.',
	        '__cmd' => '[N]',
	        '__eg' => "6",
		),
		'l' => array(
	        '__desc' => 'This command lets you set the controls read only state.',
	        '__cmd' => "[0|1]",
	        '__eg' => "1",
		),
		'u' => array(
	        '__desc' => 'This command makes the currently selected comboex item unselected.',
		),
		'w' => array(
	        '__desc' => 'This command lets you add an icon to the comboex image list.',
	        '__cmd' => "[+FLAGS] [INDEX] [FILENAME]",
	        '__eg' => "+ 113 shell32.dll",
            '__params' => array(
            	// +FLAGS
				'INDEX' => "Icon index in icon archive",
				'FILENAME' => "Icon archive filename",
			),
	        '__notes' => "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
		),
		'y' => 'This command lets you clear the comboex image list.',
		'r' => 'This command lets you clear all the comboex items.',
		'A' => array(
	        '__desc' => 'This command lets you add custom info to items.',
	        '__cmd' => "[ROW] [+FLAGS] [INFO]",
	        '__eg' => "13 +M Yetis are real",
            '__params' => array(
				'ROW' => "row",
				'+FLAGS' => "M - Mark info",
				'INFO' => "Info",
			),
	        '__notes' => "Only +M flag is supported atm.",
		),
	);
	
	writeDcxLoadIcon($XDID, 'w', '+FLAGS', 1);
}

function get_xdidprops_comboex(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"text" => array(
		    '__desc' => "This property lets you retreive the Nth comboex item text.",
	        '__cmd' => '[N]',
	        '__eg' => '1',
	        '__notes' => array(
	            'You can use a value of [v]0[/v] for [p]N[/p] to retrieve the text of the editbox.',
			),
		),
		"sel" => array(
		    '__desc' => 'This property lets you retreive the comboex selected item number.',
		),
		"seltext" => array(
		    '__desc' => 'This property lets you retreive selected comboex item text.',
		),
		"num" => array(
		    '__desc' => "This property lets you retreive the total number of comboex items.",
		),
		"find" => array(
		    '__desc' => "This property lets you retreive the comboex item index of the Nth found comboex item matching the input search patern.",
			'__cmd' => '[TAB][MATCHTEXT][TAB], TYPE, N',
	        '__eg' => '$chr(9) *Item* $chr(9), W, 3',
	        '__params' => array(
	            "MATCHTEXT" => "String composed of wildcards or regex paterns used for the search.",
	            "TYPE" => array(
	                '__desc' => "Value indicating if the search is using a regex patern or wildcard string.",
                    '__values' => array(
                        'W' => '[p]MATCHTEXT[/p] is a wildcard string.',
                        'R' => '[p]MATCHTEXT[/p] is a regex patern.',
					),
				),
				"N" => "Parameter indicating to return the Nth match."
			),
	        '__notes' => array(
	            "The [TAB] characters surrounding the matchtext are mandatory or else it won't work.",
	            'If [p]N[/p] is [v]0[/v], returns the total number of matching items found.'
			),
		),
	);
}

function get_events_comboex(&$EVENTS) {
	$EVENTS = array(
        "sclick" => array(
			'__desc' => "When an item is selected in the comboex.",
			'__cmd' => 'ITEM',
                        '__eg' => '6',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
			),
		),
		"dclick" => array(
			'__desc' => "When an item is double-clicked in a simple comboex.",
			'__cmd' => 'ITEM',
                        '__eg' => '6',
			'__params' => array(
				'ITEM' => 'Item number where the event was triggered.',
			),
		),
	    "edit" => "When the text is edited in a comboex with editbox.",
	    "return" => "When the <b>Enter</b> key is pressed in the editbox of a comboex.",
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
	);
}
?>
