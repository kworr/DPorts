
$FreeBSD: head/editors/p5-Padre/files/patch-lib_Padre_Wx.pm 340725 2014-01-22 17:40:44Z mat $

--- lib/Padre/Wx.pm.orig
+++ lib/Padre/Wx.pm
@@ -67,6 +67,7 @@
 
 sub launch_browser {
 	require Padre::Task::LaunchDefaultBrowser;
+	Wx::LaunchDefaultBrowser( $_[0] ); return 1;
 	Padre::Task::LaunchDefaultBrowser->new(
 		url => $_[0],
 	)->schedule;
