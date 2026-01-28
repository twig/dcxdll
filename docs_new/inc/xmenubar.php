<?php

function xmenubar_load($page) {
    global $XMENUBAR, $XMENUBARPROPS;

    loadSection($XMENUBAR, "get_xmenubar");
    loadSection($XMENUBARPROPS, "get_xmenubarprops");
}

function xmenubar_unload() {
    global $XMENUBAR, $XMENUBARPROPS;

    $XMENUBAR = array();
    $XMENUBARPROPS = array();
}

function get_intro_xmenubar() {
    echo "XMenuBar is a command implemented to replace the mIRC menubar using XPopup menus.";
}

function xmenubar_layout($page, $pagelabel) {
    global $SECTION, $XMENUBAR, $XMENUBARPROPS;

    dcxdoc_print_intro($page);

    $SECTION = SECTION_GENERAL;
    dcxdoc_print_description('XMenuBar Marking and Callback', xmenubar_markingAndCallback());
    dcxdoc_print_description('Special Menus and Menu IDs', xmenubar_specialMenuIDs());

    // /xmenubar commands
    if ($XMENUBAR) {
        $SECTION = SECTION_XMENUBAR;
        $count = 1;

        dcxdoc_print_description('/xmenubar Commands', "The /xmenubar command is used to replace the mIRC menubar.");

        foreach ($XMENUBAR as $cmd => $data) {
            dcxdoc_format_xmenubar($cmd, $data, $count);
            $count++;
	}
    }

    // $xmenubar commands
    if ($XMENUBARPROPS) {
        $SECTION = SECTION_XMENUBARPROPS;
        $count = 1;

        dcxdoc_print_description('$xmenubar Properties', 'The $xmenubar identifier is used to retrieve information about the DCX XMenuBar implementation.');

        foreach ($XMENUBARPROPS as $cmd => $data) {
       	    dcxdoc_format_xmenubarprops($cmd, $data, $count);
      	    $count++;
	}
    }
}

function xmenubar_markingAndCallback() {
    ob_start();
?>Just like with XDialog, you must "mark" the mIRC menubar prior to using XMenuBar.<br />
To mark the mIRC menubar, use [f]/xmenubar -M[/f] and specify a callback function of your choice.<br />
<br />
The callback function will only be passed 1 parameter, and that is the [p]MENU_ID[/p]. The menu ID is sent to the callback function to allow customisation of event flow. To stop mIRC from processing the menu command, return [v]$true[/v] from the callback alias.<br/>
<br />
An example of a simple callback function would be:<br />
[code]
alias xmenubar_cb {
    ;// Prevent user from accessing Help -&gt; Contents
    if ($1 == 190) return $true
}
[/code]
<?php
    return ob_get_clean();
}

function xmenubar_specialMenuIDs() {
    $SPECIAL_IDs = array(
        'File' => array(
            '101' => 'Connect',
            '102' => 'Disconnect',
            '104' => 'Select Server (Alt+E)',
            '105' => 'Exit',
        ),
        'View' => array(
            '110' => 'Menubar',
            '111' => 'Toolbar',
            '112' => 'Switchbar',
            '210' => 'Treebar',
            '131' => 'Notify List (Alt+N)',
            '132' => 'Urls List (Alt+U)',
            '133' => 'Colors (Alt+K)',
            '134' => 'Font',
        ),
        'Favorites' => array(
            '141' => 'Add to Favorites',
            '142' => 'Organize Favorites (Alt+J)',
        ),
        'Tools' => array(
            '150' => 'Channels List (Alt+L)',
            '203' => 'Scripts Editor (Alt+R)',
            '151' => 'Send File (Alt+S)',
            '152' => 'Chat (Alt+C)',
            '154' => 'Received Files',
            '155' => 'Log Files',
            '156' => 'Address Book (Alt+B)',
            '157' => 'Online Timer (Alt+I)',
            '159' => 'Options (Alt+O)',
        ),
        'Window' => array(
            '170' => 'Tile Horizontal',
            '171' => 'Tile Vertical',
            '172' => 'Cascade',
            '173' => 'Arrange Icons',
            '174' => 'Group',
            '175' => 'Order',
            '176' => 'Close (Alt+Z)',
        ),
        'Help' => array(
            '190' => 'Contents (F1)',
            '191' => 'Search (Shift+F1)',
            '192' => 'Latest News',
            '193' => 'Message Board',
            '194' => 'About mIRC',
            '195' => 'Register',
        ),
    );

    ob_start();
?>The callback parses menu clicks before mIRC gets a chance to process them. If the custom XPopupMenu has an ID which matches the ID of an existing mIRC menu item, it can be used to call the mIRC functionality.<br />
For example, the menu ID for the mIRC Options dialog is [v]159[/v]. If you used [f]/xpop -a[/f] and specified the [p]ID[/p] [v]159[/v] and added the XPopup menu to the XMenuBar, the item will open up the mIRC Options dialog when clicked.<br />
<br />
Additionally, there is a special menu called the [v]scriptpopup[/v] menu. This allows you to add the original mIRC scriptable popup menu on your XMenuBar. The usage of the other special menus ([v]mirc[/v] and [v]mircbar[/v]) has been disabled for the XMenuBar.<br />
<br />
[n]For custom menu commands which are specific to only your script, it is advised you use an ID starting from [v]15000[/v]. This is to prevent any clashes in existing IDs and XPopup menu items.[/n]<br />
<br />
<b>Please do not create confusing or misleading menus!</b><br />
<br />
Below is a list of special menu IDs for those wishing to redesign mIRC's menubar.<br />
[n]The list was compiled from mIRC v6.31[/n].<br />
<br />
<table class="xmenubarSpecialIDs">
<?php
    foreach ($SPECIAL_IDs as $menu => $items) {
        echo "<tr><th colspan='2'>$menu</th></tr>\n";
        echo "<tr class='heading'><td>Special ID</td><td>Description</td></tr>\n";

        foreach ($items as $id => $desc) {
            echo "<tr><td>$id</td><td>$desc</td></tr>\n";
        }
    }
?>
</table>
<?php
    return ob_get_clean();
}

