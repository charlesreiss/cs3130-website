---
layout: default
title: Homeworks and Labs 
index_title: HWs+Labs
index_sort: 2
---

This page lists all lab and homework assignments.
There are also [quizzes]({{site.quiz_site}}){:target="_top"} before and after each week of lecture.

Labs are due by the end of the day (11:59pm Eastern Time) unless otherwise noted.

Homeworks are due at 4:59pm Eastern Time (before the first lab) unless otherwise noted.

If an assignment is linked but tentative, there may be some changes, perhaps very substantial changes,
to the assignment before it is officially released.

{: #assignments}
{% for assignment in site.data.assignments %}
{%- if assignment.hide_web -%}
    {% continue %}
{%- endif -%}
{%- if assignment.lab -%}
{%- if assignment.page -%}
*  Lab: <a href="{{assignment.page | relative_url}}">{{assignment.name}}</a> {%- if assignment.tentative -%}&nbsp;(writeup tentative) {%- endif -%} &mdash; on {{assignment.due}}{{nl}}
{%- elsif assignment.no_due_link -%}
*  Lab: {{assignment.name}} ({{assignment.due}}){{nl}}
{%- else -%}
*  Lab: {{assignment.name}} (writeup not yet available) &mdash; on {{assignment.due}}{{nl}}
{%- endif -%}
{%- else -%} 
{%- if assignment.page -%}
*  HW: <a href="{{assignment.page | relative_url}}">{{assignment.name}}</a> {%- if assignment.tentative -%}&nbsp;(tentative) {%- endif -%} &mdash; due {{assignment.due}}{{nl}}
{%- elsif assignment.url -%}
*  HW: <a href="{{assignment.url}}">{{assignment.name}}</a> {%- if assignment.tentative -%}&nbsp;(writeup tentative) {%- endif -%} &mdash; due {{assignment.due}}{{nl}}
{%- else -%}
*  HW: {{assignment.name}} (writeup not yet available) &mdash; due {{assignment.due}}{{nl}}
{%- endif -%}
{%- endif -%}
{{nl}}
{% endfor %}
