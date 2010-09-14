<?php

function xdock_load($page) {
	global $XDOCK, $XDOCKPROPS;

	loadSection($XDOCK, "get_xdock");
	loadSection($XDOCKPROPS, "get_xdockprops");
}

function xdock_unload() {
	global $XDOCK, $XDOCKPROPS;
	
	$XDOCK = array();
	$XDOCKPROPS = array();
}

function get_intro_xdock() {
	echo "XDock is the complete rewrite of UltraDock. It handles docking of all types of windows and customisation of behaviour upon resizing.";
}

function xdock_layout($page, $pagelabel) {
	global $SECTION, $XDOCK, $XDOCKPROPS;
	
	dcxdoc_print_intro($page);
	
	// /xdock commands
	if ($XDOCK) {
        $SECTION = SECTION_XDOCK;
        $count = 1;

        dcxdoc_print_description('/xdock Commands', "The /xdock command is used to force the positioning of a window within another window, with specific behaviour.");

        foreach ($XDOCK as $cmd => $data) {
       		dcxdoc_format_xdock($cmd, $data, $count);
      		$count++;
		}
	}
	
	// $xdock commands
	if ($XDOCKPROPS) {
        $SECTION = SECTION_XDOCKPROPS;
        $count = 1;

        dcxdoc_print_description('$xdock Properties', 'The $xdock identifier is used to retrieve docking information about windows and various mIRC components.');

        foreach ($XDOCKPROPS as $cmd => $data) {
       		dcxdoc_format_xdockprops($cmd, $data, $count);
      		$count++;
		}
	}
	
	// xdock signals
	$SECTION = SECTION_EVENTS;
	dcxdoc_print_description('XDock Signals', xdock_events());
}

