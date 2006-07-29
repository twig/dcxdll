<?php
function get_intro_cla() {
	echo "The Cell Layout Algorithm is a dynamic autosizing algorithm aimed at making child controls of dialogs or container windows fully resizeable with a set of predefined rules. The rules are encapsulated in a container-like objects called [v]cells[/v]. People with knowledge of HTML table structures will recognize the type of operations at stake.";
}

function cla_details() {
	ob_start();
?>
The types of cells are enumerated below with their characteristics.<br />
<br />
<table class="tbl_cla_details">
	<tr>
		<th>Type</th>
		<th>Child</th>
		<th>Control</th>
		<th>Description</th>
	</tr>

	<tr>
		<td class="type">[s]Fixed[/s]</td>
		<td>No</td>
		<td>Yes</td>
		<td class="desc">This cell has a fixed [p]Width[/p] or [p]Height[/p] or [p]Both[/p]. It can host a child control that will be expanded in the direction where it is not fixed to fill the available length. It CANNOT hold child [v]cell[/v] structures.</td>
	</tr>

	<tr>
		<td class="type">[s]Fill[/s]</td>
		<td>No</td>
		<td>Yes</td>
		<td class="desc">This cell is expanded in all directions to fill the available space. It can host a child control. It CANNOT hold child [v]cell[/v] structures.</td>
	</tr>

	<tr>
		<td class="type">[s]Pane[/s]</td>
		<td>Yes</td>
		<td>No</td>
		<td class="desc">This cell control hosts a collection of stacked [p]horizontal[/p] or [p]vertical[/p] cell structures. They CANNOT host child controls as these must be hosted by their hosted cell structures. Its child cell structures are added with an associated [p]weight[/p] (which can be [v]0[/v]), where the its child cell structures will receive space determined by the total weight proportional factor. For example, if a two cell structures are added with a weight of [v]1[/v] each, then each will receive [v]50%[/v] of the available space.</td>
	</tr>
</table>
<br />
The root control is always set to the maximum client area of the dialog minus its padding values.
<?
	return ob_get_clean();
}




function cla_visual() {
	ob_start();
?>
The following image illustrates a layout of cells which is explained lower.<br />
<br />
<div style="text-align: center;"><img src="images/layout.png" alt="Cell Layout Algorithm Example" /></div>
<br />
Here is the cell layout pattern of the precedent image with their paths
in parenthesis (used in the /xdialog or /xdid commands) :

<ul>
	<li><font color="red"><b>Pane - Horizontal (root)</b></font></li>
	<li>
		<ul>
			<li><font color="blue"><b>Fixed - Width - Weight = 0 (1)</b></font></li>
			<li><font color="blue"><b>Pane - Vertical - Weight = 1 (2)</b></font></li>
			<li>
				<ul>
					<li><font color="green"><b>Fill - Weight = 1 (2 1)</b></font></li>
					<li><font color="green"><b>Fill - Weight = 2 (2 2)</b></font></li>
				</ul>
			</li>
		</ul>
	</li>
</ul>
<br />
As we can see clearly, the top right fill cell occupies one third (1 / (1 + 2)) of the available space and the bottom right fill cell occupies the two thirds (2 / (1 + 2)).<br />
<br />
Here is another example that illsutrates the usage of fill cells to achieve centering.<br />
<br/>
<div style="text-align: center;"><img src="images/layout2.png" alt="Cell Layout Algorithm Example" /></div>
<br />
The image is again represented by the following layout pattern with their paths in parenthesis (used in the /xdialog or /xdid commands):

<ul>
	<li><font color="blue"><b>Pane - Horizontal (root)</b></font></li>
	<li>
		<ul>
			<li><font color="red"><b>Fill - Weight = 1 (1)</b></font></li>
			<li><font color="red"><b>Pane - Vertical - Weight = 8 (2)</b></font></li>
			<li>
				<ul>
					<li><font color="green"><b>Fill - Weight = 1 (2 1)</b></font></li>
					<li><font color="green"><b>Fill - Weight = 8 (2 2)</b></font></li>
					<li><font color="green"><b>Fill - Weight = 1 (2 3)</b></font></li>
				</ul>
			</li>
			<li><font color="red"><b>Fill - Weight = 1 (3)</b></font></li>
		</ul>
	</li>
</ul>
<br />
This layout has the control always centered in the window. It also is always exactly 80% in width and height (8 / 10).
<?
	return ob_get_clean();
}


function cla_examples() {
	ob_start();
?>If the DCX documentation is unsufficient for you to start off using CLA (most likely it would have confused you like hell!), here are a few links you can visit for further reading.<br />
<br />
<ul>
	<li><a href="http://dcx.scriptsdb.org/forum/showthread.php?tid=120">DCX Forum: Help: CLA</a></li>
	<li><a href="http://dcx.scriptsdb.org/forum/showthread.php?tid=127">DCX Forum: CLA :(</a></li>
	<li><a href="http://www.mircscripts.org/comments.php?cid=2977">Mpdreamz's Cell Layout Algorithm Tutorial</a></li><?php //'; ?>
	<li><a href="http://dcx.scriptsdb.org/forum/showthread.php?tid=320">EinsteinXXL's CLA Tutorial (German)</a></li>
</ul>
<br />
Feel free to post any messages after reading those threads if you still need any help.
<?
	//'
	return ob_get_clean();
}

?>
