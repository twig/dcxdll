<?php
function get_intro_dialog() {
?>This control enables you to dock a dialog as a control on another dialog. The dialog must be opened before any attempt to add it to the dialog.<br />
<br />
You can dock any number of dialogs as long as each is a separate entity.<br />
<br />
Please take note that the dialog controls are ejected when the main dialog is closed.
<?php
}


function get_events_dialog(&$EVENTS) {
	$EVENTS = array(
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
