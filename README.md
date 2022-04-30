**dcxdll**
======

**D**ialog **C**ontrol e**X**tensions for mIRC

Support forum: None...
Pre-Built dll's: [https://github.com/twig/dcxdll/releases](https://github.com/twig/dcxdll/releases "Pre-Built binaries")

This is DCX version 3.1+ & requires mIRC v7.45+

How to compile

1: Install Microsoft Visual Studio 2022, make sure you install the latest SDK as well as all the C++ desktop stuff

2: Clone the git for dcxdll

3: Clone the git for Detours [link](https://github.com/microsoft/Detours.git "Detours GitHub")

4: Clone the git for GSL [link](https://github.com/microsoft/GSL.git "GSL GitHub")

5: Install mIRC somewhere for debugging the dll, this should be a special install just for debugging.

6: Install PHP NTS build [link](https://windows.php.net/download/ "PHP download link") this is only needed to build the docs.

7: Edit dcx.props & change

	<mIRCDir> - to point to the previously mentioned mIRC install
	
	<PHPDir> - to point to the PHP install
	
	<DetoursDir> - to point to the Detours dir
	
	<GSLDir> - to point to the GSL dir
	
8: Compile Detours libs

9: Compile DCX
