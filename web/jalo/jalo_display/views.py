from django.shortcuts import render

from django.http import HttpResponse

from jalo_display.models import TargetData

import os
import stl
import numpy as np
import cv2 as cv

# Create your views here.

history_size = 5000
MODELS_DIR = os.path.join(os.path.dirname(__file__), "../../../models/shop1")
viewport_disp = [0, 0, 200, 150]
viewport = [-6.5, -5.5, 20, 15]

targets = {}

def pos_on_frame(x0, y0):
    x = (-y0 - viewport[0]) / viewport[2] * viewport_disp[2] + viewport_disp[0]
    y = (-x0 - viewport[1]) / viewport[3] * viewport_disp[3] + viewport_disp[1]
    return [int(x), int(y)]

def reload_models():
    global rects
    for file in os.listdir(MODELS_DIR):
        path = os.path.join(MODELS_DIR, file)
        mesh = stl.mesh.Mesh.from_file(path)
        name = file.split(".")[-2]
        points = []
        center = np.zeros((2,), dtype=np.float32)
        for v0, v1, v2 in zip(mesh.v0, mesh.v1, mesh.v2):
            triangle = []
            triangle.append(pos_on_frame(v0[0], v0[1]))
            triangle.append(pos_on_frame(v1[0], v1[1]))
            triangle.append(pos_on_frame(v2[0], v2[1]))
            for pt in triangle:
                center += pt
            points.append(triangle)
        center /= (len(points) * 3)
        if name not in targets:
            targets[name] = {}
        targets[name]["center"] = center
        targets[name]["model"] = points

def reload_db():
    entries = TargetData.objects.all()

    # entries = list(entries)[-history_size:]

    for name in targets:
        targets[name]["hits"] = 0

    for entry in entries:
        if entry.target is None:
            continue
        elif entry.target not in targets:
            targets[entry.target] = {}
            targets[entry.target]["hits"] = 0
        else:
            targets[entry.target]["hits"] += 1

    print(len(entries))

    maxHits = 0
    for name in targets:
        if name not in ["1", "17"] and targets[name]["hits"] > maxHits:
            maxHits = targets[name]["hits"]

    for name in targets:
        heat = targets[name]["hits"] / maxHits
        if heat > 1:
            heat = 1
        targets[name]["heat"] = heat


reload_models()

def index(request):
    reload_db()

    html = open(os.path.join(os.path.dirname(__file__), "static/pages/index.html")).read()

    drawing = '<svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"' + \
    'x="{}px" y="{}px" '.format(viewport_disp[0], viewport_disp[1])+\
    'viewBox="{} {} {} {}" '.format(viewport_disp[0], viewport_disp[1], viewport_disp[2], viewport_disp[3]) + \
    'xml:space="preserve" preserveAspectRatio="none" >'

    for name, data in targets.items():
        heat = data["heat"]
        h0 = 240
        h1 = 60
        h = h0 + heat * (h1 - h0)
        color = "hsl({}, 100%, 50%)".format(h)

        for (x0, y0), (x1, y1), (x2, y2) in data["model"]:
            # drawing += '<rect id="box" x="{}" y="{}" width="{}" height="{}"/>'.format(x, y, w, h)
            drawing += '<polygon style="fill: {}" id="box" points="{},{} {},{} {},{}"/>\n'.format(color, x0, y0, x1, y1, x2, y2)

    for name, data in targets.items():
        xc, yc = data["center"]
        xc -= 1.1 * (len(name) + 1)
        yc += 2
        drawing += '<text x="{}" y="{}" class="label">{}</text>\n'.format(xc, yc, name)

    drawing += "</svg>"

    html = html.replace("$$MAP$$", drawing)

    return HttpResponse(html)
