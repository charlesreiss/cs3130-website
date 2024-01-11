---
layout: default
title: Schedule
index_title: Schedule
index_sort: 99
---

<p>
<b>This schedule is an estimate. It may be updated based on the actual pacing of the course material.</b>
</p>

<p id="weekJump" style="display: none"></p>
<p id="hideOutline" style="display: none"></p>

<table class="schedule" id="schedule" data-start="{{ site.data.schedule[0].date | date: "%Y-%m-%d" }}">
<tr class="header"><td>Date</td><td>Topic</td><td>Assignment</td></tr>
{%- assign last_week = 0 -%}
{%- for day in site.data.schedule -%}
{%- capture week_day -%}{{ day.date | date: "%a" }}{%- endcapture -%}
{%- capture file_prefix -%}{{ day.date | date: "%Y%m%d" }}{%- endcapture -%}
{%- assign due_assignments = site.data.assignments | where:"due",day.date | where: "hide_web",nil -%}
{%- assign released_assignments = site.data.assignments | where:"released",day.date | where: "hide_web",nil -%}
{%- assign open_quizzes = site.data.quizzes | where:"open_date",day.date -%}
{%- assign due_quizzes = site.data.quizzes | where:"due_date",day.date -%}
{%- capture file_prefix -%}{{ day.date | date: "%Y%m%d" }}{%- endcapture -%}
{%- assign video_raw = site.data.recordings | where:"prefix",file_prefix-%}
{%- if video_raw.size > 0 -%}
    {%- assign video = true %}
{%- else -%}
    {%- assign video = false %}
{%- endif -%}
{%- if week_day == "Fri" or week_day == "Sat" or week_day == "Sun" -%}
{%- assign non_lecture_day = true -%}
{%- else -%}
{%- assign non_lecture_day = false -%}
{%- endif -%}
{%- if day.week != nil and day.week != last_week -%} 
{%- assign last_week = day.week -%}
<tr class="week_num" id="week-{{ day.week }}" data-week-number="{{ day.week }}" data-iso-week="{{ day.date | date: "%Y-W%V" }}"><td colspan="3">Week {{ day.week }}</td></tr>
{%- endif -%}
<tr class="{%- if day.no_class -%} no_class {%- elsif day.no_lab -%} no_lab {%- elsif day.lab -%} lab {%- elsif day.exam -%} exam {%- elsif non_lecture_day -%} non_lecture_day {%- else -%} lecture {%- endif -%}">
<td class="date">{{ day.date | date: "%a %d %b" }}</td>
<td class="description">
<div class="titlestuff">
{%- if day.title -%}
<h3>{{ day.title }}</h3>
{%- endif -%}
{%- if day.who or day.slides or day.video_webm or day.slides_see or day.slides_base -%}
&nbsp;[&nbsp;
    {%- if day.who -%}{{ day.who }}:&nbsp;{%- endif -%}
{%- endif -%}
{%- if day.slides -%}
<div class="slides">
  {%- if day.slides_see -%}
    end of
    <a href="slides/{{ day.slides_see | date: "%Y%m%d" }}-slides.pdf">{{day.slides_see | date: "%d %b"}}</a>
    pdf plus
  {%- endif -%}
  <a href="slides/{{ file_prefix }}-slides.pdf">{%- if day.slides_tentative or day.slides_prelim -%}&nbsp;tentative&nbsp;{%- else -%} <!-- -->{%- endif -%}slides</a>
  {%- if day.slides_extra -%}&nbsp;<span class="slides-extra-note">(more than one day of slides)</span>{%- endif -%}
  {%- if day.slides_pptx -%}&nbsp;(or <a href="slides/{{ file_prefix }}-slides.pptx">as pptx</a>){%- endif -%}
  {%- if day.slides_see2 -%}
      &nbsp;plus <a href="slides/{{ day.slides_see2 | date: "%Y%m%d" }}-slides.pdf">{{day.slides_see2 | date: "%d %b"}} slides</a> {%- if day.slides_see2_note -%}{{day.slides_see2_note}}{%- endif -%}
  {%- endif -%}
</div>
{%- else -%}
  {%- if day.slides_see -%}
  <div class="slides">
    (tentative) slides: see <a href="slides/{{ day.slides_see | date: "%Y%m%d" }}-slides.pdf">{{day.slides_see | date: "%d %b"}}</a> {%- if day.slides_see_note -%}{{day.slides_see_note}}{%- endif -%}
    {%- if day.slides_see2 -%}
        and <a href="slides/{{ day.slides_see2 | date: "%Y%m%d" }}-slides.pdf">{{day.slides_see2 | date: "%d %b"}}</a> {%- if day.slides_see2_note -%}{{day.slides_see2_note}}{%- endif -%}
    {%- endif -%}
  </div>
  {%- else -%}
    {%- if day.slides_base -%}
      <a href="slides/{{ day.slides_base }}.pdf">tentative slides</a>
    {%- endif -%}
  {%- endif -%}
{%- endif -%}
{%- if video -%}
      | screencapture (<a href="/~cr4bd/videoplayer/?3130/F2023/recordings/{{ file_prefix }}-video-and-audio">browser</a>&nbsp; or download <a href="recordings/{{ file_prefix }}-video-and-audio.mp4">mp4</a> <a href="recordings/{{ file_prefix }}-video-and-audio.webm">webm</a> <a href="recordings//{{ file_prefix }}-audio.mp3">audio</a>
    )
{%- else -%}
    {%- if day.video_mp4 -%}
      | screencapture (download <a href="recordings/{{ file_prefix }}-video-and-audio.mp4">mp4</a> <a href="recordings//{{ file_prefix }}-audio.mp3">audio</a>)
    {%- endif -%}
{%- endif -%} 
{%- if day.slides or day.video_webm or day.who or day.slides_see -%}
&nbsp;]
{%- endif -%}
</div>
<!-- TODO: lecture/etc. links -->
<div class="due_assignments">
{%- for due_assignment in due_assignments -%}
{%- if due_assignment.lab -%}
<h3>Lab: {{due_assignment.name}}</h3>
<br>
{%- if due_assignment.page -%}
    {%- if due_assignment.tentative -%}
        <a href="{{due_assignment.page|relative_url}}">tentative lab writeup</a>
    {%- else - %}
        <a href="{{due_assignment.page|relative_url}}">lab writeup</a>
    {%- endif -%}
{%- endif -%}
{%- endif -%}
{%- endfor -%}
</div>
<div class="description">
{%- if day.reading -%}
<em class="readingLabel">Reading: </em> <span class="readingText">{{day.reading | markdownify}}</span>
{%- endif -%}
{%- if day.description -%}
  {{ day.description | markdownify }}
{%- elsif day.no_class -%}
  (no class)
{%- elsif non_lecture_day -%}
  &mdash;
{%- endif -%}
</div>
</td>
{%- if due_assignments or released_assignments or day.assignment or open_quizzes or due_quizzes -%}
<td class="assignment">
{%- for quiz in open_quizzes -%}
Quiz {{quiz.name}} ({{quiz.title}}) released, due {{quiz.due_date}} {{quiz.due | date: "%H:%M"}}<br>
{%- endfor -%}
{%- for quiz in due_quizzes -%}
Quiz {{quiz.name}} ({{quiz.title}}) due {{quiz.due | date: "%H:%M"}} (released {{quiz.open_date}})<br>
{%- endfor -%}
{%- if day.assignment -%}
  {{day.assignment}}
{%- endif -%}
{%- for due_assignment in due_assignments -%}
  {%- if due_assignment.due_message -%}
    {%- if due_assignment.tentative -%}
    <span class="duehw">{{due_assignment.due_message}} (<a href="{{due_assignment.page|relative_url}}">tentative writeup</a>)</span><br>
    {%- else -%}
    <span class="duehw"><a href="{{due_assignment.page|relative_url}}">{{due_assignment.due_message}}</a></span><br>
    {%- endif -%}
  {%- else -%}
      {%- if due_assignment.lab -%}
        Lab assigned<br>
      {%- elsif due_assignment.page -%}
        {%- if due_assignment.tentative -%}
        <span class="duehw">{{due_assignment.name}} (<a href="{{due_assignment.page|relative_url}}">tentative writeup</a>) due by 11:59pm</span><br>
        {%- else -%}
        <span class="duehw"><a href="{{due_assignment.page|relative_url}}">{{due_assignment.name}}</a> due by 11:59pm</span><br>
        {%- endif -%}
      {%- elsif due_assignment.url -%}
        <span class="duehw"><a href="{{due_assignment.url}}">{{due_assignment.name}}</a>due</span><br>
      {%- else -%}
        <span class="duehw">{{due_assignment.name}} due</span><br>
      {%- endif -%}
  {%- endif -%}
{%- endfor -%}
{%- for released_assignment in released_assignments -%}
  {%- if released_assignment.released_message -%}
      {{released_assignment.release_message}}<br>
  {%- else -%}
      {%- if released_assignment.page -%}
          {%- if released_assignment.tentative -%}
          <a href="{{released_assignment.page|relative_url}}">{{released_assignment.name}}</a> released<br>
          {%- else -%}
          {{released_assignment.name}} (<a href="{{released_assignment.page|relative_url}}">tentative writeup</a>) released<br>
          {%- endif -%}
      {%- else -%}
      {{released_assignment.name}} released<br>
      {%- endif -%}
  {%- endif -%}
{%- endfor -%}
{%- if day.due_message -%}
{{day.due_message}}
{%- endif -%}
</td>
{%- else -%}
<td class="noassignment"></td>
{%- endif -%}
</tr>
{%- endfor -%}
</table>

<script>
week_1 = new Date(document.querySelector("#schedule").dataset.start);
week_1.setDate(week_1.getDate() - week_1.getDay());
now = Date.now();
offset = (now - week_1) / (1000 * 60 * 60 * 24 * 7);
offset_week = Math.floor(offset);
current_week = offset_week + 1;
result = ""
if (document.querySelector("#week-" + current_week)) {
    result = "Jump to <a href='#week-" + current_week + "'>the current week</a> (week " + current_week + ").";
}
if (result != "") {
    document.querySelector("#weekJump").innerHTML = result;
    document.querySelector("#weekJump").style.display = "block";
}

function showOutlines() {
    document.querySelectorAll('.description').forEach(x => x.classList.remove('hide'));
}
function hideOutlines() {
    document.querySelectorAll('.description').forEach(x => x.classList.add('hide'));
}

document.querySelector("#hideOutline").innerHTML =
    '<input type="button" value="show" onclick="showOutlines()"> \
    or <input type="button" value="hide" onclick="hideOutlines()"> outlines/readings';

document.querySelector("#hideOutline").style.display = "block";
</script>
