<?
header("Cache-Control: no-cache, must-revalidate");	// HTTP/1.1
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");	// Date in the past

$archive = 'archive';

// version => date released
$versions = array(
	'1.3.4' => 1150614000,
	'1.3.3' => 1149420240,
	'1.3.2' => 1149180480,
	'1.3.1' => 1147423920,
	'1.3.0' => 1145517420,
);

//echo "hello downloader " . time();
$version = isset($_GET['v']) ? $_GET['v'] : '';

// if a version is specified
if ($version) {
	header("Location: $archive/dcx_v$version.zip");
	exit();
}

echo "<table border=1>";
echo "<tr><th>Version</th><th>Release Date</th><th>Link</th><th>Change Log</th></tr>";

foreach ($versions as $version => $datereleased) {
	echo "<tr>
	<td>v$version</td>
	<td>" . date("dS F, Y", $datereleased) . "</td>
 	<td>";

	if (file_exists("$archive/dcx_v$version.zip"))
		echo "<a href=\"{$_SERVER['PHP_SELF']}?v=$version\">Download</a>";
	else
	    echo "(Missing)";

	echo "</td>
	<td><a href=\"changes.htm#v$version\">Changes</a></td>
	</tr>";
}

echo "</table>";

?>
