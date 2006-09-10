<?php
function get_intro_dialog() {
?>This control enables you to dock a dialog as a control on another dialog. The dialog must be opened before any attempt to add it to the dialog.<br />
<br />
You can dock any number of dialogs as long as each is a separate entity.<br />
<br />
[n]The docked dialogs are ejected when the main dialog is closed, or the control is deleted.[/n]
<?php
}


function get_xdidprops_dialog(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"dname" => "This property lets you retreive the name of the dialog which has been docked.",
	);
}


function get_events_dialog(&$EVENTS) {
	$EVENTS = array(
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
