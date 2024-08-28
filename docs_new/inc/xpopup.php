<?php

function xpopup_load($page) {
	global $XPOP, $XPOPPROPS, $XPOPUP, $XPOPUPPROPS;
	global $XDID, $XDIDPROPS, $EVENTS;

	loadSection($XPOPUP, "get_xpopup");
	loadSection($XPOPUPPROPS, "get_xpopupprops");
	loadSection($XPOP, "get_xpop");
	loadSection($XPOPPROPS, "get_xpopprops");
}

function xpopup_unload() {
	global $XPOP, $XPOPPROPS, $XPOPUP, $XPOPUPPROPS;

	$XPOPUP = array();
	$XPOPUPPROPS = array();
	$XPOP = array();
	$XPOPPROPS = array();
}

function xpopup_layout($page, $pagelabel) {
	global $SECTION, $XPOP, $XPOPPROPS, $XPOPUP, $XPOPUPPROPS;

	dcxdoc_print_intro($page);

        $SECTION = SECTION_GENERAL;
	dcxdoc_print_description("Special Menus", xpopup_special());
        dcxdoc_print_description('XPopup Item Path', xpopup_paths());

        $SECTION = SECTION_EVENTS;
        dcxdoc_print_description('XPopup Events', xpopup_events());

	// /xpopup commands
	if ($XPOPUP) {
        $SECTION = SECTION_XPOPUP;
        $count = 1;

        dcxdoc_print_description('/xpopup Command', "The /xpopup command is used to create/modify/destroy an XPopup menu.");

        foreach ($XPOPUP as $cmd => $data) {
       		dcxdoc_format_xpopup($cmd, $data, $count);
      		$count++;
		}
	}

	// $xpopup properties
	if ($XPOPUPPROPS) {
		$SECTION = SECTION_XPOPUPPROPS;
		$count = 1;

		dcxdoc_print_description('$xpopup() Properties', 'The $xpopup identifier is a given mIRC alias that communicates with the XPopup DLL to extract information from XPopup menus.');

		foreach ($XPOPUPPROPS as $prop => $data) {
	        dcxdoc_format_xpopupprop($prop, $data, $count);
	        $count++;
  		}
	}

	// /xpop commands
	if ($XPOP) {
        $SECTION = SECTION_XPOP;
        $count = 1;

        dcxdoc_print_description("/xpop Command", "The /xpop command is used to add/modify/remove menu items in XPopup menus.");

        foreach ($XPOP as $cmd => $data) {
       		dcxdoc_format_xpop($cmd, $data, $count);
      		$count++;
		}
	}

	// $xpop properties
	if ($XPOPPROPS) {
		$SECTION = SECTION_XPOPPROPS;
		$count = 1;

		dcxdoc_print_description('$xpop Identifier', 'The $xpop identifier is a given mIRC alias that communicates with the XPopup DLL to extract information in XPopup menu items.');

		foreach ($XPOPPROPS as $prop => $data) {
	        dcxdoc_format_xpopprops($prop, $data, $count);
	        $count++;
  		}
	}

	// DCXML for XPopup
	$SECTION = SECTION_GENERAL;
    dcxdoc_print_description('DCXML for XPopup', xpopup_dcxml());
}

function get_intro_xpopup() {
	echo "This extension lets you build nice stylish popup menus to be used everywhere in your script including coloring mIRC's own popup menus.";
}

