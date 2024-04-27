<?php

function xtreebar_load($page) {
    global $XTREEBAR, $XTREEBARPROPS;

    loadSection($XTREEBAR, "get_xtreebar");
    loadSection($XTREEBARPROPS, "get_xtreebarprops");
}

function xtreebar_unload() {
    global $XTREEBAR, $XTREEBARPROPS;

    $XTREEBAR = array();
    $XTREEBARPROPS = array();
}

function get_intro_xtreebar() {
    echo "XTreebar is a command implemented to control the mIRC Treebar. Root control: <a href='https://learn.microsoft.com/en-gb/windows/win32/controls/bumper-tree-view-tree-view-control-reference'>TreeView</a>";
}

function xtreebar_layout($page, $pagelabel) {
    global $SECTION, $XTREEBAR, $XTREEBARPROPS;

    dcxdoc_print_intro($page);
	
    // /xtreebar commands
    if ($XTREEBAR) {
        $SECTION = SECTION_XTREEBAR;
        $count = 1;

        dcxdoc_print_description('/xtreebar Commands', "The /xtreebar command is used to control the mIRC Treebar and change various properties.");

        foreach ($XTREEBAR as $cmd => $data) {
            dcxdoc_format_xtreebar($cmd, $data, $count);
            $count++;
	}
    }

    // $xtreebar commands
    if ($XTREEBARPROPS) {
        $SECTION = SECTION_XTREEBARPROPS;
        $count = 1;

        dcxdoc_print_description('$xtreebar Properties', 'The $xtreebar identifier is used to retrieve information about the mIRC TreeBar.');

        foreach ($XTREEBARPROPS as $cmd => $data) {
       	    dcxdoc_format_xtreebarprops($cmd, $data, $count);
      	    $count++;
	}
    }
}

