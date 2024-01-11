---
layout: default
title: Homeworks and Labs 
index_title: HWs+Labs
index_sort: 2
---

This page lists all lab and homework assignments.
There are also [quizzes]({{site.quiz_site}}){:target="_top"} after each week of lecture.

Labs which allow submission are due by 11:59am (before noon) the following day unless otherwise noted. Labs which do not allow submission must be completed in the lab time
(or some special arrangement made with the instructors) unless otherwise noted.

Homeworks are due at 11:59pm Eastern Time unless otherwise noted.

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
{%- if assignment.due_message -%}
{%- if assignment.page -%}
*  HW: <a href="{{assignment.page | relative_url}}">{{assignment.due_message}}</a> {%- if assignment.tentative -%}&nbsp;(tentative) {%- endif -%} &mdash; on {{assignment.due}}{{nl}}
{%- elsif assignment.url -%}
*  HW: <a href="{{assignment.url}}">{{assignment.due_message}}</a> {%- if assignment.tentative -%}&nbsp;(writeup tentative) {%- endif -%} &mdash; on {{assignment.due}}{{nl}}
{%- else -%}
*  HW: {{assignment.name}} (writeup not yet available) &mdash; due {{assignment.due}}{{nl}}
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
{%- endif -%}
{{nl}}
{% endfor %}
