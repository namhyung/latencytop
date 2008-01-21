<? 
include("topbar.php");
?>

<table>
<tr><td valign=top width=190>
<table>
<tr><td class=subsection>GENERAL</td></tr>
<tr><td class='nav_box'><a class='nav' href='/index.php?n=Info'>Info</a></td></tr>
<tr><td class='nav_box_active'><a class='nav' href='announce.php'>Announcement</a></td></tr> 
<tr><td class='nav_box'><a class='nav' href='download.php'>Download</a></td></tr> 
<tr><td class='nav_box'><a class='nav' href='git.php'>GIT Repository</a></td></tr> 
</table>
</td>
<td>

<h3>Announcement text</h3>
<pre>
The Intel Open Source Technology Center is pleased to announce the 
release of version 0.1 of LatencyTOP, a tool for developers to visualize 
system latencies.

http://www.latencytop.org

Slow servers, Skipping audio, Jerky video --everyone knows the symptoms 
of latency. But to know what's really going on in the system, what's causing 
the latency, and how to fix it... those are difficult questions without 
good answers right now. 

LatencyTOP is a Linux tool for software developers (both kernel and 
userspace), aimed at identifying where system latency occurs, and what 
kind of operation/action is causing the latency to happen. By identifying 
this, developers can then change the code to avoid the worst latency 
hiccups. 

There are many types and causes of latency, and LatencyTOP focus on type
that causes audio skipping and desktop stutters. Specifically, LatencyTOP 
focuses on the cases where the applications want to run and execute useful 
code, but there's some resource that's not currently available (and the 
kernel then blocks the process). This is done both on a system level and 
on a per process level, so that you can see what's happening to the system, 
and which process is suffering and/or causing the delays. 

Rather than spending a thousand words on describing what LatencyTOP is, 
I'll let the numbers speak..

Here's some output of LatencyTOP, collected for a make -j4 of a kernel 
on a quad core system:

Cause                                        Maximum          Average
process fork                               1097.7 msec         2.5 msec
Reading from file                          1097.0 msec         0.1 msec
updating atime                             850.4 msec         60.1 msec
Locking buffer head                        433.1 msec         94.3 msec
Writing to file                            381.8 msec          0.6 msec
Synchronous bufferhead read                318.5 msec         16.3 msec
Waiting for buffer IO                      298.8 msec          7.8 msec


This shows that during the kernel build, one fork() system call had 
a > 1 second latency, as did a read from a file. Updating the atime 
of a file (in memory!) had 850 milliseconds, etc etc.

With a small change to the EXT3 journaling layer to fix a priority 
inversion problem that was found with LatencyTOP the following results 
for the exact same test were achieved [*]
( http://kerneltrap.org/mailarchive/linux-kernel/2007/10/15/343499 ) 

Cause                                        Maximum          Average
Writing to file                            814.9 msec          0.8 msec
Reading from file                          441.1 msec          0.1 msec
Waiting for buffer IO                      419.0 msec          3.4 msec
Locking buffer head                        360.5 msec         75.7 msec
Unlinking file                             292.7 msec          5.9 msec
EXT3 (jbd) do_get_write_access             274.0 msec         36.0 msec
Filename lookup                            260.0 msec          0.5 msec

As you can see from the results, the maximum observed latency is down 
from 1097 to 814 msec, and the other top latencies are down as well 
(although some have shifted around some, such as writing to a file).


Kernel patches
--------------
LatencyTOP requires some kernel patches to work. These patches are 
available from the website for review and I will be posting them as 
reply to this announce email as well.
The patches aren't quite ready for mainline inclusion yet 
(assuming they aren't hated universally in the first place). For example,
the locking is still rather coarse grained. However, it will be easy
to split the lock in a reasonable way. I'd rather incorporate all other 
feedback first before doing more fancy locking though.

Annotations
-----------
The kernel patch is needed in part to allow the kernel to annotate certain 
operations for tracking of latency. The idea for an annotation is that the 
kernel keeps track of what high level operation it is doing, which is 
subsequently used from the low level scheduler code to account the actual 
observed latency to. The good news is that the number of annotations needed 
is limited;x for one, all low level functions that sleep directly don't need 
annotation (this gets dealt with automatically), and there's only a finite 
number of higher level operations that are relevant .

The most basic annotation looks like this (in the patches more complex
versions can be seen):

 asmlinkage long sys_sync(void)
 {
+       struct latency_entry reason;
+       set_latency_reason("sync system call", &reason);
        do_sync(1);
+       restore_latency_reason(&reason);
+
        return 0;
 }

This annotation tells the kernel that for anything that sleeps in the 
entire do_sync() codepath, the latency introduced by that should be 
accounted as "sync system call". The nesting of this annotation is 
very simple: latency gets accounted to highest level annotated; so 
even if there's another annotation inside the do_sync codepath 
somewhere, the blocking latency that happens in this codepath counts 
towards the sync function call.


At this point, I would like to invite comments and reviews of the 
concept and the code to see if people consider this tool useful, 
what they would like the tool to do in addition to its current 
functionality, and what kind of code improvements people would like 
to see. I also fully realize that not everyone will like the way the 
annotations are done and I'm open to better ways of achieving the same 
goals.


Known TODO list:
* More finegrained reporting of scheduler based (CFS) statistics
* Improve locking granularity
	
	

	
	[*] I'm not trying to get this patch merged, Jens Axboe had many 
	    valid comments about it and it needs to be redone for mainline
	    inclusion. The patch and it's effect are for illustration 
	    purposes only.
</pre>

</td></tr>
</table>
<hr>
Intel is a trademark of Intel Corporation in the U.S. and other countries. | * Other names / brands may be claimed as the property of others 
</html>