function get_xtreebar(&$XTREEBAR) {
    $XTREEBAR = array(
        'c' => array(
            '__desc' => 'This command changes the mIRC Treebar colors.',
            '__cmd' => '[+COLORFLAGS] [COLOUR]',
            '__eg' => '+b 255',
            '__params' => array(
                '+COLORFLAGS' => array(
                    '__desc' => 'Color flags.',
                    '__values' => array(
                        'b' => "Background color.",
                        'e' => 'Event color.',
                        'E' => 'Event background color.',
                        'i' => 'Insert mark color.',
                        'l' => "Line color.",
                        's' => 'Selected text color.',
                        'S' => 'Selected background color.',
                        't' => "Text color.",
                        'm' => 'Message color.',
                        'M' => 'Message background color.',
                    ),
                ),
                'COLOR' => 'The color in $rgb() format.',
            ),
            '__notes' => 'This requires [cmd]/xtreebar -T[/cmd] to be activated.',
        ),
        'f' => array(
            '__desc' => 'This command allows you to change the mIRC Treebar font.',
            '__cmd' => '[+FONTFLAGS] [CHARSET] [SIZE] [FONTNAME]',
            '__eg' => '+i default 20 Verdana',
            '__params' => array(
                '+FONTFLAGS' => array(
                    '__desc' => "Font style flags.",
                    '__values' => array(
                        'a' => "Font is anti-aliased.",
                        'b' => "Font is bold.",
                        'd' => "Default font.",
                        'i' => "Font is italic.",
                        's' => "Font is strikedout.",
                        'u' => "Font is underlined.",
                    ),
                ),
                'CHARSET' => "Can be any of the following: ansi, baltic, chinesebig, default, easteurope, gb2312, greek, hangul, mac, oem, russian, shiftjis, symbol, turkish, vietnamese.",
                'SIZE' => "Font size in points.",
                'FONTNAME' => "Font name. Ex: Tahoma, Courier New, Arial, ...",
            ),
        ),
        's' => array(
            '__desc' => 'This command allows you to change the mIRC Treebar styles.',
            '__cmd' => '[STYLES]',
            '__eg' => 'tooltips scroll singleexpand',
            '__params' => array(
                'fullrowselect' => 'Enables fullrowselection.',
                'nofullrowselect' => 'Disables fullrowselection.',

                'haslines' => 'Enables haslines.',
                'nohaslines' => 'Disables haslines.',

                'trackselect' => 'Enables hot-tracking.',
                'notrackselect' => 'Disables hot-tracking.',

                'tooltips' => 'Enables tooltips on items',
                'notooltips' => 'Disables tooltip on items.',

                'balloon' => 'Enables balloon styled tooltips. See note for further information.',
                'noballoon' => 'Disables balloon styled tooltips.',

                'infotip' => 'Enables modification of the tooltip text within the $xtreebar_callback. See note for further information.',
                'noinfotip' => 'Disables modification of the tooltip text.',

                'hasbuttons' => 'Enables &plusmn; buttons on the side of items containing child items for expand/collapse operations.',
                'nohasbuttons' => 'Disables &plusmn; buttons on the side of items containing child items for expand/collapse operations.',

                'rootlines' => 'Enables lines to be connected to the root item.',
                'norootlines' => 'Disables lines to be connected to the root item.',

                'singleexpand' => 'Causes the item being selected to expand and the item being unselected to collapse upon selection in the tree view.',
                'nosingleexpand' => 'Disables [s]singleexpand[/s].',

                'scroll' => 'Enables horizontal and vertical scrolling in the mIRC Treebar.',
                'noscroll' => 'Disables horizontal and vertical scrolling in the mIRC Treebar.',

                'showsel' => 'Selections will be shown when mIRC Treebar loses focus.',
                'noshowsel' => 'Selections will not be shown when mIRC Treebar loses focus.',

                'autohscroll' => 'Remove the horizontal scrollbar and auto-scroll depending on mouse position. [o]Vista[/o]',
                'noautohscroll' => 'Enables the horizontal scrollbar and auto-scroll depending on mouse position. [o]Vista[/o]',
                'doublebuffer' => 'Specifies how the background is erased or filled. [o]Vista[/o]',
                'nodoublebuffer' => 'Specifies how the background is erased or filled. [o]Vista[/o]',
                'fadebuttons' => 'Fade expand buttons in or out when the mouse moves away or into a state of hovering over the control. [o]Vista[/o]',
                'nofadebuttons' => 'Does not fade expand buttons in or out when the mouse moves away or into a state of hovering over the control. [o]Vista[/o]',
                'indent' => 'Indents the tree view for the expand buttons. [o]Vista[/o]',
                'noindent' => 'Does not indent the tree view for the expand buttons. [o]Vista[/o]',
                'richtooltip' => 'Allow rich tooltips in the tree view (custom drawn with icon and text). [o]Vista[/o]',
                'norichtooltip' => 'Disables rich tooltips in the tree view (custom drawn with icon and text). [o]Vista[/o]',
            ),
            '__notes' => array(
                'Using [s]noscroll[/s] while the scrollbar is visible may lead to graphical glitches.',
                'When using [s]infotip[/s], the alias [v]$xtreeview_callback(gettooltip, [TYPE], [TEXT])[/v] is called. Use return [r]NEW_TIP_TEXT[/r] to change the tooltip text, and return nothing to prevent the tip from being displayed.',
                '[s]balloon[/s] can only be used AFTER [s]tooltips[/s] has been enabled, in a seperate [v]/xtreebar[/v] command.',
            ),
        ),
        'T' => array(
            '__desc' => 'This command allows you to enable or disable DCX controlled drawing of the mIRC TreeBar.',
            '__cmd' => '[0|1]',
            '__eg' => '1',
            '__notes' => array(
                'When enabled, the script function [v]$xtreebar_callback(geticons, [TYPE], [TEXT])[/v] will be called for each item. This will allow you to configure the appearance of each item in the TreeBar.',
                'The same alias is called when an item is added to the TreeBar. This will allow you to configure the appearance of the new item.',
                'The [v]$xtreebar_callback[/v] alias should return [v]2[/v] image indexes, [p]IMAGE[/p] [p]SELECTEDIMAGE[/p]. (eg. [v]return 2 5[/v])',
            ),
        ),
        'w' => array(
            '__desc' => 'This command allows you to change the mIRC Treebar images.',
            '__cmd' => '[POS] [+FLAGS] [INDEX] [FILE]',
            '__eg' => array(
		        '0 + 1 icons.icl',
		        'clear',
		        'default',
	        ),
            '__params' => array(
                'clear' => 'Clears the icon list.',
                'default' => 'Reverts the icon list to the default icons.',
                'POS' => 'The position to add the new icon. Use [v]0[/v] to add to the end.',
                // + dcxLoadIcon flags for +FLAGS
            ),
            '__notes' => 'This requires [cmd]/xtreebar -T[/cmd] to be activated.',
        ),
    );
    
    writeDcxLoadIcon($XTREEBAR, 'w', '+FLAGS', 4);
}

function get_xtreebarprops(&$XDIDPROPS) {
    $XDIDPROPS = array(
        'icon' => array(
            '__desc' => 'Returns the current icons being used by the mIRC TreeBar item.',
            '__cmd' => 'N',
            '__eg' => '3',
            '__params' => array(
                'N' => 'The index of the item.',
            ),
            '__notes' => 'If [p]N[/p] is [v]0[/v], this property retrieves the last item in the mIRC Treebar.',
        ),
        'item' => array(
            '__desc' => 'Returns the text of the mIRC TreeBar item.',
            '__cmd' => 'N',
            '__eg' => '3',
            '__params' => array(
                'N' => 'The index of the item.',
            ),
            '__notes' => array(
                'If [p]N[/p] is [v]0[/v], this property retrieves the total number of items in the mIRC Treebar.',
                'Returns [v]$null[/v] if unsuccessful.',
            ),
        ),
    );
}
?>