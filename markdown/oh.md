---
title: Office Hours
...

Professor offices hours are in the professor's office.
If I'm engaged with a student, don't just wait silently in the hall; ask me if you can come in and join the other student(s) there (often the answer will be yes, and even if no I'll know you are waiting).

{.example ...} How to get help from a professor

1. Find when a professor is free
2. Go to that professor's office
3. Come inside, or if it is full make sure the professor knows you are there and are waiting
{/}


TA office hours are in Thornton Stacks (A-235), and are requested by using the [office hour queue](https://kytos.cs.virginia.edu/ohq/?c=coa2).
The queue is open even when TAs are not present; see below for when to expect TAs in office hours.

{.example ...} How to get help from a TA

1. Find when TAs are holding office hours
2. Go to Thorton Stacks and sit in a seat
3. Go to the office hour queue and enter your seat number (see [a map](https://kytos.cs.virginia.edu/StacksStickers.png){target="_blank"} if your seat's sticker has been removed)
4. Stay there until a TA comes to you
{/}




<div style="display:table; font-size:200%; margin: 1em auto; padding:1ex; box-shadow: 0 1px 10px rgba(0,0,0,.1); border: thin solid #eee; border-radius:1ex; background-image: linear-gradient(to bottom, #ffffff, #f2f2f2);">[Get TA Help](https://kytos.cs.virginia.edu/ohq/?c=coa2)</div>

Upcoming office hours (for the next week):

<table class="oh">
<thead><tr><th>Day</th><th>Whom</th><th>Starts</th><th>Ends</th></tr></thead>
<tbody id="cal-oh">
</tbody>
</table>

<style>
table.oh { margin: auto; }
table.oh thead td { border-bottom: thin solid black; }
table.oh td { text-align: center; }
table.oh tbody tr:nth-child(2n+1) { background: linear-gradient(to right, rgba(0,0,0,0.03125), rgba(0,0,0,0)); }
.oh.ta { background-color: rgba(0,255,127,0.125); }
.oh.faculty { background-color: rgba(0,127,255,0.125); }
tr.Sun, tr.Mon, tr.Tue, tr.Wed, tr.Thu, tr.Fri, tr.Sat { border-top: thick solid rgba(0,0,0,0.25); }
tr.Sun + tr.Sun, tr.Mon + tr.Mon, tr.Tue + tr.Tue, tr.Wed + tr.Wed, tr.Thu + tr.Thu, tr.Fri + tr.Fri, tr.Sat + tr.Sat { border-top: none; }
</style>
<script src="moment.min.js" type="text/javascript"></script>
<script src="cal-oh.js" type="text/javascript"></script>
<script type="text/javascript">//<!--
now = new Date(); now.setDate(now.getDate() - 1); now = now.toISOString()
week = new Date(); week.setDate(week.getDate() + 7); week = week.toISOString()
within = document.getElementById('cal-oh')
oh_feed.forEach(x => {
    if (x.end < now || x.start > week) return;
    console.log(x)
    s = new Date(x.start)
    e = new Date(x.end)
    tr = within.insertRow()
    tr.classList.add(moment(x.start).format('ddd'))
    tr.insertCell().innerText = moment(x.start).format('ddd D MMM')
    let entry = tr.insertCell()
    entry.classList.add('oh')
    entry.classList.add(x['title'].split(' ')[0] == 'TA' ? 'ta' : 'faculty')
    if ('link' in x) {
        let a = document.createElement('a')
        a.href = x['link']
        a.innerText = x['title'].replace(/\bOH\b/g, '').trim()
        entry.appendChild(a);
    } else {
        entry.innerText = x['title'].replace(/\bOH\b/g, '').trim()
    }
    tr.insertCell().innerText = moment(x.start).format('h:mm a')
    tr.insertCell().innerText = moment(x.end).format('h:mm a')
})
//--></script>