function xpopup_special() {
	ob_start();
?>
There are two special reserved menu names in XPopup. They are for mIRC's regular popup menus [v]mirc[/v] (channel, statusbar, query and nicklist) and mIRC's menubar popup menu [v]mircbar[/v] (the menubar). Their appearance can be changed by XPopup and activated with the following command:<br />
<br />
<table>
	<tr>
		<td class="syntax">Syntax:</td>
		<td><div class="syntax">/mpopup [mirc|mircbar] [ENABLED]</div></td>
	</tr>
	<tr>
		<td class="example">Example:</td>
		<td><div class="example">/mpopup mirc 1</div></td>
	</tr>
</table>
<br />
Both menus can have their style, colors and icons modified using the /xpopup command. The command switches -c/-d are disabled for these menus. You cant use /xpop to modify these menus either. To add icons the mIRCs popups menus (status, menubar, channel, query and nicklist) you need to add the icons the menu image lists using /xpopup -i/-j. The icon index number needs to be inserted in the mIRC popup menu syntax like this:<br />
<br />
<table class="xpopupprop">
	<tr>
		<td class="syntax">Syntax:</td>
		<td><div class="syntax">[$style()] [ICON] [VTAB] [TEXT] (TAB) (Accelerator Text) : [mIRC Command]</div></td>
	</tr>
	<tr><td colspan="2"><b>Parameters:</b></td></tr>
	<tr>
		<td class="param">$style()</td>
		<td>mIRC's $style identifier.</td>
	</tr>
	<tr>
		<td class="param">ICON</td>
		<td>Icon index number. (You can use [v]0[/v] for no icon).</td>
	</tr>

	<tr>
		<td class="param">VTAB</td>
		<td>mIRC $chr(11).</td>
	</tr>
	<tr>
		<td class="param">TEXT</td>
		<td>Menu item text.</td>

	</tr>
	<tr>
		<td class="param">TAB</td>
		<td>mIRC $chr(9). (Optional)</td>
	</tr>
	<tr>
		<td class="param">Accelerator Text</td>
		<td>Accelerator Text (Eg. Alt+S)</td>
	</tr>
	<tr>
		<td class="param">COMMAND</td>
		<td>mIRC menu command.</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td colspan="2">[n]Accelerator text is usually a shortcut key combination to the same command seen in the menu that which the text is right aligned in the menu item like the CTRL-H seen in the image below.[/n]</td>
	</tr>
</table>
<br />
Remember that the same mIRC popup rules still apply. It is only the menu item text syntax that needs to be shaped for XPopup to interpret it correctly. If the [VTAB] element is not present, XPopup assumes that the item text is a regular item (no icon element).<br />
You can also add $idents in your menu item text like mIRC has always permitted to create your own tri-state check system for example with the use of custom icons.<br />
<br/>
The image below is an example of a mIRC menubar popup menu skinned with XPopup which includes icons.<br />
<br />
<div style="text-align: center;">
	<img src="images/xpopup3.png" alt="XPopup mIRC Colorful Iconed Menus" />
</div>
<?php
	//'
	return ob_get_clean();
}


