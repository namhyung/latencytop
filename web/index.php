<? 
include("topbar.php");
?>

<table>
<tr><td valign=top width=190>
<table>
<tr><td class=subsection>GENERAL</td></tr>
<tr><td class='nav_box_active'><a class='nav' href='/index.php'>Info</a></td></tr>
<tr><td class='nav_box'><a class='nav' href='announce.php'>Announcement</a></td></tr> 
<tr><td class='nav_box'><a class='nav' href='download.php'>Download</a></td></tr> 
<tr><td class='nav_box'><a class='nav' href='git.php'>GIT Repository</a></td></tr> 
</table>
</td>
<td>
<h1>About LatencyTOP</h1>

Skipping audio, slower servers, everyone knows the symptoms of latency. But
to know what's going on in the system, what's causing the latency, how to
fix it... that's a hard question without good answers right now.

<br><br>

LatencyTOP is a Linux* tool for software developers (both kernel and
userspace), aimed at identifying where in the system latency is happening,
and what kind of operation/action is causing the latency to happen so that
the code can be changed to avoid the worst latency hiccups.

<br>
<h3>About latency</h3>

There are many types and causes of latency. LatencyTOP focuses on the type
of latency that causes skips in audio, stutters in your desktop experience
or that overloads your server (while you have plenty of CPU power left).

<br><br>

LatencyTOP focuses on the cases where the applications want to run and
execute useful code, but there's some resource that's not currently
available (and the kernel then blocks the process). This is done both on a
system level and on a per process level, so that you can see what's
happening to the system, and which process is suffering and/or causing the
delays.

<br><br>
<img src="latencytop.png">


</td></tr>
</table>
<hr>
Intel is a trademark of Intel Corporation in the U.S. and other countries. | * Other names / brands may be claimed as the property of others 
</html>