function get_xdock(&$XDOCK) {
	$XDOCK = array(
		'b' => array(
			'__desc' => 'This command docks a window to the mIRC TreeBar.',
			'__cmd' => '[HWND_DOCK] [+FLAGS]',
			'__eg' => '$dialog(dcx).hwnd +h',
			'__params' => array(
			    'HWND_DOCK' => 'The HWND of the window you wish to dock.',
				'+FLAGS' => array(
					'__desc' => 'Flags for docking into the mIRC TreeBar.',
					'__values' => array(
						's' => 'Docked window will be resized to fit its parent width & height automatically.',
						'h' => 'Docked window will be resized to fit its parent width automatically.',
						'v' => 'Docked window will be resized to fit its parent height automatically.',
						'n' => 'Docked window will <strong>not</strong> be resized automatically.',
						'o' => 'Uses the old style treebar docking. This is when the docked window is on top of the treeview control within treebar.',
					),
				),
			),
                        '__notes' => 'Dialogs should not be docked within the INIT event as they are not yet visible. This can be fixed by using a timer.',
		),
		'c' => array(
			'__desc' => 'This command docks a window to a custom window (channel, query, custom, or status windows).',
			'__cmd' => '[HWND_DOCK] [+FLAGS] [DESTINATION]',
			'__eg' => '$dialog(dcx).hwnd +h $query(ClickHeRe).hwnd',
			'__params' => array(
			    'HWND_DOCK' => 'The HWND of the window you wish to dock.',
				'+FLAGS' => array(
					'__desc' => 'Flags for docking into a custom window.',
					'__values' => array(
						's' => 'Docked window will be resized to fit its parent width & height automatically.',
						'h' => 'Docked window will be resized to fit its parent width automatically.',
						'v' => 'Docked window will be resized to fit its parent height automatically.',
						'n' => 'Docked window will <strong>not</strong> be resized automatically.',
					),
				),
				'DESTINATION' => 'The HWND of the window that you wish to dock your window into.',
			),
                        '__notes' => 'Dialogs should not be docked within the INIT event as they are not yet visible. This can be fixed by using a timer.',
		),
                'm' => array(
			'__desc' => 'This command docks a window to your mIRC window.',
			'__cmd' => '[HWND_DOCK] [+FLAGS]',
			'__eg' => '$dialog(dcx).hwnd +r',
			'__params' => array(
			    'HWND_DOCK' => 'The HWND of the window you wish to dock.',
				'+FLAGS' => array(
					'__desc' => 'Flags for docking into a mIRC window.',
					'__values' => array(
						't' => 'Docks the window to the top.',
						'b' => 'Docks the window to the bottom.',
						'l' => 'Docks the window to the left.',
						'r' => 'Docks the window to the right.',
					),
				),
			),
                        '__notes' => 'Dialogs should not be docked within the INIT event as they are not yet visible. This can be fixed by using a timer.',
		),
		'M' => array(
			'__desc' => 'This command shows/hides the Menubar.',
			'__cmd' => '[1|0]',
			'__eg' => '0',
		),
		'n' => array(
			'__desc' => 'This command docks a window to a nicklist listbox.',
			'__cmd' => '[HWND_DOCK] [+FLAGS] [DESTINATION]',
			'__eg' => '$dialog(dcx).hwnd +h $channel(#help).hwnd',
			'__params' => array(
			    'HWND_DOCK' => 'The HWND of the window you wish to dock.',
				'+FLAGS' => array(
					'__desc' => 'Flags for docking into a custom window.',
					'__values' => array(
						'b' => 'Docked window\'s parent will not have any scrollbars',
						's' => 'Docked window will be resized to fit its parent width & height automatically.',
						'h' => 'Docked window will be resized to fit its parent width automatically.',
						'v' => 'Docked window will be resized to fit its parent height automatically.',
						'n' => 'Docked window will <strong>not</strong> be resized automatically.',
						'b' => 'Disable the nicklist scrollbar.',
					),
				),
				'DESTINATION' => 'The HWND of the channel window that you wish to dock your window into.',
			),
                        '__notes' => 'Dialogs should not be docked within the INIT event as they are not yet visible. This can be fixed by using a timer.',
		),
		'p' => 'This command forces mIRC to update its docked windows.',
		'r' => array(
			'__desc' => 'This command resizes a docked window.',
			'__cmd' => '[HWND_DOCK] [+FLAGS] [WIDTH] [HEIGHT]',
			'__eg' => '$dialog(dcx).hwnd + 250 3000',
			'__params' => array(
			    'HWND_DOCK' => 'The HWND of the window you wish to resize.',
				'+FLAGS' => array(
					'__desc' => 'There are currently no flags.',
					'__values' => array(),
				),
				'WIDTH' => 'The width that the window will be set to.',
				'HEIGHT' => 'The height that the window will be set to.',
			),
			'__notes' => array(
				'If the window is docked in a top or bottom position, then [p]WIDTH[/p] will be ignored.',
				'If the window is docked in a left or right position, then [p]HEIGHT[/p] will be ignored.',
			),
		),
		'R' => array(
			'__desc' => 'This command shows/hides the Treebar.',
			'__cmd' => '[1|0]',
			'__eg' => '0',
		),
		's' => array(
			'__desc' => 'This command docks a window to the mIRC SwitchBar.',
			'__cmd' => '[HWND_DOCK] [+FLAGS]',
			'__eg' => '$dialog(dcx).hwnd +v',
			'__params' => array(
			    'HWND_DOCK' => 'The HWND of the window you wish to dock.',
				'+FLAGS' => array(
					'__desc' => 'Flags for docking into the mIRC SwitchBar.',
					'__values' => array(
						's' => 'Docked window will be resized to fit its parent width & height automatically.',
						'h' => 'Docked window will be resized to fit its parent width automatically.',
						'v' => 'Docked window will be resized to fit its parent height automatically.',
						'n' => 'Docked window will <strong>not</strong> be resized automatically.',
					),
				),
			),
                        '__notes' => 'Dialogs should not be docked within the INIT event as they are not yet visible. This can be fixed by using a timer.',
		),
		'S' => array(
			'__desc' => 'This command shows/hides the Switchbar.',
			'__cmd' => '[1|0]',
			'__eg' => '0',
		),
		't' => array(
			'__desc' => 'This command docks a window to the mIRC ToolBar.',
			'__cmd' => '[HWND_DOCK] [+FLAGS]',
			'__eg' => '$dialog(dcx).hwnd +v',
			'__params' => array(
			    'HWND_DOCK' => 'The HWND of the window you wish to dock.',
				'+FLAGS' => array(
					'__desc' => 'Flags for docking into the mIRC ToolBar.',
					'__values' => array(
						's' => 'Docked window will be resized to fit its parent width & height automatically.',
						'h' => 'Docked window will be resized to fit its parent width automatically.',
						'v' => 'Docked window will be resized to fit its parent height automatically.',
						'n' => 'Docked window will <strong>not</strong> be resized automatically.',
					),
				),
			),
                        '__notes' => 'Dialogs should not be docked within the INIT event as they are not yet visible. This can be fixed by using a timer.',
		),
		'T' => array(
			'__desc' => 'This command shows/hides the Toolbar.',
			'__cmd' => '[1|0]',
			'__eg' => '0',
		),
		'u' => array(
			'__desc' => 'This command undocks a window.',
			'__cmd' => '[HWND_DOCK] [+FLAGS]',
			'__eg' => '$dialog(dcx).hwnd +',
			'__params' => array(
			    'HWND_DOCK' => 'The HWND of the window you wish to undock.',
			),
			'__notes' => 'There are currently no flags, but the [v]+[/v] character is required for DCX prior to v1.3.8',
		),
	);
}