function get_xpopup(&$XPOPUP) {
	$XPOPUP = array(
	    'b' => array(
	        '__desc' => 'This command lets you set the menu item background image in [s]custom[/s] or [s]custombig[/s] style.',
	        '__cmd' => '(IMAGEFILE)',
	        '__eg' => 'mc.bmp',
			'__notes' => array(
			    'Please take note that the image is stretched to fit the menu item rectangle.',
			    'Only BMP images are supported through this command if GDI+ is not available.',
			    'If [p]IMAGEFILE[/p] is not specified, then the current image will be removed.',
				'In [s]custombig[/s] style the image is stretched over the whole menu not the individual items.',
			),
		),
		'c' => array(
	        '__desc' => 'This command lets you create an XPopup empty menu.',
	        '__cmd' => '[STYLE] ($chr(9) (CALLBACK))',
	        '__eg' => 'office2003rev',
	        '__params' => array(
                'STYLE' => array(
                    '__desc' => "Image flags.",
                    '__values' => array(
						'office2003' => 'Office 2003 menu style.',
						'office2003rev' => 'Office 2003 Reverse menu style.',
						'officexp' => 'Office XP menu style.',
						'icy' => 'Icy menu style.',
						'icyrev' => 'Icy Reverse menu style.',
						'grade' => 'Grade menu style.',
						'graderev' => 'Grade Reverse menu style.',
						'normal' => 'Normal Plain menu style.',
						'vertical' => 'Vertically Gradient menu style.',
						'verticalrev' => 'Vertically Gradient Reverse menu style.',
						'custom' => 'Lets you apply a custom background item image (see [cmd]/xpopup -b[/cmd]).',
						'custombig' => 'Lets you apply a custom background image (see [cmd]/xpopup -b[/cmd]).',
						'button' => 'Items look like buttons (raised).',
						'buttonrev' => 'Items look like buttons (sunken).',
					),
				),
				'CALLBACK' => 'Callback alias to use with +C flagged items.',
			),
	        '__notes' => array(
				'This command will fail if the menu already exists, use [prop]$xpopup().ismenu[/prop] to check if the menu exists.',
				'Some colors are not used in certain menu styles.',
			)
		),
		'd' => 'This command lets you destroy a XPopup menu.',
		'i' => array(
	        '__desc' => 'This command lets you add one or more icons to the menu image list.',
	        '__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
	        '__eg' => array(
				'+g 113 shell32.dll',
				'+ 1,13,20-26 shell32.dll',
			),
	        '__params' => array(
	        	// +FLAGS
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => "Use 0 if the file is a single icon file.",
		),
		'j' => 'This command lets you clear the menu image list.',
		'l' => array(
            '__desc' => 'This command lets you set the Nth color in the menu as given by the order listed in [cmd]/xpopup -p[/cmd].',
	        '__cmd' => '[N] [COLOR]',
	        '__eg' => '2 $rgb(255,0,0)',
            '__notes' => 'You can specify [v]default[/v] in [p]COLOR[/p] to reset it to the default value.',
		),
        'm' => array(
            '__desc' => 'This command patches mIRC v6.20, allowing XPopup to work.',
            '__notes' => array(
				'THIS ONLY WORKED ON MIRC v6.20, AND NO LONGER DOES ANYTHING AS THIS DLL IS MIRC V7+',
                'After patching, mIRC will crash if /dll -u is used.',
                'This will only apply to mIRC menus, so use [v]mirc[/v] for [p]MENU[/p].'
            ),
        ),
		'M' => array(
            '__desc' => 'This command lets you set the marked text, which can contain any information.',
	        '__cmd' => '(TEXT)',
	        '__eg' => 'additional information',
		),
		'p' => array(
	        '__desc' => 'This command lets you set the whole menu color palette in one command where [p]COLORS[/p] is a space separated list of RGB colors.',
	        '__cmd' => '[COLOR1] [COLOR2] [COLOR...] [COLOR10]',
	        '__eg' => '$rgb(255,0,0) $rgb(0,0,255) ... $rgb(0,255,0)',
                '__params' => array(
                    'COLORS' => array(
						'__desc' => 'The colors in $rgb() format must be in this order:',
						'__values' => array(
							'1' => 'Menu background color',
							'2' => 'Icon box color',
							'3' => 'Checkbox color',
							'4' => 'Disabled checkbox color',
							'5' => 'Disabled selection box color',
							'6' => 'Disabled text color',
							'7' => 'Selection box color',
							'8' => 'Selection box border color',
							'9' => 'Separator line color',
							'10' => 'Text color',
							'11' => 'Selected text color',
						),
					),
				),
	        '__notes' => array(
				"If you are not using the [s]checkbox[/s] style, this function can also change the state icon of an item. Be sure to insert the state icons before you use them.",
				'You can specify [v]default[/v] in [p]COLORS[/p] to reset it to the default value.',
			),
		),
		'R' => array(
			'__desc' => 'This command lets you add more visual styles to your XPopup',
			'__cmd' => '[+FLAG] [ARGS]',
			'__eg' => array(
				'+r 1',
				'+a 150',
				'+t 1',
				'+R 1',
			),
            '__params' => array(
				'MENU' => 'Name of the Xpopup you wish to style.',
				'+FLAGS' => array(
					'__desc' => 'Menu display style flags.',
					'__values' => array(
						'r' => 'This gives the menu selection indicator a rounded look.',
						'R' => 'This gives the menu itself a rounded look.',
						'a' => 'This sets the menu alpha value.',
						't' => 'This enables/disables displaying tooltips for menu items.',
					)
                ),
				'__args' => array(
					'r' => array(
						'__cmd' => '[1|0]',
					),
					'R' => array(
						'__cmd' => '[1|0]',
					),
					'a' => array(
						'__cmd' => '[VALUE]',
						'__params' => array(
						'VALUE' => 'The opacity to apply. Values range from [v]0[/v] (transparent) to [v]255[/v] (opaque).',
						),
					),
					't' => array(
						'__cmd' => '[1|0]',
					),
				),
			),
		),
		's' => array(
			'__desc' => 'This command lets you display a menu.',
			'__cmd' => '[+FLAGS] [X] [Y] (HWND)',
			'__eg' => '+ 100 100',
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Menu display style flags.',
					'__values' => array(
						'b' => 'The menu is bottom aligned according to the Y value.',
						'c' => 'The menu is center aligned according to the X value.',
						'l' => 'The menu is left aligned according to the X value.',
						'm' => 'The menu items are clickable only with the left mouse button. ([v]default[/v])',
						'n' => 'The menu items are clickable only with the right mouse button.',
						'r' => 'The menu is right aligned according to the X value.',
						't' => 'The menu is top aligned according to the Y value.',
						'v' => 'The menu is vertically center aligned according to the Y valued.',
					)
				),
				'X' => 'X coordinate where menu will popup in screen coordinates.',
				'Y' => 'Y coordinate where menu will popup in screen coordinates.',
				'HWND' => 'The popup coordinates will be displayed relative to this window.',
			),
		),
		't' => array(
	        '__desc' => 'This command lets you change the menu style.',
	        '__cmd' => '[STYLE]',
	        '__eg' => 'officexp',
		),
		'x' => array(
	        '__desc' => 'This command lets you set the menu extended styles.',
	        '__cmd' => '[+FLAGS]',
	        '__eg' => '+dp',
			'__params' => array(
                '+FLAGS' => array(
                    '__desc' => 'Flags to set the extended styles for the popup.',
                    '__values' => array(
						'd' => 'Disabled items display a selection box.',
						'i' => 'Icons have a 3D shadow effect when menu item is selected.',
						'p' => 'Icons have a plain 3D effect when menu item is selected.',
				    ),
				),
			),
		),
	);

	writeDcxLoadIcon($XPOPUP, 'i', '+FLAGS', 1);
}

