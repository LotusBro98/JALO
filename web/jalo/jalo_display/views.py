from django.shortcuts import render

from django.http import HttpResponse
\
from jalo_display.models import TargetData

# Create your views here.

history_size = 5000

def index(request):
    entries = TargetData.objects.all()
    
    entries = list(entries)[-history_size:]

    targets = {}

    for entry in entries:
        if entry.target is None:
            continue
        elif entry.target not in targets:
            targets[entry.target] = 0
        else:
            targets[entry.target] += 1

    html = "<ul>"

    for target, count in targets.items():
        html += "<li>" + target + ": " + str(count) + "</li>"

    html += "</ul>"

    return HttpResponse(html)
