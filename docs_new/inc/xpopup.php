<?php
function get_intro_xpopup() {
	echo "This extension lets you build nice stylish popup menus to be used everywhere in your script including coloring mIRC's own popup menus.";
}

function xpopup_special() {
	ob_start();
?>
There are two XPopup special menus. They are mIRCs regular popup menus (channel, statusbar, query and nicklist) and mIRCs menubar popup menus which include (menubar). The use the reserved menu names [s]mirc[/s] and [s]mircbar[/s]. They can be activated with the following command:<br />
<br />
<table>
	<tr>
		<td class="syntax">Syntax:</td>
		<td><div class="syntax">/mpopup [mirc|mircbar] [ENABLED]</div></td>
	</tr>
	<tr>
		<td class="example">Example:</td>
		<td><div class="example">/xpopup -g mymenu +n 50 50 $mircdir/images/blah.png</div></td>
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
		<td colspan="2"><a class="note">Note.</a> Accelerator text is usually a shortcut key combination to the same command seen in the menu that which the text is right aligned in the menu item like the CTRL-H seen in the image below.</td>
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
<?
	//'
	return ob_get_clean();
}


function get_xpopup(&$XPOPUP) {
	$XPOPUP = array(
	    'b' => array(
	        '__desc' => 'This command lets you set the menu item background image in [s]custom[/s] style.',
	        '__cmd' => '[IMAGEFILE]',
	        '__eg' => 'mc.bmp',
			'__notes' => array(
			    'Please take note that the image is stretched to fit the menu item rectangle.',
			    'Only BMP images are supported through this command.',
			),
		),
		'c' => array(
	        '__desc' => 'This command lets you create an XPopup empty menu.',
	        '__cmd' => '[STYLE]',
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
						'custom' => 'Lets you apply a custom background item image (see /xpopup -b).',
					),
				),
			),
	        '__notes' => array(
				'This command will fail if the menu already exists, use $xpopup().ismenu to check if the menu exists.',
				'Some colors are not used in certain menu styles.',
			)
		),
		'd' => 'This command lets you destroy an XPopup empty menu.',
		'i' => array(
	        '__desc' => 'This command lets you add an icon to the menu image list.',
	        '__cmd' => '[INDEX] [FILENAME]',
	        '__eg' => '113 C:/mIRC/shell.dll',
	        '__params' => array(
	            'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
	        '__notes' => "Use 0 if the file is a single icon file.",
		),
		'j' => 'This command lets you clear the menu image list.',
		'l' => array(
	        '__desc' => 'This command lets you set the Nth color in the menu as given by the order listed in /xpopup -p.',
	        '__cmd' => '[N] [COLOR]',
	        '__eg' => '2 $rgb(255,0,0)',
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
					),
				),
			),
	        '__notes' => "If you are not using the [s]checkbox[/s] style, this function can also change the state icon of an item. Be sure to insert the state icons before you use them.",
		),
		's' => array(
	        '__desc' => 'This command lets you display a menu.',
	        '__cmd' => '[+FLAGS] [X] [Y]',
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
					'd' => 'Disabled items display a selection box.',
					'i' => 'Icons have a 3D shadow effect when menu item is selected.',
					'p' => 'Icons have a plain 3D effect when menu item is selected.',
				),
			),
		),
	);
}

function get_xpopupprops(&$XPOPUPPROPS) {
	$XPOPUPPROPS = array(
		"ismenu" => 'This property retrieves if a menu exists.',
		"style" => 'This property retrieves the menu style.',
		"exstyle" => 'This property retrieves the menu extended styles.',
		"colors" => 'This property retrieves the menu colors as one line (see /xpopup -p for the list).',
		"color" => array(
		    '__desc' => "This property retrieves Nth menu color (see /xpopup -p for the list).",
		    '__cmd' => 'N',
	        '__eg' => '3',
		),
	);
}


function get_xpop(&$XPOP) {
	$XPOP = array(
	    'a' => array(
	        '__desc' => 'This command lets you add a menu item.',
	        '__cmd' => '[TAB] [+FLAGS] [ID] [ICON] (TEXT)',
	        '__eg' => '$chr(9) + 1 1 Menu Item 1',
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => 'Menu item flags.',
                    '__values' => array(
						'c' => 'Item is checked.',
						'g' => 'Item is disabled/grayed.',
						's' => 'An empty submenu is created for the item ready for menu items.',
					),
				),
                'ID' => "Menu item ID as returned on the event handler.",
                'ICON' => 'Menu item icon index. (Use [v]0[/v] for no icon)',
				'TEXT' => 'Menu item text. It can contain mIRC $identifiers that will be evaluated the first time the menu is being displayed.',
			),
			'__notes' => array(
			    'If the item text is meant to be dynamic, you will need to reset the text using /xpop -t',
			    '[p]ID[/p] must be unique.',
				'A menu item that has a checkmark AND an icon, it displays the icon instead of the checkmark.',
				'Remember to use mIRCs $eval($idents, 0) or $!idents to prevent the mIRC evaluation of the $idents that your text contains.',
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
	        '__desc' => 'This command lets you change the state flags on an item (see /xpop -a for the switches)',
	        '__cmd' => '[TAB] [+FLAGS]',
	        '__eg' => '$chr(9) +cg',
		),
		't' => array(
	        '__desc' => 'This command lets you change a menu item text.',
	        '__cmd' => '[TAB] [TEXT]',
	        '__eg' => '$chr(9) $eval($me, 0)',
	        '__notes' => 'Remember to use mIRCs $eval(expression,0) to prevent the mIRC evaluation of the $idents that your text contains.',
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

<pre>ON *:SIGNAL:Xpopup-*: {
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
	else {
	    echo -a unknown menu
	}
}</pre>
<?
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

<a class="note">Note.</a>
<ul>
	<li>If you specify a path that does not exist, parsing halts and an error is displayed.</li>
	<li>You can use [v]0[/v] for the last value when inserting a menu item to insert it at the end of the menu.</li>
</ul>
<?
	return ob_get_clean();
}

?>
