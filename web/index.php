<? 
include("topbar.php");
?>

<h1>About LatencyTOP</h1>

Skipping audio, slower servers, everyone knows the symptoms. But to know
what's going on in the system, what's causing it, how to fix it... that's a
hard question without good answers right now.

<br><br>

LatencyTOP is a Linux tool for software developers (both kernel and
userspace), aimed at identifying where in the system latency is happening,
and what kind of operation/action is causing the latency to happen so that
the code can be changed to avoid the worst latency hickups.

<br>
<h3>About latency</h3>

There are many types and causes of latency; LatencyTOP focuses on the type
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

<h3>Download</h3>
<table>
<tr class="odd"><th>File</th><th>Link</th></tr>
<tr><td>userspace tool</td><td><a class="plainlink" href="download/latencytop-0.1.tar.gz">latencytop-0.1.tar.gz</a></td></tr>
<tr class="odd"><td>kernel patch 1/3</td><td><a class="plainlink" href="download/latencytop-infrastructure.patch">infrastructure</a></td></tr>
<tr><td>kernel patch 2/3</td><td><a class="plainlink" href="download/latencytop-annotations.patch">annotations part 1</a></td></tr>
<tr class="odd"><td>kernel patch 3/3</td><td><a class="plainlink" href="download/latencytop-more-annotations.patch">annotations part 2</a></td></tr>
</table>
<h3>Git tree</h3>
The development version of LatencyTOP is available from <a href="http://git.infradead.org/?p=latencytop.git">git</a>:
<pre>
git clone git://git.infradead.org/latencytop.git
</pre>

<hr>
Intel is a trademark of Intel Corporation in the U.S. and other countries. | * Other names / brands may be claimed as the property of others 
</html>