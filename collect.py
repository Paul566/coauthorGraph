import requests
import re
import json
import igraph
import html
import os.path
from collections import deque

def getnet():
    templateurl = 'https://scholar.google.com/citations?user='

    if os.path.isfile('ids_to_names.json') and os.path.isfile('edges.json') and os.path.isfile('queue.json'):
        with open('ids_to_names.json') as f:
            ids_to_names = json.load(f)
        with open('edges.json') as f:
            list_edges = json.load(f)
        with open('queue.json') as f:
            queue = deque(json.load(f))
        edges = set()
        for l in list_edges:
            edges.add((l[0], l[1]))
        cnt = len(ids_to_names) - len(queue)
    else:
        if not (os.path.isfile('ids_to_names.json')) and not (os.path.isfile('edges.json')) and not (os.path.isfile('queue.json')):
            cnt = 0
            ids_to_names = {'zbtQMR8AAAAJ': 'Andronick Arutyunov'}
            edges = set()
            queue = deque(['zbtQMR8AAAAJ'])
        else:
            print("some json files are found but not all of them")
            return

    s = requests.Session()

    while queue:
        curid = queue.popleft()
        print(cnt, len(queue), "%.3f" % (len(queue) / (cnt + 1)), curid, ids_to_names[curid])
        cnt += 1
        r = html.unescape(s.get(templateurl + curid).text)
        if not r:
            print("Error", r.status_code)
            print("Breaking")
            break
        for m in re.finditer('a tabindex="-1" href="/citations\?user', r):
            id = r[m.end() + 1:m.end() + 13]
            edges.add((curid, id))
            if id in ids_to_names:
                continue
            queue.append(id)
            name = ''
            i = m.end() + 30
            while r[i] != '<' and r[i] != '(':
                name += r[i]
                i += 1
            ids_to_names[id] = name
        if cnt % 1000 == 0:
            with open('ids_to_names.json', 'w') as f:
                json.dump(ids_to_names, f)
            with open('edges.json', 'w') as f:
                json.dump(list(edges), f)
            with open('queue.json', 'w') as f:
                json.dump(list(queue), f)
    with open('ids_to_names.json', 'w') as f:
        json.dump(ids_to_names, f)
    with open('edges.json', 'w') as f:
        json.dump(list(edges), f)
    with open('queue.json', 'w') as f:
        json.dump(list(queue), f)



if __name__ == '__main__':
    getnet()