function get_xpopupprops(&$XPOPUPPROPS) {
	$XPOPUPPROPS = array(
		"ismenu" => 'This property retrieves if a menu exists.',
		'isPatched' => 'This determines if mIRC 6.20 has been patched for special menus to work.',
		'isrounded' => 'Returns wether the rounded style is enabled (see [cmd]xpopup -R[/cmd])',
		'isroundedmenu' => 'Returns wether the rounded menu style is enabled (see [cmd]xpopup -R[/cmd])',
		'istooltips' => 'Returns wether tooltips are enabled (see [cmd]xpopup -R[/cmd])',
		'alpha' => 'Returns the current alpha level of the menu (see [cmd]xpopup -R[/cmd])',
		"style" => 'This property retrieves the menu style.',
		"alpha" => 'This property retrieves the transparency value.',
		'marked' => 'This property lets you get the marked text.',
		'imagefile' => 'This property lets you get the background image filename.',
		"isrounded" => 'Is the rounded style enabled?',
		"exstyle" => 'This property retrieves the menu extended styles.',
		"colors" => 'This property retrieves the menu colors as one line (see [cmd]xpopup -p[/cmd] for the list).',
		"color" => array(
			'__desc' => "This property retrieves Nth menu color (see [cmd]xpopup -p[/cmd] for the list).",
			'__cmd' => 'N',
			'__eg' => '3',
		),
		"menuname" => array(
			'__desc' => "This property retrieves the name of the Nth menu.",
			'__cmd' => 'N',
			'__eg' => '3',
		),
	);
}