function get_xdockprops(&$XDIDPROPS) {
	$XDIDPROPS = array(
		'dockSide' => array(
			'__desc' => 'Returns the side which the dialog is docked.',
			'__params' => array(
				'SIDE' => array(
					'__desc' => 'Possible return values.',
					'__values' => array(
						'top'    => 'The window is docked in the top position.',
						'bottom' => 'The window is docked in the bottom position.',
						'left'   => 'The window is docked in the left position.',
						'right'  => 'The window is docked in the right position.',
					),
				),
			),
		),
		'isDocked' => 'Determines if a window/dialog is docked.',
		
		'hasDocked' => 'Determines if the specified window/dialog has a docked window/dialog within it.',
		
		'isAutoH' => 'Returns [v]$true[/v] if the docked window automatically resizes horizontally.',
		'isAutoV' => 'Returns [v]$true[/v] if the docked window automatically resizes vertically.',
		'isAutoS' => 'Returns [v]$true[/v] if the docked window automatically resizes both horizontally and vertically.',

		'isSwitchBar' => 'Determines if the mIRC SwitchBar is visible.',
		'isToolBar' => 'Determines if the mIRC ToolBar is visible.',
		'isMenuBar' => 'Determines if the mIRC MenuBar is visible.',
		'isTreeBar' => 'Determines if the mIRC TreeBar is visible.',
		
		'switchBarSize' => array(
			'__desc' => 'Returns the mIRC SwitchBar size.',
			'__params' => array(
				'DIMENSIONS' => array(
					'__desc' => 'Return values.',
					'__values' => array(
						'X' => 'The X-offset of the mIRC SwitchBar',
						'Y' => 'The Y-offset of the mIRC SwitchBar',
						'W' => 'The width of the mIRC SwitchBar.',
						'H' => 'The height of the mIRC SwitchBar.',
					),
				),
			),
		),
		'switchBarPos' => array(
		        '__desc' => 'Returns the mIRC SwitchBar dock position.',
			'__params' => array(
				'POSITION' => array(
					'__desc' => 'Return values.',
					'__values' => array(
						'none'   => 'The SwitchBar is not shown in mIRC.',
						'top'    => 'The SwitchBar is docked in the top position.',
						'bottom' => 'The SwitchBar is docked in the bottom position.',
						'left'   => 'The SwitchBar is docked in the left position.',
						'right'  => 'The SwitchBar is docked in the right position.',
					),
				),
			),
		),
		'toolBarSize' => array(
			'__desc' => 'Returns the mIRC ToolBar size.',
			'__params' => array(
				'DIMENSIONS' => array(
					'__desc' => 'Return values.',
					'__values' => array(
						'X' => 'The X-offset of the mIRC ToolBar',
						'Y' => 'The Y-offset of the mIRC ToolBar',
						'W' => 'The width of the mIRC ToolBar.',
						'H' => 'The height of the mIRC ToolBar.',
					),
				),
			),
		),
	    	'toolBarPos' => array(
			'__desc' => 'Returns the mIRC ToolBar position.',
			'__params' => array(
				'POSITION' => array(
					'__desc' => 'Return values.',
					'__values' => array(
						'none'   => 'The ToolBar is not shown in mIRC.',
						'top'    => 'The ToolBar is in the top position.',
						'bottom' => 'The ToolBar is in the bottom position.',
						'left'   => 'The ToolBar is in the left position.',
						'right'  => 'The ToolBar is in the right position.',
					),
				),
			),
		),
		'treeBarSize' => array(
		        '__desc' => 'Returns the mIRC TreeBar size.',
			'__params' => array(
				'DIMENSIONS' => array(
					'__desc' => 'Return values.',
					'__values' => array(
						'X' => 'The X-offset of the mIRC TreeBar',
						'Y' => 'The Y-offset of the mIRC TreeBar',
						'W' => 'The width of the mIRC TreeBar.',
						'H' => 'The height of the mIRC TreeBar.',
					),
				),
			),
		),
		'treeBarPos' => array(
		        '__desc' => 'Returns the mIRC TreeBar position.',
			'__params' => array(
				'POSITION' => array(
					'__desc' => 'Return values.',
					'__values' => array(
						'none'   => 'The TreeBar is not shown in mIRC.',
						'top'    => 'The TreeBar is in the top position.',
						'bottom' => 'The TreeBar is in the bottom position.',
						'left'   => 'The TreeBar is in the left position.',
						'right'  => 'The TreeBar is in the right position.',
					),
				),
			),
		),
		'text' => 'Returns the title text for the given HWND.',
	);
}