function get_xmenubar(&$XMENUBAR) {
    $XMENUBAR = array(
        'a' => array(
            '__desc' => 'This command adds a menu to the XMenuBar.',
            '__cmd' => '[MENU] [LABEL]',
            '__eg' => array(
                'xpMenuName DCX Menu',
                'scriptpopup Custom'
            ),
            '__params' => array(
                'MENU' => 'This is be either the name of the XPopup menu or [v]scriptpopup[/v].',
                'LABEL' => 'The label to display for this menu on the XMenuBar.',
            ),
        ),
        'd' => array(
            '__desc' => 'This command removes a menu from the XMenuBar.',
            '__cmd' => '[MENU]',
            '__eg' => 'xpMenuName',
        ),
        'g' => 'This command replaces the mIRC menubar with an empty XMenuBar.',
        'l' => array(
            '__desc' => 'This command allows you to change the XMenuBar label for a specific menu.',
            '__cmd' => '[MENU] [NEW_LABEL]',
            '__eg' => 'xpMenuName Something Else',
        ),
        'M' => array(
            '__desc' => 'This command marks the menubar and defines a callback function.',
            '__cmd' => '(CALLBACK)',
            '__eg' => 'xmenubar_cb',
            '__notes' => 'If [p]CALLBACK[/p] is not specified, this will reset the mIRC menubar.',
        ),
        'r' => 'This command resets the original mIRC menubar.',
        's' => array(
            '__desc' => 'This command simulates a menu item click by sending the menu item ID to be processed.',
            '__cmd' => '[MENU_ID]',
            '__eg' => '159',
            '__notes' => 'This should only be used to access mIRCs original menu functionality if you have replaced it.',
        ),
        'p' => array(
            '__desc' => 'This command allows additional settings to be changed.',
            '__cmd' => '[+FLAGS] [ARGS]',
            '__eg' => array(
                '+r 1',
                '+itR 1 $rgb(255,203,0)',
            ),
            '__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Item flags.',
					'__values' => array(
						'e' => 'Enable/disable',
						'r' => 'Enable/disable rounded borders',
						'O' => 'Enable/disable drawing borders',
						'f' => 'Load background image (bmp format only atm)',
						's' => 'Enable/disable shadow text',
						'v' => 'Visible/invisible menubar (works with custom or standard menubars)',
						'S' => 'menubar style',

						'i' => 'Item specific. combines with flags [f]+ftTHbBhwWo[/f] to affect a specific item.',
						'R' => 'Redraw menubar. (can be combined with any flags, or used by its self)',

						't' => 'Text colour.',
						'T' => 'Selected text colour.',
                        'H' => 'Hot text colour.',
						'b' => 'Background colour',
						'B' => 'Selected background colour',
						'h' => 'Hot background colour',
						'w' => 'Border colour.',
						'W' => 'Selected border colour.',
						'o' => 'Hot border colour.',
					),
				),
				'ARGS' => array(
					'__desc' => 'Arguments dependant on [p]+FLAGS[/p]',
					'__values' => array(
						'e,r,O,s,v' => '1 or 0',
                        'f' => 'path/filename.bmp or if combined with [f]+i[/f] [ITEM INDEX] path/filename.bmp',
						'S' => 'stylename',
						'i' => 'adds [ITEM INDEX] to the start of combined flags args.',
                        'R' => 'no args',
                        't,T,H,b,B,h,w,W,o' => '[RGB]',
					),
				),
            ),
            '__notes' => 'fun',
        ),
    );
}

function get_xmenubarprops(&$XDIDPROPS) {
    $XDIDPROPS = array(
        'menu' => array(
            '__desc' => 'Returns the name of the menu added to the XMenuBar.',
            '__cmd' => 'N',
            '__eg' => '3',
            '__params' => array(
                'N' => 'The index of the menu.',
            ),
            '__notes' => 'If [p]N[/p] is [v]0[/v], this property retrieves the total number of menus in the XMenuBar.',
        ),
    );
}
?>