#!/bin/bash

here="$(dirname "$(readlink -m "$0")")/"
cd "$here"
target="$HOME/public_html/COA2/S2020/"
mkdir -p "$target"files

if [ "$#" -lt 1 ] || [ "$1" != 'test' ]
then
    git commit -a -m 'autocommit caused by update'
    git pull
    git push
elif [ "$1" = 'test' ]
then
    shift
fi


function pd() {
    prefix=''
    n="$1"
    while [ "$n" != "$(basename "$n")" ]
    do
        n="$(dirname $n)"
        prefix="../$prefix"
    done

    while [ $# -gt 0 ]
    do
        python3 $here/env.py "$1" | \
        pandoc --standalone --to=html5 \
            --from=markdown+inline_code_attributes+smart \
            --number-sections \
            --title-prefix="COA2" \
            --table-of-contents --toc-depth=3 \
            --css=${prefix}style.css \
            --template=${here}template.html \
            --variable=year:$(date +%Y) \
            --variable=datetime:"$(date "+%Y-%m-%d %H:%M")" \
            -o "$target${1%.md}.html"
        sed -i \
            -e 's;</?colgroup>;;g' -e 's;<col [^>]*/>;;g' \
            -e 's/<span class="co">\&#39;/<span class="st">\&#39;/g' \
            -e 's/<span class="co">\&quot;/<span class="st">\&quot;/g' \
            -e 's/<table style="width:[^"]*">/<table>/g' \
            "$target${1%.md}.html"
        chmod a+r "$target${1%.md}.html"
        shift
    done
}



if [ cal.yaml -nt markdown/schedule.html ] \
|| [ links.yaml -nt markdown/schedule.html ] \
|| [ cal2html.py -nt markdown/schedule.html ] \
|| [ cal.yaml -nt markdown/cal.ics ] \
|| [ cal.yaml -nt assignments.json ]
then
    python3 cal2html.py
    scp "assignments.json" "kytos.cs.virginia.edu:/var/www/html/coa2-s2020/meta/"
    scp "coursegrade.json" "kytos.cs.virginia.edu:/var/www/html/coa2-s2020/meta/"
fi

if [ markdown/schedule.html -nt markdown/schedule.md ]
then
    touch --date="$(stat -c "%y" markdown/schedule.html)" markdown/schedule.md
fi

cd "$here"markdown
for f in *.md files/*.md
do
    if [ ! -e "$f" ]; then continue; fi
    name="${f%.md}"
    if [ "$f" -nt "$target$name.html" ] || [ -n "$1" ] || [ ../template.html -nt "$target$name.html" ]
    then
        echo 'updating' "$target$name.html"
        pd "$f"
        sed -i 's;XXXX-XX-XX;'"$(stat -c '%y' "$f" | cut -d'.' -f1)"';g' "$target$name.html"
        sed -i 's,<a href="'"$name"'.html">\([^<]*\)</a>,<span class="visited">\1</span>,g' "$target$name.html"
    fi
done
cd "$here"

for f in markdown/*.{pdf,css,ics,js}
do
    if [ ! -e "$f" ]; then continue; fi
	if [ "$f" -nt "$target$f" ]
	then
		echo 'copying' "$f"
		cp "$f" "$target";
	fi
done

find markdown/files -type f | while read from
do
    if [ ! -e "$f" ]; then continue; fi
    to="$target/${from#markdown/}"
    if [ "$from" -nt "$to" ]; then 
        cp "$from" "$to"
    fi
done


#rsync --update --compress --recursive --times --verbose -e ssh "$target" "$remote"