function xdock_events() {
	global $SECTION;
	
	$color = get_section_color($SECTION);

	$data = array(
		'__params' => array(
		    'size' => 'The event signal identifier.',
			'WINDOW_TYPE' => array(
				'__desc' => 'Information about which window type this information is describing.',
				'__values' => array(
					'mIRC' => 'The mIRC window has resized.',
					'mIRC_Switchbar' => 'The mIRC SwitchBar has resized.',
					'mIRC_Toolbar' => 'The mIRC ToolBar has resized.',
					'mIRC_TreeList' => 'The mIRC TreeList has resized.',
					'ListBox' => 'The channel ListBox has been resized.',
					'Custom' => 'A custom window has been resized.',
				),
			),
			'HWND' => 'The hwnd of the window which resized.',
			'W' => 'The new width of the window.',
			'H' => 'The new height of the window.',
		),
		'__notes' => 'For [v]ListBox[/v] and [v]Custom[/v], the [p]HWND[/p] will be the HWND of the window which triggered the event. For the other events, the HWND will be for the main mIRC window.',
	);
	
	ob_start();
?>
<table class="<?php echo get_section_name($SECTION); ?>">
	<tr><td colspan="2">Signals are sent to mIRC containing information about components which have resized. To activate signal messages for docked windows, refer to [f]/dcx xSignal[/f]</td></tr>
	<tr>
		<td class="syntax">Signal:</td>
		<td><div class="syntax">DCX size WINDOW_TYPE HWND W H</div></td>
	</tr>
	<tr>
		<td class="example">Example:</td>
		<td><div class="example">DCX size mIRC_Toolbar 345564 1024 32</div></td>
	</tr>
<?php

	// parameters
	 format_parameters($data['__params']);

	// notes
	format_notes($data['__notes']);
?>
</table>
<?php
	return ob_get_clean();
}
?>