function get_xpop(&$XPOP) {
	$XPOP = array(
	    'a' => array(
	        '__desc' => 'This command lets you add a menu item.',
	        '__cmd' => '[TAB] [+FLAGS] [ID] [ICON] (TEXT) ([TAB] [TOOLTIP])',
	        '__eg' => '$chr(9) + 1 1 Menu Item 1 $chr(9) Tooltip!',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => 'Menu item flags.',
                    '__values' => array(
						'c' => 'Item is checked.',
						'C' => 'Item calls the saved callback alias on selection.',
						'g' => 'Item is disabled/grayed.',
						's' => 'An empty submenu is created for the item ready for menu items.',
					),
				),
                'ID' => "Menu item ID as returned on the event handler.",
                'ICON' => 'Menu item icon index. (Use [v]0[/v] for no icon)',
				'TEXT' => 'Menu item text. It can contain mIRC $identifiers that will be evaluated the first time the menu is being displayed.',
				'TOOLTIP' => 'Menu items tooltip.',
			),
			'__notes' => array(
			    'If the item text is meant to be dynamic, you will need to reset the text using [cmd]/xpop -t[/cmd]',
			    '[p]ID[/p] must be unique.',
				'A menu item that has a checkmark AND an icon, it displays the icon instead of the checkmark.',
				'Remember to use mIRCs $eval($idents, 0) or $!idents to prevent the mIRC evaluation of the $idents that your text contains.',
				'The [TOOLTIP] is only displayed if enabled for this menu via [cmd]/xpopup -R[/cmd]',
				'Tooltips are only displayed for menu items, NOT submenus',
			),
		),
		'c' => array(
	        '__desc' => 'This command lets you create a submenu on a menu item.',
	        '__notes' => 'If the item already has a submenu, it is destroyed. This can be used to clear a submenu instead of deleting the whole menu item.'
		),
		'd' => 'This command lets you destroy a submenu on a menu item.',
		'f' => 'This command lets you delete a menu item.',
		'i' => array(
	        '__desc' => 'This command lets you change the icon on a menu item.',
	        '__cmd' => '[TAB] [ICON]',
	        '__eg' => '$chr(9) 3',
		),
		's' => array(
	        '__desc' => 'This command lets you change the state flags on an item (see [cmd]/xpop -a[/cmd] for the switches)',
	        '__cmd' => '[TAB] [+FLAGS]',
	        '__eg' => '$chr(9) +cg',
		),
		't' => array(
	        '__desc' => 'This command lets you change a menu item text.',
	        '__cmd' => '[TAB] [TEXT]',
	        '__eg' => '$chr(9) $eval($me, 0)',
	        '__notes' => 'Remember to use mIRCs $eval(expression,0) to prevent the mIRC evaluation of the $idents that your text contains.',
		),
		'T' => array(
	        '__desc' => 'This command lets you add/change a menu items tooltip.',
	        '__cmd' => '[TAB] [TEXT]',
	        '__eg' => '$chr(9) This is a tooltip!',
		),
	);
}

function get_xpopprops(&$XPOPPROPS) {
	$XPOPPROPS = array(
		"num" => array(
		    '__desc' => "This property retrieves the number of menu items in a submenu.",
	        '__notes' => '[p]PATH[/p] can be [v]root[/v] to find the number of items on the root menu.'
		),
		"text" => 'This property retrieves the menu item text.',
		"tooltip" => 'This property retrieves the menu items tooltip text (if any).',
		"icon" => 'This property retrieves the menu item icon index number.',
		"enabled" => 'This property retrieves if a menu item is enabled or disabled.',
		"submenu" => 'This property retrieves if a menu item has a submenu or not.',
	);
}


