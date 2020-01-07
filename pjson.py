def prettyjson(d, newlineindent=2, maxinline=79):
    """The way I like to see JSON:
    commas begin lines
    short collections inline
    no extra spaces"""
    import json
    def doDates(d):
        return d.strftime('%Y-%m-%d %H:%M')
    
    s = json.dumps(d, separators=(',',':'), default=doDates)
    indent = 0
    instr = False
    def skipshort(start, maxlen):
        """Given the starting index of a list or dict, returns either
        the same index, if it is too long,
        or the index of the last character of the list/dict"""
        nest = 0
        instr = False
        i = start
        comma = False
        while i < len(s) and ((not comma) or (i < start+maxlen)):
            if instr:
                if s[i] == '\\': i+=1
                elif s[i] == '"': instr = False
            else:
                if s[i] == '"': instr = True
                elif s[i] in '[{': nest += 1
                elif s[i] in ']}':
                    nest -= 1
                    if nest == 0: return i
                elif s[i] == ',': comma = True
            i += 1
        return start
    chunks = []
    i=0
    last=0
    indents = []
    while i < len(s):
        if instr:
            if s[i] == '\\': 
                i+=1
            elif s[i] == '"': instr = False
        elif s[i] == '"': instr = True
        else:
            if s[i] in '[{':
                end = skipshort(i, maxinline-(i-last)-indent)
                if end > i: i = end
                elif (i-last) < 8:
                    indents.append(indent)
                    indent += (i-last)
                else:
                    chunks.append(s[last:i])
                    indents.append(indent)
                    indent += newlineindent
                    chunks.append('\n'+' '*indent)
                    last = i
            elif s[i] in ']}':
                chunks.append(s[last:i])
                chunks.append('\n'+' '*indent + s[i])
                indent = indents.pop()
                last = i+1
            elif s[i] == ',':
                chunks.append(s[last:i])
                chunks.append('\n'+' '*indent)
                last = i
        i += 1
    if last < i: chunks.append(' '*indent + s[last:i])
    return ''.join(chunks)
