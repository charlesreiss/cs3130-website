var today = new Date().toISOString().substr(0,10);

document.querySelectorAll('.day').forEach(function(x){
    if (!x.getAttribute('date')) return;
    if (x.getAttribute('date') < today) x.classList.add('past');
    else {
        /* if (x.getAttribute('date') == today) x.classList.add('today');
        else */ x.classList.add('future');
        x.parentElement.classList.remove('past');
    }
});
document.querySelectorAll('.week').forEach(function(x){
    if (x.querySelector('.future'))
        x.classList.add('future');
    else
        x.classList.add('past');
});
document.querySelector('.day:not(.past) div').parentElement.classList.add('today');

function saveCookie(key, value) {
    var d = new Date();
    d.setTime(d.getTime() + (365.24*24*60*60*1000)); // 1 year
    var expires = "expires="+ d.toUTCString();
    document.cookie = key + '=' + value + ";" + expires;
}
function viewmode(me) {
    if (me.value) me = me.value;
    document.getElementById('schedule').classList.remove('calendar');
    document.getElementById('schedule').classList.remove('agenda');
    document.getElementById('schedule').classList.add(me);
    saveCookie('viewmode', me);
    if (!document.getElementById('viewmode='+me).checked) {
        document.getElementById('viewmode='+me).checked = true;
    }
}
function show(me,val) {
    if (me.value) return show(me.value, me.checked);
    let css = document.getElementById('schedule-css');
    if (val) {
        for(let i=0; i<css.sheet.cssRules.length; i+=1) {
            if (css.sheet.cssRules[i].cssText == '.'+me+' { display: none; }') {
                css.sheet.deleteRule(i);
                break;
            }
        }
    } else {
        css.sheet.insertRule('.'+me+' { display: none; }');
    }
    saveCookie('view_'+me, val);
}
function showPast(visible) {
    if (typeof(visible) != 'boolean') {
        visible = visible.checked;
        saveCookie('showpast', visible);
        console.log('showPast button',visible)
    } else {
        console.log('showPast cookie',visible)
    }
    let css = document.getElementById('schedule-css');
    if (visible) {
        for(let i=0; i<css.sheet.cssRules.length; i+=1) {
            if (css.sheet.cssRules[i].cssText == '.calendar .week.past { display: none; }')
            { css.sheet.deleteRule(i); i-=1; }
            else if (css.sheet.cssRules[i].cssText == '.calendar > .day.past { display: none; }')
            { css.sheet.deleteRule(i); i-=1; }
            else if (css.sheet.cssRules[i].cssText == '.agenda .day.past { display: none; }')
            { css.sheet.deleteRule(i); i-=1; }
            else if (css.sheet.cssRules[i].cssText == '.agenda .week.past { display: none; }')
            { css.sheet.deleteRule(i); i-=1; }
        }
    } else {
        css.sheet.insertRule('.calendar .week.past { display: none; }');
        css.sheet.insertRule('.calendar > .day.past { display: none; }');
        css.sheet.insertRule('.agenda .day.past { display: none; }');
        css.sheet.insertRule('.agenda .week.past { display: none; }');
    }
    document.getElementById('showpast').checked = visible;
}
String(document.cookie).split('; ').forEach(function(x){
    x = x.split('=');
    if (x[0] == 'viewmode') viewmode(x[1]);
    else if (x[0] == 'showpast') showPast(x[1] == 'true');
    else if (x[0].startsWith('view_')) {
        let input = document.querySelector('input[name="show"][value="'+x[0].substr(5)+'"]');
        if (input) {
            input.checked = x[1] == 'true';
            show(x[0].substr(5), input.checked);
        }
    }
});