function xpopup_events() {
	ob_start();
?>
Events are sent through mIRCs &quot;ON SIGNAL&quot; event with the signal name being xpopup-[MENU] where [MENU] is the menu name that sent the event. The $1 parameter is the supplied [ID] of the menu item. This is why it's important that this number be unique for all menu items. A returned value of 0 means no selection was made in the menu.<br />
<br />
An example of the XPopup signal handler code is shown below:<br />

[code]
ON *:SIGNAL:Xpopup-*: {
        ;// get the menu name from Xpopup-[MENUNAME]
        var %m $gettok($signal, 2-, 45)

        ;// nothing clicked
        if ($1 == 0) {
                return
        }

        if (%m == menu_open) {
                if ($1 == 1) { echo -a command 1 }
                else if ($1 == 2) { echo -a command 2 }
        }
        else if (%m == menu_help) {
                if ($1 == 1) { url http://dcx.scriptsdb.org }
                else if ($1 == 2) { echo -a command aboutbox }
        }
        else {
                echo -a unknown menu
        }
}
[/code]
<?php
	//'
	return ob_get_clean();
}


function xpopup_paths() {
	ob_start();
?>The XPopup menus have an engine that parses the given item "path" data to know where to insert/delete/modify a given item in the menu structure. The parameters you supply are a space separated numerical token list of the menu submenus that lead to the menu item position:<br />
<br/>

<table>
	<tr>
		<td class="syntax">Format:</td>
		<td><div class="syntax">N N N ... N</div></td>
	</tr>
	<tr>
		<td class="example">Example:</td>
		<td><div class="example">2 3</div></td>
	</tr>
</table>
<br />
This position represents the 3rd menu item of the of the second menu item submenu as shown by the selected item in the picture below:<br />
<br />
<div style="text-align: center;"><img src="images/xpopup2.png" alt="XPopup Item Path Example" /></div>

[n][/n]
<ul>
	<li>If you specify a path that does not exist, parsing halts and an error is displayed.</li>
	<li>You can use [v]0[/v] for the last value when inserting a menu item to insert it at the end of the menu.</li>
</ul>
<?php
	return ob_get_clean();
}

function xpopup_dcxml() {
	ob_start();
?>
	DCXML also allows users to create menus. The syntax and format is similar to the XML used for creating dialogs, but simpler.<br />
	<br />
	<table>
		<tr>
			<td class="syntax">Format:</td>
			<td><div class="syntax">/dcxml -p [MENUNAME] [DATASET] [FILENAME]</div></td>
		</tr>
		<tr>
			<td class="example">Example:</td>
			<td><div class="example">/dcxml -p menuName LagBarMenu $qt($mircdir $+ dc.xml)</div></td>
		</tr>
	</table>
	<br />
	In the given example, a menu named [v]menuName[/v] will be created from the dataset specified by the name [v]LagBarMenu[/v].<br />
	<br />
	An example of the DCXML format without XPopup support is given below:<br />
	<pre class="dcxml">
	&lt;dcxml&gt;
		&lt;dialogs&gt;
			...
		&lt;/dialogs&gt;
	&lt;/dcxml&gt;
	</pre>
	<br />
	DCXML support for XPopup is similar, as shown below:<br />
	<pre class="dcxml">
	&lt;dcxml&gt;
		&lt;dialogs&gt;
			...
		&lt;/dialogs&gt;

		&lt;popups&gt;
			&lt;popup name="LagBarMenu"&gt;
				...
			&lt;/popup&gt;

			&lt;popup name="ToolbarDropdownMenu"&gt;
				...
			&lt;/popup&gt;

			&lt;popup name="PreferencesMenu"&gt;
				...
			&lt;/popup&gt;
		&lt;/popups&gt;
	&lt;/dcxml&gt;
	</pre>
	<br />
	Global styles can be used to allow for a consistant look and feel for your menus. However, a style specified in a popup tag will override global style.<br />
	<pre class="dcxml">
	&lt;dcxml&gt;
		&lt;popups&gt;
			&lt;styles&gt;
				&lt;all style="vertical" /&gt;
			&lt;/styles&gt;

			&lt;popup name="LagBarMenu"&gt;
				...
			&lt;/popup&gt;

			&lt;popup name="ToolbarDropdownMenu"&gt;
				...
			&lt;/popup&gt;
		&lt;/popups&gt;
	&lt;/dcxml&gt;
	</pre>
	<br />
	It is now much easier to add items (and submenus) to your menu.<br />
	<pre class="dcxml">
	&lt;dcxml&gt;
		&lt;popups&gt;
			&lt;popup name="LagBarMenu"&gt;
				&lt;item id="1" caption="Check lag" /&gt;
				&lt;item caption="-" /&gt;
				&lt;item caption="Lagbar styles"&gt;
					&lt;item id="3" caption="Line" /&gt;
					&lt;item id="4" caption="Bars" /&gt;
				&lt;/item&gt;
				&lt;item caption="This item does nothing and has no icon" /&gt;
			&lt;/popup&gt;
		&lt;/popups&gt;
	&lt;/dcxml&gt;
	</pre>
	<br />
	And lastly, icon support. The same rules apply from [link section="xpopup" flag="i"]/xpopup -i[/link].<br />
	<pre class="dcxml">
	&lt;dcxml&gt;
		&lt;popups&gt;
			&lt;popup name="LagBarMenu"&gt;
				&lt;icons&gt;
					&lt;icon index="1" src="$mircdir $+ popup_icons.icl" /&gt;
					&lt;icon index="3" src="$mircdir $+ popup_icons.icl" /&gt;
					&lt;icon index="5" src="$mircdir $+ popup_icons.icl" /&gt;
				&lt;/icons&gt;

				&lt;item id="1" icon="1" caption="Check lag" /&gt;
				&lt;item caption="-" /&gt;
				&lt;item caption="Lagbar styles"&gt;
					&lt;item id="3" icon="2" caption="Line" /&gt;
					&lt;item id="4" icon="3" caption="Bars" /&gt;
				&lt;/item&gt;
				&lt;item caption="This item does nothing and has no icon" /&gt;
			&lt;/popup&gt;
		&lt;/popups&gt;
	&lt;/dcxml&gt;
	</pre>
	<br />
	Below is a list of attributes and which tag they apply to.<br />
	[n]All attributes which apply to the [v]popup[/v] tag will also apply to the global [v]all[/v] tag.[/n]<br />
	<br />
	<table class="attrs">
		<tr><th colspan="2">style</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;popup&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>The XPopup style.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>office2003</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>See [link section="xpopup" flag="c"]/xpopup -c[/link] for a complete listing of styles.</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;popup style=&quot;grade&quot;&gt;</td>
		</tr>
	</table>

	<br />
	<table class="attrs">
		<tr><th colspan="2">bgcolour, iconcolour, cbcolour, discbcolour, disselcolour, distextcolour, selcolour, selbordercolour, seperatorcolour, textcolour <span style="color: black;">&amp;/or</span> seltextcolour</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;popup&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>Sets a specific color for the menu.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>Not applicable.</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>A numerical long value or mIRC formatted [v]$rgb(R, G, B)[/v]. See [link section="xpopup" flag="p"]/xpopup -p[/link] for more details.</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;popup bgcolour=&quot;$rgb(0, 0, 255)&quot; textcolour=&quot;255&quot; seperatorcolour=&quot;$rgb(0, 255, 0)&quot;&gt;</td>
		</tr>
	</table>

	<br />
	<table class="attrs">
		<tr><th colspan="2">background</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;popup&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>Sets the background for a menu with the [s]custom[/s] style.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>Nothing.</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>A full path to the image. See [link section="xpopup" flag="b"]/xpopup -b[/link] for more details.</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;popup background=&quot;$mircdir $+ gfx\menu_bg.png&quot;&gt;</td>
		</tr>
	</table>

	<br />
	<table class="attrs">
		<tr><th colspan="2">flags</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;icon&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>Options to load the icon with.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>Not applicable.</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>See [link section="xpopup" flag="i"]/xpopup -i[/link] for more details.</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;icon flags=&quot;+g&quot; /&gt;</td>
		</tr>
	</table>

	<br />
	<table class="attrs">
		<tr><th colspan="2">index</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;icon&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>The icon index in the file specified.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>Not applicable.</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>An integer. See [link section="xpopup" flag="i"]/xpopup -i[/link] for more details.<br />
The only difference is that you can load multiple icons by seperating the indexes with commas.
			</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;icon index=&quot;1,3,4&quot; /&gt;</td>
		</tr>
	</table>

	<br />
	<table class="attrs">
		<tr><th colspan="2">src</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;icon&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>The icon file.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>Not applicable.</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>A full path to the icon. See [link section="xpopup" flag="i"]/xpopup -i[/link] for more details.</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;icon index=&quot;2&quot; src=&quot;$mircdir $+ gfx\menu_icons.icl&quot; /&gt;</td>
		</tr>
	</table>

	<br />
	<table class="attrs">
		<tr><th colspan="2">caption</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;item&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>The menu item text.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>Not applicable.</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>Any label. If the label is [v]-[/v], then the item is a seperator.</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;item caption=&quot;Menu Item Text&quot; /&gt;<br />
&lt;item caption=&quot;-&quot; /&gt;</td>
		</tr>
	</table>

	<br />
	<table class="attrs">
		<tr><th colspan="2">id</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;item&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>The menu item ID.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>0</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>A positive integer.</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;item id=&quot;13&quot; caption=&quot;Menu Item Text&quot; /&gt;</td>
		</tr>
	</table>

	<br />
	<table class="attrs">
		<tr><th colspan="2">icon</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;item&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>The icon to use for the menu item.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>-1</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>An index which corresponds to an icon loaded with &lt;icon&gt;.</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;item icon=&quot;5&quot; caption=&quot;Menu with Icon&quot; /&gt;</td>
		</tr>
	</table>

	<br />
	<table class="attrs">
		<tr><th colspan="2">state</th></tr>
		<tr>
			<td class="left">Applies to:</td>
			<td>&lt;item&gt;</td>
		</tr>
		<tr>
			<td class="left">Description:</td>
			<td>The menu item states, such as checked or grayed.</td>
		</tr>
		<tr>
			<td class="left">Default value:</td>
			<td>None.</td>
		</tr>
		<tr>
			<td class="left">Valid values:</td>
			<td>See [link section="xpop" flag="a"]/xpop -a[/link] for more details.</td>
		</tr>
		<tr>
			<td class="left">Example:</td>
			<td>&lt;item state=&quot;+c&quot; /&gt;</td>
		</tr>
	</table>
<?php
	return ob_get_clean();
}

?>