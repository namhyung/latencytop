<? 
include("topbar.php");
?>

<h1>About LatencyTOP</h1>
Skipping audio, slower servers, everyone knows the symptoms. But to know what's 
going on exactly, what's causing it, how to fix it... that's a hard question.
<br><br>
LatencyTOP is a tool aimed at developers, aimed at identifying where in the system
latency is happening, and what kind of operation/action is causing the latency
to happen.
<br>
<h2>About latency</h2>
There are many types and causes of latency; LatencyTOP focuses on the type
of latency that causes skips in audio, stutters in your desktop experience
or that causes your overloaded server to run slower with idle CPUs.
<br><br>
LatencyTOP focuses on the cases where the applications want to run and 
execute, but there's some resource that's not currently available; both on a system level
as on a per process level.
<br>
<img src="latencytop.png">

<h2>Get it / Download</h2>
For now, LatencyTOP is available from <a href="http://git.infradead.org/?p=latencytop.git">git</a>:
<pre>
git clone git://git.infradead.org/latencytop.git
</pre>