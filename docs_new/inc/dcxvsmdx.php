<?php

function dcxvsmdx_layout($page, $pagelabel) {
	dcxdoc_print_description("Comparison Chart", dcx_vs_mdx());
}

function dcx_vs_mdx() {
	ob_start();
?>
<br />
<table class="tbl_dcxvsmdx">
	<tr>
		<th>Comparison</th>
		<th>DCX</th>
		<th>MDX</th>
	</tr>

	<tr>
		<td colspan="3" class="section_title section_bg">General dialog</td>
	</tr>

	<tr>
		<td class="comparison">Amount of added controls:</td>
		<td>34</td>
		<td>17</td>
	</tr>

	<tr>
		<td class="comparison">Create controls on the fly:</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
		<td>
			<img src="images/no.gif" alt="Does not have feature" /><br />
			(DynamicControl is useless as it cant receive events other than sclick)
		</td>
	</tr>

	<tr>
		<td class="comparison">Modify control Styles on the fly:</td>
		<td><img src="images/no.gif" alt="Does not have feature" /></td>
		<td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Modify borders/colours on the fly:</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Dialog Animations:</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
		<td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Command to minimize and maximize dialog:</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
		<td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Identify Visible Controls:</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Get a controls x y w h:</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Identify enabled/disabled controls:</td>
    	<td><img src="images/ok.gif" alt="Has feature" /></td>
    	<td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Move controls:</td>
        <td>
			<img src="images/ok.gif" alt="Has feature" /><img src="images/ok.gif" alt="Has feature" /><br />
			Using the Cell Layout Algorithm you can set sizing rules, works like a charm.
		</td>
		<td>
			<img src="images/ok.gif" alt="Has feature" /><br />
			Though it should really be said that MDX relies on crappy $calc coding and is eating a lot of CPU.</td>
	</tr>

	<tr>
		<td class="comparison">Change dialog icon:</td>
    	<td><img src="images/ok.gif" alt="Has feature" /></td>
    	<td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Change dialog background and borders:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Mark controls (add description):</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Drag &amp; Drop support:</td>
    	<td>
			<img src="images/ok.gif" alt="Has feature" /><br />
			Some controls
		</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">Calendar</td>
		<td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>


	<tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">Directshow</td>
		<td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">Stacker</td>
		<td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">WebCtrl</td>
        <td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">Rebar</td>
        <td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">Panel</td>
        <td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">Pager</td>
        <td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">DateTime Picker</td>
        <td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>
	<tr>
		<td class="section_title section_bg">Tab</td>
    	<td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
	    <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">RichEdit</td>
    	<td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
    	<td class="section_bg"><img src="images/ok.gif" alt="Has feature" /> Undocumented</td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">ColorCombo</td>
        <td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">Button</td>
        <td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">MultiCombo</td>
        <td class="section_bg"><img src="images/ok.gif" alt="Has feature" /></td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr>
		<td class="section_title section_bg">Stacker</td>
        <td class="section_bg"><img src="images/ok.gif" alt="Has feature" /> Incomplete</td>
        <td class="section_bg"><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

    <tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">TreeView</td></tr>

	<tr>
		<td class="comparison">Events:</td>
        <td>Has dclick and node expand/collapse which MDX lacks.</td>
        <td>&nbsp;</td>
	</tr>

	<tr>
		<td class="comparison">Auto Sorting:</td>
        <td>Using xdid, advanced auto sorting can be based on a lot of criteria.</td>
        <td>Only ascending alpha numerical sort using control style so this cant be changed after creation.</td>
	</tr>

	<tr>
		<td class="comparison">Per line colouring:</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Path determination method:</td>
		<td>Logical and simple. Path is part of the command which you read from right to left.</td>
        <td>Hard, uses a directory kind of structure in which you have to give browse commands before you can add items.</td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">Listview</td></tr>

	<tr>
		<td class="comparison">Control Styles:</td>
		<td>Sub items icons are supported.</td>
    	<td>Has a nosortheader which allows you to have a buttonless header and has a few more options to the hottrack style.</td>
	</tr>

	<tr>
		<td class="comparison">Sub item images:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Column icons</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Per line colouring of text and background colours:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Dropdowns and progressbars in an item:</td>
        <td>
			<img src="images/ok.gif" alt="Does not have feature" />
			<img src="images/no.gif" alt="Does not have feature" /><br />
			Has progress bars
		</td>
		<td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr>
		<td class="comparison">$xdid/$did information:</td>
        <td>Lacks information on the headerdims.</td>
        <td>&nbsp;</td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">Toolbar</td></tr>

	<tr>
		<td class="comparison">Events:</td>
        <td>Has rclick event, a dropdown event for arrows that also returns the suggested coordinates for a popup.</td>
        <td>&nbsp;</td>
	</tr>

	<tr>
		<td class="comparison">Control styles:</td>
        <td>Has a transparent style so the toolbars background is transparent.</td>
        <td>&nbsp;</td>
	</tr>

	<tr>
		<td class="comparison">Per item colouring + Bolding of text:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Autosizeitems to full width:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">Progressbar</td></tr>

	<tr>
		<td class="comparison">Events:</td>
        <td>Has several events (ie. sclick, rclick, mousebar)</td>
        <td>Has no events.</td>
	</tr>

	<tr>
		<td class="comparison">Styles:</td>
        <td>Has a marquee style MDX lacks.</td>
        <td>&nbsp;</td>
	</tr>

	<tr>
		<td class="comparison">Text in Progressbar:</td>
		<td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">ComboEx</td></tr>

	<tr>
		<td class="comparison">Events:</td>
        <td>Has a return event when Enter is pressed in the edit that MDX lacks.</td>
        <td>&nbsp;</td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">Trackbar:</td></tr>

	<tr>
		<td class="comparison">Events:</td>
        <td>Misses selection events as selection is not supported.</td>
        <td>&nbsp;</td>
	</tr>

	<tr>
		<td class="comparison">Selection:</td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">UpDown</td></tr>

	<tr>
		<td class="comparison">Events:</td>
        <td>Misses a up &amp; down event. This can be determined from sclick though.</td>
        <td>&nbsp;</td>
	</tr>

	<tr>
		<td class="comparison">Align Updown left or right:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Dock to a text control:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">IpAddress</td></tr>

	<tr>
		<td class="comparison">Set individual field ranges:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Set focus on a field:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">Docked @windows</td></tr>

	<tr>
		<td class="comparison">Events:</td>
        <td>Misses a whole bunch of mouse events that MDX has.</td>
        <td>&nbsp;</td>
	</tr>

	<tr>
		<td class="comparison">Dock more then 1 @window:</td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr>
		<td class="comparison">Capture mouse in @window:</td>
        <td><img src="images/no.gif" alt="Does not have feature" /></td>
        <td><img src="images/ok.gif" alt="Has feature" /></td>
	</tr>

	<tr><td colspan="3"></td></tr>

	<tr><td class="section_title section_bg" colspan="3">Divider</td></tr>

	<tr>
		<td class="comparison">Different Controls</td>
        <td colspan="2">The divider control in MDX and DCX differ so much all they really have in common is the name.</td>
	</tr>

	<tr>
		<td class="comparison">Functionality</td>
        <td>In DCX a divider is a moving bar that you "dock" 2 controls to. By moving this bar the docked controls are automatically sized. Its also possible to have a divider, in a divider, in a divider, etc.</td>
        <td>In MDX the divider is a sizing bar that you can move around and in combination with MoveControl and $calc you can adjust controls sizes.</td>
	</tr>
</table>
<?php
	return ob_get_clean();
}
?>
