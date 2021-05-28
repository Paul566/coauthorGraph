import requests
import re
import json
import igraph
import html
import os.path


def getnet():
    templateurl = 'https://scholar.google.com/citations?user='

    if os.path.isfile('ids_to_names.json') and os.path.isfile('edges.json') and os.path.isfile('queue.json'):
        with open('ids_to_names.json') as f:
            ids_to_names = json.load(f)
        with open('edges.json') as f:
            list_edges = json.load(f)
        with open('queue.json') as f:
            queue = json.load(f)
        edges = set()
        for l in list_edges:
            edges.add((l[0], l[1]))
        cnt = len(ids_to_names) - len(queue)
    else:
        if not (os.path.isfile('ids_to_names.json')) and not (os.path.isfile('edges.json')) and not (os.path.isfile('queue.json')):
            cnt = 0
            ids_to_names = {'zbtQMR8AAAAJ': 'Andronick Arutyunov'}
            edges = set()
            queue = ['zbtQMR8AAAAJ']
        else:
            print("some json files are found but not all of them")
            return

    while queue:
        curid = queue.pop(0)
        print(cnt, len(queue), "%.3f" % (len(queue) / (cnt + 1)), curid, ids_to_names[curid])
        cnt += 1
        r = html.unescape(requests.get(templateurl + curid).text)
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
        if cnt % 100 == 0:
            with open('ids_to_names.json', 'w') as f:
                json.dump(ids_to_names, f)
            with open('edges.json', 'w') as f:
                json.dump(list(edges), f)
            with open('queue.json', 'w') as f:
                json.dump(queue, f)
    with open('ids_to_names.json', 'w') as f:
        json.dump(ids_to_names, f)
    with open('edges.json', 'w') as f:
        json.dump(list(edges), f)
    with open('queue.json', 'w') as f:
        json.dump(queue, f)


def plotnet(threshold):
    with open('ids_to_names.json') as f:
        ids_to_names = json.load(f)
    with open('edges.json') as f:
        list_edges = json.load(f)
    edges = set()
    for l in list_edges:
        edges.add((l[0], l[1]))
    list_ids = []
    for key in ids_to_names:
        list_ids.append(key)
    g = igraph.Graph()
    N = len(list_ids)
    g.add_vertices(N)
    for i in range(N):
        g.vs[i]['label'] = ids_to_names[list_ids[i]]
    for i in range(N):
        for j in range(i, N):
            if (list_ids[i], list_ids[j]) in edges:
                g.add_edge(i, j)

    nodes_to_delete = []
    for i in range(N):
        if g.vs[i].degree() < threshold:
            nodes_to_delete.append(i)
    g.delete_vertices(nodes_to_delete)

    visual_style = {'vertex_size': 5,
                    'vertex_label_size': 18,
                    'vertex_label_dist': 3,
                    'bbox': (1000, 1000),
                    'margin': 100,
                    'edge_curved': True}

    plot = igraph.Plot(target='coauthorGraph.png', bbox=(1000, 1000), background="white")
    plot.add(g, layout=g.layout_kamada_kawai(), **visual_style)
    plot.redraw()
    # ctx = cairo.Context(plot.surface)
    # ctx.set_font_size(24)
    # drawer = TextDrawer(ctx, title, halign=TextDrawer.CENTER)
    # drawer.draw_at(0, 40, width=1000)
    plot.save()


if __name__ == '__main__':
    getnet()
