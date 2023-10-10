<?php

function howtocompile_layout($page, $pagelabel) {
	global $SECTION;
	
	dcxdoc_print_intro($page);
	
	$SECTION = SECTION_GENERAL;
	dcxdoc_print_description("How To Compile Details", howtocompile_details());
}

function get_intro_howtocompile() {
	?>
	<table><tr><th>To compile DCX you need:</th></tr>
	<tr style="text-indent: 40px"><td>Visual Studio 2019 16.11.14+, recommended is VisualStudio 2022 v17.6.5+</td></tr>
	<tr style="text-indent: 40px"><td>Full latest windows desktop SDK installed.</td></tr>
    <tr style="text-indent: 40px"><td>DCX Source. <a href='https://github.com/twig/dcxdll'>https://github.com/twig/dcxdll</a></td></tr>
    <tr style="text-indent: 40px"><td>GSL installed. <a href='https://github.com/microsoft/GSL.git'>https://github.com/microsoft/GSL.git</a></td></tr>
	<tr style="text-indent: 40px"><td>PHP v7+ installed, recommended PHP nts 8.2.11+ (only for the docs) <a href='https://www.php.net'>https://www.php.net/</a></td></tr>
	<tr style="text-indent: 40px"><td>Git for windows installed. (This is needed to auto create gitBuild.h)</td></tr>
	<tr style="text-indent: 40px"><td>mIRC v7+ installed, recommended v7.75+. (only for debugging) <a href='https://www.mirc.co.uk/'>https://www.mirc.co.uk/</a></td></tr>
	<tr><td><strong>Optional:</strong></td></tr>
	<tr style="text-indent: 40px"><td>Detours installed. Only if you don't wish to use the supplied libs. <a href='https://github.com/microsoft/Detours.git'>https://github.com/microsoft/Detours.git</a></td></tr>
	<tr style="text-indent: 40px"><td>PCRE2 installed. Only if you don't wish to use the supplied libs. <a href='https://github.com/PhilipHazel/pcre2.git'>https://github.com/PhilipHazel/pcre2.git</a></td></tr>
	</table>
<?php
}

function howtocompile_details() {
	ob_start();
?>
First you will need to edit the file dcx.props<br />
Near the top you will see <code>&lt;PropertyGroup Label="UserMacros"&gt;</code> this group will contain the entries:
<table class="tbl_tag_details">
	<tr>
		<th>Tag</th>
		<th>Description</th>
	</tr>

    <tr>
	  <td class="tag"><code>&lt;mIRCDir&gt;C:\mIRC_Basic&lt;/mIRCDir&gt;</code></td>
	  <td class="desc">Path to the mIRC executable to use for debugging, this should be a stand alone install of mIRC, not your main install.</td>
	</tr>
    <tr>
	  <td class="tag"><code>&lt;PHPDir&gt;C:\php&lt;/PHPDir&gt;</code></td>
	  <td class="desc">Path to the php.exe</td>
	</tr>
    <tr>
	  <td class="tag"><code>&lt;DetoursDir&gt;$(SolutionDir)Detours\&lt;/DetoursDir&gt;</code></td>
	  <td class="desc">Path to the Detours files. Only change this if you decide to compile Detours yourself.</td>
	</tr>
    <tr>
	  <td class="tag"><code>&lt;GSLDir&gt;F:\CSource\GSL-Master\GSL\include&lt;/GSLDir&gt;</code></td>
	  <td class="desc">Path to the GSL files.</td>
	</tr>
    <tr>
	  <td class="tag"><code>&lt;PCRE2Dir&gt;$(SolutionDir)pcre2\&lt;/PCRE2Dir&gt;</code></td>
	  <td class="desc">Path to PCRE2 compiled libs. Only change this if you decide to compile PCRE2 yourself.</td>
	</tr>
    <tr>
	  <td class="tag"><code>&lt;DirectXTKDir&gt;F:\CSource\DirectXTK\&lt;/DirectXTKDir&gt;</code></td>
	  <td class="desc">Path to DirectXTK files. (not used atm)</td>
	</tr>
</table>
<br />
These entries must be changed to match the system you are using.<br />
Before compiling DCX you must first have Detours compiled, You will need the 32bit debug & 32bit release builds.
After these have been edited you can load the solution file and compile.<br />
When compiling you should see the "SourceControl" project run before the DCX project, this creates gitBuild.h<br />
You should also see the project "DxErr" run before DCX, this creates the DxErr.lib file needed by DCX.<br />

<br />
<?php
	return ob_get_clean();
}
?>
